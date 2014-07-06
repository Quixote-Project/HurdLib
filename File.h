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
typedef struct HurdLib_File * File;

typedef struct HurdLib_File_State * File_State;

/**
 * External HurdLib_File object representation.
 */
struct HurdLib_File
{
	/* External methods. */
	_Bool (*open_ro)(const File, const char *);
	_Bool (*open_rw)(const File, const char *);
	_Bool (*open_wo)(const File, const char *);

	_Bool (*read_Buffer)(const File, const Buffer, size_t);
	_Bool (*slurp)(const File, const Buffer);
	_Bool (*read_String)(const File, const String);
	_Bool (*write_Buffer)(const File, const Buffer);

	off_t (*seek)(const File, off_t);

	void (*reset)(const File);
	_Bool (*poisoned)(const File);
	void (*whack)(const File);

	/* Private state. */
	File_State state;
};


/* HurdLib_File constructor call. */
extern File HurdLib_File_Init(void);

#endif
