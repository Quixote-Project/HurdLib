/** \file
 * This file contains an implementation of an object for carrying out
 * common file manipulations.
 */

/**************************************************************************
 * (C)Copyright 2014, The Open Hurderos Foundation. All rights reserved.
 *
 * Please refer to the file named COPYING in the top of the source tree
 * for licensing information.
 **************************************************************************/

#ifndef HurdLib_File_HEADER
#define HurdLib_File_HEADER


/* Object type definitions. */
typedef struct HurdLib_File * HurdLib_File;

typedef struct HurdLib_File_State * HurdLib_File_State;

/**
 * External HurdLib_File object representation.
 */
struct HurdLib_File
{
	/* External methods. */
	_Bool (*open_ro)(const HurdLib_File, const char *);
	_Bool (*open_rw)(const HurdLib_File, const char *);

	_Bool (*read_buffer)(const HurdLib_File, const Buffer, size_t);
	_Bool (*slurp)(const HurdLib_File, const Buffer);
	_Bool (*write_buffer)(const HurdLib_File, const Buffer);

	off_t (*seek)(const HurdLib_File, off_t);

	void (*reset)(const HurdLib_File);
	_Bool (*poisoned)(const HurdLib_File);
	void (*whack)(const HurdLib_File);

	/* Private state. */
	HurdLib_File_State state;
};


/* HurdLib_File constructor call. */
extern HurdLib_File HurdLib_File_Init(void);

#endif
