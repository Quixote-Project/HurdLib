/** \file
 * This file contains a unit test for the Config object.
 */

/**************************************************************************
 * Copyright (c) 2022, Enjellic Systems Development, LLC. All rights reserved.
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
#include "Config.h"


/*
 * Program entry point.
 */

extern int main(int argc, char *argv[])

{
	int rc = 1;

	Config cfg = NULL;


	if ( argv[1] == NULL ) {
		fputs("No configuration file specified.\n", stderr);
		goto done;
	}


	INIT(HurdLib, Config, cfg, goto done);

	if ( !cfg->parse(cfg, argv[1]) ) {
		fputs("Failed to parse configuration file.\n", stderr);
		goto done;
	}

	cfg->dump(cfg);


 done:
	WHACK(cfg);

	return rc;
}
