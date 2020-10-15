/** \file
 * This file contains the implementation of a Config object.  This object
 * implements a system for parsing and retrieving information from a
 * multi-section configuration file.
 * 
 */

/**************************************************************************
 * Copyright (c) 2020, Enjellic Systems Development, LLC. All rights reserved.
 **************************************************************************/

/* Include files. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "HurdLib.h"
#include "Origin.h"
#include "Config.h"


/* Verify library/object header file inclusions. */
#if !defined(HurdLib_LIBID)
#error Library identifier not defined.
#endif

#if !defined(HurdLib_Config_OBJID)
#error Object identifier not defined.
#endif


/* Type declarations. */
struct cfg_value
{
	char *name;
	char *value;
};

struct section
{
	char *name;

	int size;
	int current;
	struct cfg_value **elements;
};


/** Config private state information. */
struct HurdLib_Config_State
{
	/* The root object. */
	Origin root;

	/* Library identifier. */
	uint32_t libid;

	/* Object identifier. */
	uint32_t objid;

	/* The number of sections. */
	int size;

	/* The section which is currently active. */
	int current;
	   
	/* The array of configuration sections. */
	struct section **sections;
};


/* Variable declarations. */
extern FILE *yyin;


/* External function declarations. */
extern int yylex_basic(Config);


/**
 * Internal private method.
 *
 * This method is responsible for initializing the HurdLib_Config_State
 * structure which holds state information for each instantiated object.
 *
 * \param S	A pointer to the object containing the state information
 *		which is to be initialized.
 */

static void _init_state(Config_State const S) {

	S->libid = HurdLib_LIBID;
	S->objid = HurdLib_Config_OBJID;

	S->size	    = 1;
	S->current  = 0;
	S->sections = NULL;

	return;
}


/**
 * Private function
 *
 * This function is responsible for allocating or re-allocating the
 * sections array in the Config action.  This array contains pointers
 * to individual arrays of variables.
 *
 * \param cp	A pointer to the configuration object which is having
 *		its sections array allocated.
 *
 * \return	A boolean value is returned to indicate whether or not
 *		the allocation was successful.  A false value implies an
 *		error.
 */

static _Bool _allocate_section(const Config_State const S, char *name)

{
	auto struct section *sp,
		            **p;

	auto size_t amt = sizeof(struct section *) * S->size;


	/* Allocate or re-allocate memory and check for errors. */
	if ( (p = realloc(S->sections, amt)) == NULL )
		return false;


	/* Allocate and initialize a section. */
	sp = malloc(sizeof(struct section));
	if ( sp == NULL )
		return false;

	sp->name = name;
	if ( sp->name != NULL ) {
		sp->name = malloc(strlen(name + 1));
		if ( sp->name == NULL ) {
			free(sp);
			return false;
		}

		memset(sp->name, '\0', strlen(name + 1));
		strncpy(sp->name, name + 1, strlen(name + 1) - 1);
	}

	sp->size     = 0;
	sp->current  = -1;
	sp->elements = NULL;


	/* Return success. */
	S->sections = p;
	S->sections[S->current] = sp;
	return true;
}


/**************************************************************************
 * Function:	_allocate_elements
 *
 * Purpose:	This function is responsible for allocating memory for
 *		the array of configuration elements.  The allocation
 *		is based on the size value in configuration object.
 *
 * Arguements:	(Config)
 *
 *		config: -> A pointer to a structure defining the
 *	                   configuration being parsed.
 *
 * Return:	int
 *
 *		A negative value is returned if an error was encountered
 *		while doing the allocation.  Otherwise a value of zero
 *		is returned.
 **************************************************************************/

static _Bool _allocate_elements(struct section *section)

{
	auto struct cfg_value **p;

	auto size_t amt = sizeof(struct cfg_value *) * (section->size + 1);


	/* Allocate or re-allocate memory and check for errors. */
	if ( (p = realloc(section->elements, amt)) == NULL )
		return false;


	/* Return success. */
	++section->size;
	section->elements = p;

	return true;
}


/**
 * Public method.
 *
 * This method implements the parsing of a configuration file.  The
 * sections and variable definitions in the file are converted into
 * elements of the private state structur.
 * 
 * \param this		A pointer to the configuration object which
 *			will be populated with information from the
 *			configuration file.
 *
 * \param cfgfile	A pointer to the name of the configuration file to
 *			be parsed.
 *
 * \return	A boolean value is returned to indicate whether or not
 *		the parsing of the file was successful.
 */

static _Bool parse(const Config const config, const char * const cfgfile)

{
	auto _Bool retn;


	/* Open configuration file. */
	if ( (yyin = fopen(cfgfile, "r")) == NULL )
		return false;

	/*
	 * Parse the file into the defined configuration. Then close
	 * the configuration file and return the results of the parse.
	 */
	retn = yylex_basic(config);
	fclose(yyin);

	return retn;
}


/**
 * Public method.
 *
 * This method is responsible for adding a declared variable to the
 * configuration.  It will be triggered by the detection of a symbol while
 * not in value mode.
 *
 * \param this		A pointer to the configuration object being
 *			populated with a variable.
 *
 * \param variable	A pointer to the buffer containing the string
 *			representation of the variable name.
 *
 * \return	A boolean value is returned to indicate the success or
 *		failure of the variable addition.
 *
 */

static _Bool add_variable(const Config const this, char *variable)

{
	auto Config_State S = this->state;

	auto struct cfg_value *cfp;

	auto struct section *section = S->sections[S->current];


	/* Allocate an variable element. */
	_allocate_elements(section);


	/*
	 * Allocate a configuration array element and copy the 
	 * the variable name.
	 */	
	if ( (cfp = malloc(sizeof(struct cfg_value))) == NULL )
		return false;

	if ( (cfp->name = strdup(variable)) == NULL )
	{
		free(cfp);
		return false;
	}
	cfp->value = NULL;


	/* Success. */
	section->elements[++section->current] = cfp;
	return true;
}


/**
 * Public method.
 *
 * This method is responsible for adding the value of a configuration
 * variable to a previously declared variable.
 *
 * \param this		A pointer to the configuration object which is
 *			having a variable value set.
 *
 * \param value		A character pointer to the null-terminated buffer
 *			containing the value to add to the last
 *			variable which has been defined.
 *
 * \return	A boolean value is returned to indicate the success or
 *		failure of the addition of the value.
 */

static _Bool add_value(const Config const this, char * const value)

{
	auto Config_State S = this->state;

	auto struct section *section = S->sections[S->current];

	auto struct cfg_value *elptr = section->elements[section->current];


	/* Add a value to the current variable. */
	if ( (elptr->value = strdup(value)) == NULL )
		return false;

	return true;
}


/**
 * Public method.
 *
 * This function implements a method for adding a new variable section
 * to a configuration.  This section will become the default section for
 * additional variables.
 *
 * \param config	A pointer to the configuration to which a new
 *			section is being added.
 *
 * \param name		A pointer to a character buffer containing the
 *			section name.
 *
 * \return		A boolean value is returned to indicate whether
 *			or not the addition of the section was succesful.
 *			A true value indicates succes.
v */

static _Bool add_section(const Config const this, char *name)

{
	auto Config_State S = this->state;


	++S->current;
	++S->size;

	if ( !_allocate_section(S, name) ) {
		--S->current;
		--S->size;
		return false;
	}

	return true;
}


/**
 * Public method.
 *
 * This method is responsible for setting the default section which will
 * be accessed for subsequent variable lookups.
 *
 * \param config	A pointer to the configuration containing the
 *			section from which variables will be looked up.
 *
 * \return		A boolean value is used to indicated whether or
 *			not the specified section exists in the
 *			configuration.  A true value indicates the
 *			section was found.
 */

extern _Bool set_section(const Config const this, const char *name)

{
	auto Config_State S = this->state;

	auto unsigned int secnumber;

	auto struct section **sections = S->sections;


	/* Handle the special case of the default section. */
	if ( (name == NULL) | (strcmp(name, "Default") == 0) ) {
		S->current = 0;
		return true;
	}


	/*
	 * Loop through the remaining section entries looking for the
	 * section name provided by the caller.
	 */
	for (secnumber= 1; secnumber < S->size; ++secnumber) {
		if ( strcmp(sections[secnumber]->name, name) == 0 ) {
			S->current = secnumber;
			return true;
		}
	}
		
	return false;
}


/**
 * Public method.
 *
 * This method provides the interface for retrieving the value of a
 * variable with the specified name.  The currently selected section
 * will be used to search for the variable. The caller should use the
 * set_section method call if anything other than the default section
 * should be searched.
 *
 * \param this		A pointer to the configuration object which
 *			is to be searched.
 *
 * \param variable	A pointer to the null-terminated character buffer
 *			containing the name of the variable to be
 *			searched for.
 *
 * \return	If the variable is not found a null value is returned.
 *		Otherwise a pointer to the value of the variable is
 *		returned.
 **************************************************************************/

static char * get(const Config const this, const char *varname)

{
	auto Config_State S = this->state;

	auto int lp;

	auto struct section *section = S->sections[S->current];


	/* Sanity check. */
	if ( section->current == -1 )
		return NULL;
	

	/*
	 * Loop through the array of defined variables looking for
	 * an entry which matches the name specified in the function
	 * call.
	 */
	for (lp= 0; lp <= section->current; ++lp) {
		if ( strcmp(section->elements[lp]->name, varname) == 0 )
			return section->elements[lp]->value;
	}

	return NULL;
}


/**
 * Public method.
 *
 * This method provides the interface for retrieving the value of a
 * variable specified with a case insensitive variable name.  The currently
 * selected section will be used to search for the variable. The caller
 *should use the set_section method call if anything other than the default
 * section should be searched.
 *
 * \param this		A pointer to the configuration object which
 *			is to be searched.
 *
 * \param variable	A pointer to the null-terminated character buffer
 *			containing the name of the variable to be
 *			searched for.
 *
 * \return	If the variable is not found a null value is returned.
 *		Otherwise a pointer to the value of the variable is
 *		returned.
 **************************************************************************/

static char * get_ignore(const Config const this, const char *varname)

{
	auto Config_State S = this->state;

	auto int lp;

	auto struct section *section = S->sections[S->current];


	/* Sanity check. */
	if ( section->current == -1 )
		return NULL;
	

	/*
	 * Loop through the array of defined variables looking for
	 * an entry which matches the name specified in the function
	 * call.
	 */
	for (lp= 0; lp <= section->current; ++lp) {
		if ( strcasecmp(section->elements[lp]->name, varname) == 0 )
			return section->elements[lp]->value;
	}

	return NULL;
}


/**
 * External method.
 *
 * This method implements printint out all of the informaton in a
 * configuration.
 *
 * \param this		A pointer to the configuration which is to
 *			be printed.
 */

static void dump(const Config const this)

{
	auto Config_State S = this->state;

	auto int lp,
		 lp1;

	auto struct section *section = S->sections[S->current];

	auto struct cfg_value *cfp;


	/*
	 * Output sections with their variable lists.
	 */
	fprintf(stdout, "%s:\n", __func__);
	for (lp= 0; lp < S->size; ++lp) {
		section = S->sections[lp];

		fprintf(stdout, "\tsection: %d\tname: ", lp);
		if ( section->name == NULL )
			fputs("Default\n", stdout);
		else
			fprintf(stdout, "%s\n", section->name);

		for (lp1= 0; lp1 <= section->current; ++lp1) {
			cfp = section->elements[lp1];
			fprintf(stdout, "\t\t%s -> %s\n", cfp->name, \
				cfp->value);
		}
		fputs("\n", stdout);
	}


	return;
}


/**
 * External public method.
 *
 * This method implements a destructor for a Config object.
 *
 * \param this	A pointer to the object which is to be destroyed.
 */

static void whack(const Config const this)

{
	auto const Config_State const S = this->state;

	auto int lp,
		 lp1;

	auto struct section *section;

	auto struct cfg_value *cfp;


	/* Release the variables in each section and then the section array. */
	for (lp= 0; lp < S->size; ++lp) {
		section = S->sections[lp];

		for (lp1= 0; lp1 < section->size; ++lp1) {
			cfp = section->elements[lp1];
			free(cfp->value);
			free(cfp->name);
			free(cfp);
		}

		free(section);
	}

	free(S->sections);


	/* Then the object itself. */
	S->root->whack(S->root, this, S);
	return;
}

	
/**
 * External constructor call.
 *
 * This function implements a constructor call for a Config object.
 *
 * \return	A pointer to the initialized Config.  A null value
 *		indicates an error was encountered in object generation.
 */

extern Config HurdLib_Config_Init(void)

{
	auto Origin root;

	auto Config this = NULL;

	auto struct HurdLib_Origin_Retn retn;


	/* Get the root object. */
	root = HurdLib_Origin_Init();

	/* Allocate the object and internal state. */
	retn.object_size  = sizeof(struct HurdLib_Config);
	retn.state_size   = sizeof(struct HurdLib_Config_State);
	if ( !root->init(root, HurdLib_LIBID, HurdLib_Config_OBJID, &retn) )
		return NULL;
	this	    	  = retn.object;
	this->state 	  = retn.state;
	this->state->root = root;

	/* Initialize object state. */
	_init_state(this->state);

	/* Initialize aggregate objects. */
	if ( !_allocate_section(this->state, NULL) ) {
		root->whack(root, this, this->state);
		return NULL;
	}

	/* Method initialization. */
	this->parse		= parse;
	this->add_variable	= add_variable;
	this->add_value		= add_value;
	this->add_section	= add_section;
	this->set_section	= set_section;
	this->get		= get;
	this->get_ignore	= get_ignore;
	this->dump		= dump;
	this->whack		= whack;

	return this;
}
