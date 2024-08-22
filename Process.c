/** \file
 * This file contains the implementation of an object that manages
 * the execution of a subordinate process.
 */

/**************************************************************************
 * Copyright (c) 2024, Enjellic Systems Development, LLC. All rights reserved.
 *
 * Please refer to the file named COPYING in the top of the source tree
 * for licensing information.
 **************************************************************************/

/* Include files. */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <stdio.h>

#include "Origin.h"
#include "HurdLib.h"
#include "Buffer.h"
#include "String.h"
#include "Gaggle.h"
#include "Process.h"


/* State initialization macro. */
#define STATE(var) CO(Process_State, var) = this->state


/* Verify library/object header file inclusions. */
#if !defined(HurdLib_LIBID)
#error Library identifier not defined.
#endif

#if !defined(HurdLib_Process_OBJID)
#error Object identifier not defined.
#endif


/** Process private state information. */
struct HurdLib_Process_State
{
	/* The root object. */
	Origin root;

	/* Library identifier. */
	uint32_t libid;

	/* Object identifier. */
	uint32_t objid;

	/* Object status. */
	_Bool poisoned;

	/* Flag variable to indicate executable name has been specified. */
	_Bool have_executable;

	/* Buffer object holding argv array. */
	Buffer argv;

	/* Gaggle object holding argument strings. */
	Gaggle args;
};


/**
 * Internal private method.
 *
 * This method is responsible for initializing the HurdLib_Process_State
 * structure which holds state information for each instantiated object.
 *
 * \param S A pointer to the object containing the state information which
 *        is to be initialized.
 */

static void _init_state(CO(Process_State, S)) {

	S->libid = HurdLib_LIBID;
	S->objid = HurdLib_Process_OBJID;

	S->poisoned = false;
	S->have_executable = false;

	S->argv = NULL;
	S->args = NULL;

	return;
}


/**
 * External public method.
 *
 * This method implements adding a string value to the argv array.
 *
 * \param this	A pointer to the object whose argument is to be set.
 *
 * \param arg	A pointer to the null-terminated character buffer containing
 *		the name of the argument.
 *
 * \return	A boolean value is used to indicate whether or not the
 *		argument was added.  A false value indicates the addition
 *		failed while a true value indicates the argument was set.
 */

static _Bool add_argument(CO(Process, this), CO(char *, arg))

{
	STATE(S);

	_Bool retn = false;

	unsigned char bufr[sizeof(void *)];

	String str = NULL;


	/* Verify object status. */
	if ( S->poisoned )
		goto done;
	if ( !S->have_executable )
		goto done;


	/* Add a slot for argv pointer. */
	memset(bufr, '\0', sizeof(bufr));
	if ( !S->argv->add(S->argv, bufr, sizeof(bufr)) )
		goto done;
	if ( arg == NULL ) {
		retn = true;
		goto done;
	}


	/* Add the string to the list of arguments. */
	INIT(HurdLib, String, str, goto done);
	if ( !str->add(str, arg) )
		goto done;

	if ( !GADD(S->args, str) )
		goto done;
	retn = true;


 done:
	if ( !retn )
		WHACK(str);

	return retn;
}


/**
 * External public method.
 *
 * This method implements the specification of the executable.  It must
 * be called before any arguments are added with the ->add_argument method.
 *
 * \param this	A pointer to the object whose executable is to be
 *		set.
 *
 * \param arg	A pointer to the null-terminated character buffer containing
 *		the name of the executable.
 *
 * \return	A boolean value is used to indicate whether or not the
 *		executable name was set.  A false value indicates an error
 *		was encountered while setting the executable name while a
 *		true value indicates the executable name was set.
 */

static _Bool set_executable(CO(Process, this), CO(char *, executable))

{
	STATE(S);

	_Bool retn = false;


	/* Verify object status. */
	if ( S->poisoned )
		goto done;
	if ( S->have_executable )
		goto done;


	/* Add the name of the executable. */
	S->have_executable = true;
	if ( !this->add_argument(this, executable) )
		goto done;
	retn = true;


 done:
	return retn;
}


/**
 * External public method.
 *
 * This method implements the execution of the process that has been
 * configured.
 *
 * \param this	A pointer to the object which is to be run.
 *
 * \return	A boolean value is used to indicate whether or not the
 *		process was successfully started.  A false value indicates
 *		a failure.  In the case of a successful execution this
 *		method does not return.
 */

static _Bool run(CO(Process, this))

{
	STATE(S);

	_Bool retn = false;

	char **argv_base;

	unsigned int lp;

	String str;


	/* Verify object. */
	if ( S->poisoned )
		goto done;
	if ( !S->have_executable )
		goto done;


	/* Add the pointers to the new argv variable including a NULL. */
	if ( !this->add_argument(this, NULL) )
		goto done;
	argv_base = (char **) S->argv->get(S->argv);

	for (lp= 0; lp < S->args->size(S->args); ++lp) {
		str = GGET(S->args, str);
		argv_base[lp] = str->get(str);
	}


	/* Execute the process. */
	execvp(argv_base[0], argv_base);


 done:
	return retn;
}


/**
 * External public method.
 *
 * This method implements the execution of a command specified by an
 * array of character pointers similar to what would be available from
 * the argv command-line arguments.
 *
 * \param this	A pointer to the object which is to be run.
 *
 * \param argc	The number of arguments to be processed from the
 *		command-line argument array.
 *
 * \param argv	A pointer to an array of strings that define the command
 *		to be run.
 *
 * \return	A boolean value is used to indicate whether or not the
 *		command was successfully run.  A false value indicates
 *		a failure.  In the case of a successful execution this
 *		method does not return.
 */

static _Bool run_command(CO(Process, this), int argc, char *argv[])

{
	STATE(S);

	_Bool retn = false;

	unsigned int lp;


	/* Verify object. */
	if ( S->poisoned )
		goto done;
	if ( argc < 1 )
		goto done;


	/* Specify the executable as the first argument. */
	if ( !this->set_executable(this, argv[0]) )
		goto done;


	/* Add the remainder of the arguments. */
	for (lp= 1; lp < argc; ++lp) {
		if ( !this->add_argument(this, argv[lp]) )
			goto done;
	}


	/* Execute the command. */
	this->run(this);


 done:
	return retn;
}


/**
 * External public method.
 *
 * This method implements the execution of a command that was specified
 * on the command-line by by the -- character pair.
 *
 * \param this	A pointer to the object which is to be run.
 *
 * \parm  argc	The number of arguments in the argv array.
 *
 * \param argv	A pointer to an array of strings from the command-line
 *		of the program that was invoked.
 *
 * \return	A boolean value is used to indicate whether or not the
 *		command was successfully run.  A false value indicates
 *		a failure.  In the case of a successful execution this
 *		method does not return.
 */

static _Bool run_command_line(CO(Process, this), int argc, char *argv[])

{
	STATE(S);

	_Bool retn = false;

	unsigned int argv_size;


	/* Verify object. */
	if ( S->poisoned )
		goto done;
	if ( argc < 1 )
		goto done;


	/* Locate start of command line arguments. */
	for (argv_size= 0; argv_size < argc; ++argv_size) {
		if ( strcmp(argv[argv_size], "--") == 0 )
			break;
	}

	if ( argv_size == argc )
		goto done;

	if ( strcmp(argv[argv_size], "--") == 0 ) {
		++argv_size;
		if ( argv[argv_size] == NULL )
			goto done;
	}


	/* Execute the command-line. */
	argc -= argv_size;
	if ( !this->run_command(this, argc, &argv[argv_size]) )
			goto done;


 done:
	return retn;
}


/**
 * External public method.
 *
 * This method implements a destructor for a Process object.
 *
 * \param this	A pointer to the object which is to be destroyed.
 */

static void whack(CO(Process, this))

{
	STATE(S);


	WHACK(S->argv);
	GWHACK(S->args, String);

	S->root->whack(S->root, this, S);
	return;
}


/**
 * External constructor call.
 *
 * This function implements a constructor call for a Process object.
 *
 * \return	A pointer to the initialized Process.  A null value
 *		indicates an error was encountered in object generation.
 */

extern Process HurdLib_Process_Init(void)

{
	Origin root;

	Process this = NULL;

	struct HurdLib_Origin_Retn retn;


	/* Get the root object. */
	root = HurdLib_Origin_Init();

	/* Allocate the object and internal state. */
	retn.object_size  = sizeof(struct HurdLib_Process);
	retn.state_size   = sizeof(struct HurdLib_Process_State);
	if ( !root->init(root, HurdLib_LIBID, HurdLib_Process_OBJID, &retn) )
		return NULL;
	this	    	  = retn.object;
	this->state 	  = retn.state;
	this->state->root = root;

	/* Initialize object state. */
	_init_state(this->state);

	/* Initialize aggregate objects. */
	INIT(HurdLib, Buffer, this->state->argv, goto fail);
	INIT(HurdLib, Gaggle, this->state->args, goto fail);


	/* Method initialization. */
	this->add_argument   = add_argument;
	this->set_executable = set_executable;

	this->run	       = run;
	this->run_command      = run_command;
	this->run_command_line = run_command_line;

	this->whack = whack;

	return this;


fail:
	WHACK(this->state->argv);
	WHACK(this->state->args);

	root->whack(root, this, this->state);
	return NULL;
}
