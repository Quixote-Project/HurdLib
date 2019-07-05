/**************************************************************************
 * (C)Copyright 2005, The Open Hurderos Foundation. All rights reserved.
 *
 * This file contains the definition of a flex parser used to get
 # basic KerDAP configuration information.
 *
 * Please see bottom of file for change information.
 **************************************************************************/


/* Include common definitions for the configuration system. */
%{
#include <stdbool.h>

#include "HurdLib.h"
#include "Config.h"
%}


/* Disable unput functionality. */
%option nounput


/*
 * Specify an inclusive start condition for the variable value start
 * condition.
 */
%x VALUE
%x QUOTE


/*
 * Define the yylex routine to take a pointer to a configuration structure
 * as its arguement.
*/
%{
#define YY_DECL int yylex_basic(Config config)
%}


%%
			auto _Bool r;

#.*$

[ \t\n]+

\[[[:alnum:]_ ]+\]	{	r = config->add_section(config, yytext);
				if ( !r )
					return false;
			}

=			BEGIN(VALUE);

[[:alnum:]_]+       	{	r = config->add_variable(config, yytext);
				if ( !r )
					return false;
			}


[^ \t\n=a-zA-Z_]


<VALUE>[ \t]+

<VALUE>[\n]+		BEGIN(INITIAL);

<VALUE>["']		BEGIN(QUOTE);

<VALUE>[0-9a-zA-Z.,=_]+	{	r = config->add_value(config, yytext);
				if ( !r )
					return false;
				BEGIN(INITIAL);
			}


<QUOTE>["']		BEGIN(INITIAL);

<QUOTE>[^"']+		{	r = config->add_value(config, yytext);
				if ( !r )
					return false;
			}

<<EOF>>			return true;

%%
