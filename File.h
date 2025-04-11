/** \file
 * This file contains an implementation of an object for carrying out
 * common file manipulations.
 */

/**************************************************************************
 * Copyright (c) 2020, Enjellic Systems Development, LLC. All rights reserved.
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
	_Bool (*write_String)(const File, const String);

	off_t (*seek)(const File, off_t);

	int (*error)(const File);
	void (*reset)(const File);
	void (*clear)(const File);
	_Bool (*poisoned)(const File);
	void (*whack)(const File);

	/* Private state. */
	File_State state;
};


/* HurdLib_File constructor call. */
extern HCLINK File HurdLib_File_Init(void);

#endif
