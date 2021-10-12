/** \file
 * This file contains a unit test for the String object.
 */

/**************************************************************************
 * Copyright (c) 2021, Enjellic Systems Development, LLC. All rights reserved.
 *
 *
 * Please refer to the file named COPYING in the top of the source tree
 * for licensing information.
 **************************************************************************/


/* Include files. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "HurdLib.h"
#include "String.h"


/*
 * Program entry point.
 */

extern int main(int argc, char *argv[])

{
	int rc = 1;

	char bf[512];

	String str = NULL;


	INIT(HurdLib, String, str, goto done);

	fputs("Initial string:\n", stdout);
	if ( !str->add(str, "test string") )
		goto done;
	str->print(str);

	snprintf(bf, sizeof(bf), "function = %s", __func__);
	fprintf(stdout, "\nGenerating formatted string: '%s' (%lu)\n", bf,
		strlen(bf));
	str->reset(str);
	if ( !str->add_sprintf(str, "function = %s", __func__) )
		goto done;
	str->print(str);

	snprintf(bf, sizeof(bf), ", line = %d", 100);
	fprintf(stdout, "\nAppending formatted string: '%s' (%lu)\n", bf, \
		strlen(bf));
	if ( !str->add_sprintf(str, ", line = %d", 100) )
		goto done;
	str->print(str);

	fputs("\nAppending formatted string:\n", stdout);
	if ( !str->add_sprintf(str, ", %s", "ERROR") )
		goto done;
	str->print(str);

	fputs("\nConcantenating a string:\n", stdout);
	if ( !str->add(str, ", added string.") )
		goto done;
	str->print(str);

	fputs("\nResetting and adding a formatted string.\n", stdout);
	str->reset(str);
	if ( !str->add_sprintf(str, "%s: added %02x, %f", __func__, 0xff, \
			       3.44) )
		goto done;
	str->print(str);

	rc = 0;



 done:
	WHACK(str);

	return rc;
}
