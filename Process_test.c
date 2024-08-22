/** \file
 * This file contains a unit test for the Gaggle object.
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
#include "Process.h"


/*
 * Program entry point.
 */

extern int main(int argc, char *argv[])

{
	int rc = 1;

	Process process = NULL;


	INIT(HurdLib, Process, process, goto done);


	/* Default is to execute the currently defined SHELL. */
	if ( argc == 1 ) {
		fputs("No command specified, running SHELL.\n", stdout);
		fflush(stdout);

		if ( !process->set_executable(process, getenv("SHELL")) )
			goto done;
		if ( !process->run(process) )
			goto done;
		rc = 0;
		goto done;
	}


	/* The -C argument executes the remainder of the command-line. */
	if ( strcmp(argv[1], "-C") == 0 ) {
		fputs("Running command line.\n", stdout);
		fflush(stdout);

		if ( !process->run_command(process, argc - 2, &argv[2]) )
			goto done;

		rc = 0;
		goto done;
	}


	/* The -X arguments executes the command delimited by --. */
	if ( strcmp(argv[1], "-X") == 0 ) {
		fputs("Running -- delimited command.\n", stdout);
		fflush(stdout);

		if ( !process->run_command_line(process, argc, argv) )
			goto done;

		rc = 0;
		goto done;
	}

	fprintf(stdout, "Unknown command mode: %s\n", argv[1]);
	rc = 2;


 done:
	WHACK(process);

	if ( rc == 1 )
		fputs("Failed execution.\n", stderr);

	return rc;
}
