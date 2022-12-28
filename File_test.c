/** \file
 * This file contains a unit test for the File object.
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
#include "Buffer.h"
#include "String.h"
#include "File.h"


/*
 * Program entry point.
 */

extern int main(int argc, char *argv[])

{
	int rc = 1;

	String str = NULL;

	File file = NULL;

	static const char *filename = "File_test.txt";


	INIT(HurdLib, String, str, goto done);
	if ( !str->add(str, "Test string\n") ) {
		fputs("Unable to add test string to file.\n", stderr);
		goto done;
	}

	INIT(HurdLib, File, file, goto done);
	if ( !file->open_rw(file, filename) ) {
		fputs("Unable to open test file.\n", stderr);
		goto done;
	}

	if ( !file->write_String(file, str) ) {
		fputs("Unable to write string to test file.\n", stderr);
		goto done;
	}
	fprintf(stdout, "Wrote: '%s'\n", str->get(str));


	/* Test reading of the file. */
	file->reset(file);
	if ( !file->open_ro(file, filename) ) {
		fputs("Unable to open test file read-only.\n", stderr);
		goto done;
	}

	str->reset(str);
	if ( !file->read_String(file, str) ) {
		fputs("Unable to read test file.\n", stderr);
		goto done;
	}

	fprintf(stdout, "Read: '%s'\n", str->get(str));
	rc = 0;


 done:
	WHACK(str);
	WHACK(file);

	return rc;
}
