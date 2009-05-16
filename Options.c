/*****************************************************************************
 * $Source$
 *
 * $Author$
 *
 * $Revision$
 *
 * $State$
 *
 * $Date$
 *
 * (C)Copyright 2005, Open Hurderos Foundation.  All rights reserved.
 ****************************************************************************/


/* Include files. */
#include <stdio.h>
#include <stdlib.h>

#include "Origin.h"
#include "Options.h"


/* Internal type definitions. */
struct config_arg
{
	int present;
	char *arg;
};

struct HURD_Options_State
{
	/* The root object. */
	Origin root;

	/* The object identity. */
	int id;

	struct config_arg o1;
	struct config_arg o2;
	struct config_arg o3;
	struct config_arg o4;
	struct config_arg o5;
	struct config_arg o6;
	struct config_arg o7;
	struct config_arg o8;
	struct config_arg o9;

	struct config_arg a;
	struct config_arg b;
	struct config_arg c;
	struct config_arg d;
	struct config_arg e;
	struct config_arg f;
	struct config_arg g;
	struct config_arg h;
	struct config_arg i;
	struct config_arg j;
	struct config_arg k;
	struct config_arg l;
	struct config_arg m;
	struct config_arg n;
	struct config_arg o;
	struct config_arg p;
	struct config_arg q;
	struct config_arg r;
	struct config_arg s;
	struct config_arg t;
	struct config_arg u;
	struct config_arg v;
	struct config_arg w;
	struct config_arg x;
	struct config_arg y;
	struct config_arg z;

	struct config_arg A;
	struct config_arg B;
	struct config_arg C;
	struct config_arg D;
	struct config_arg E;
	struct config_arg F;
	struct config_arg G;
	struct config_arg H;
	struct config_arg I;
	struct config_arg J;
	struct config_arg K;
	struct config_arg L;
	struct config_arg M;
	struct config_arg N;
	struct config_arg O;
	struct config_arg P;
	struct config_arg Q;
	struct config_arg R;
	struct config_arg S;
	struct config_arg T;
	struct config_arg U;
	struct config_arg V;
	struct config_arg W;
	struct config_arg X;
	struct config_arg Y;
	struct config_arg Z;
};


/* Static function declarations. */
static void print(Options);
static void whack(Options);


/**
 * Internal private method.
 *
 * This method is responsible for initializing the Hurd_Options_State
 * structure which holds the state information for each instantiated
 * object.
 *
 * @state A pointer to the object containing the state information which
 *  is to be initialized.
 */

static void _init_state(Options_State state) {

	state->id = HURD_OPTIONS_ID;

	/* Numeric options. */
	state->o1.present = 0;
	state->o1.arg = NULL;

	state->o2.present = 0;
	state->o2.arg = NULL;

	state->o3.present = 0;
	state->o3.arg = NULL;

	state->o4.present = 0;
	state->o4.arg = NULL;

	state->o5.present = 0;
	state->o5.arg = NULL;

	state->o6.present = 0;
	state->o6.arg = NULL;

	state->o7.present = 0;
	state->o7.arg = NULL;

	state->o8.present = 0;
	state->o8.arg = NULL;

	state->o9.present = 0;
	state->o9.arg = NULL;


	/* Lower case options. */
	state->a.present = 0;
	state->a.arg = NULL;

	state->b.present = 0;
	state->b.arg = NULL;

	state->c.present = 0;
	state->c.arg = NULL;

	state->d.present = 0;
	state->d.arg = NULL;

	state->e.present = 0;
	state->e.arg = NULL;

	state->f.present = 0;
	state->f.arg = NULL;

	state->g.present = 0;
	state->g.arg = NULL;

	state->h.present = 0;
	state->h.arg = NULL;

	state->i.present = 0;
	state->i.arg = NULL;

	state->j.present = 0;
	state->j.arg = NULL;

	state->k.present = 0;
	state->k.arg = NULL;

	state->l.present = 0;
	state->l.arg = 0;

	state->m.present = 0;
	state->m.arg = 0;

	state->n.present = 0;
	state->n.arg = NULL;

	state->o.present = 0;
	state->o.arg = NULL;

	state->p.present = 0;
	state->p.arg = NULL;

	state->q.present = 0;
	state->q.arg = NULL;

#if 0
	r;
	s;
	t;
	u;
	v;
	w;
	x;
	y;
	z;

	struct config_arg A;
	struct config_arg B;
	struct config_arg C;
	struct config_arg D;
	struct config_arg E;
	struct config_arg F;
	struct config_arg G;
	struct config_arg H;
	struct config_arg I;
	struct config_arg J;
	struct config_arg K;
	struct config_arg L;
	struct config_arg M;
	struct config_arg N;
	struct config_arg O;
	struct config_arg P;
	struct config_arg Q;
	struct config_arg R;
	struct config_arg S;
	struct config_arg T;
	struct config_arg U;
	struct config_arg V;
	struct config_arg W;
	struct config_arg X;
	struct config_arg Y;
	struct config_arg Z;
#endif

	return;
}


/**
 * Implements a method for printing out the identity of the object.
 *
 * @this A pointer to the Options object being operated on.
 *
 * @return No return value is defined.
 */

static void print(Options this)

{
	printf("Object id: %d\n", this->state->id);
	return;
}


/**
 * This function implements the destructor method for the calling
 * object.
 */

static void whack(Options this)

{
	printf("Object[method]: %s[%s]\n", __FILE__, __FUNCTION__);
	return;
}

	
/**
 * This function is responsible for initializing a structure which will
 * track and maintain the state of options passed to a program.
 */

extern Options HURD_Options_Init(void)

{
	auto Origin root;

	auto Options this = NULL;

	auto struct HurdLib_Origin_Retn retn;


	/* Get the root object. */
	root = HurdLib_Origin_Init();

	/* Allocate the object and internal state. */
	retn.objamt   = sizeof(struct HURD_Options);
	retn.stateamt = sizeof(struct HURD_Options_State);
	root->init(root, 1, 1, &retn);
	this		  = retn.object;
	this->state	  = retn.state;
	this->state->root = root;

	/* Initialize object state. */
	_init_state(this->state);

	this->print = print;
	this->whack = whack;

	return(this);
}


/**
 * This function implements a destructor for the object which tracks and
 * maintains the state of options passed to a program.  It de-allocates
 * all storage which was allocated as part of the object initializer.
 */

extern void HURD_Options_Destroy(Options options)

{
	if ( options != NULL )
		free(options);
	return;
}


/*
 * $Log$
 */
