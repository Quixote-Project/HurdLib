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

#include "HurdLib.h"
#include "Buffer.h"
#include "Gaggle.h"


/*
 * Program entry point.
 */

extern int main(int argc, char *argv[])

{
	int rc = 1;

	unsigned int lp;

	Buffer bufr = NULL;

	Gaggle gaggle = NULL;

	static unsigned char b1[2] = { 0xfe, 0xad };

	static unsigned char b2[2] = { 0xbe, 0xaf };
	
	
	INIT(HurdLib, Gaggle, gaggle, goto done);

	INIT(HurdLib, Buffer, bufr, goto done);
	if ( !bufr->add(bufr, b1, sizeof(b1)) )
		goto done;
	fprintf(stdout, "Adding 0: %p\n", bufr);
	bufr->hprint(bufr);
	if ( !GADD(gaggle, bufr) )
		goto done;

	INIT(HurdLib, Buffer, bufr, goto done);
	if ( !bufr->add(bufr, b2, sizeof(b2)) )
		goto done;
	fprintf(stdout, "\nAdding 1: %p\n", bufr);
	bufr->hprint(bufr);
	if ( !GADD(gaggle, bufr) )
		goto done;


	/* Verify objects that were added. */
	for (lp= 0; lp < gaggle->size(gaggle); ++lp) {
		fprintf(stdout, "\nGetting object: %u\n", lp);
		bufr = GGET(gaggle, bufr);
		fprintf(stdout, "Returned: %p\n", bufr);
		bufr->hprint(bufr);
	}

	rc = 0;


 done:
	fputs("\nReleasing objects.\n", stdout);
	GWHACK(gaggle, bufr);

	WHACK(gaggle);

	return rc;
}			     
