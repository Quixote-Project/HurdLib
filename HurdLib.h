/** \file
 * This file is the object identifier file for the HurdLib object
 * library.
 */

/**************************************************************************
 * Copyright (c) 2020, Enjellic Systems Development, LLC. All rights reserved.
 **************************************************************************/

#ifndef HurdLib_HEADER
#define HurdLib_HEADER

/* Macro for defining C-linkage for object initialization. */
#if __cplusplus
#define HCLINK "C"
#else
#define HCLINK
#endif

/*
 * The following definitions are used to implement source size
 * optimizations for various commonly used routines.
 */

/* Macro for defining a pointer to a constant object. */
#define CO(obj, var) const obj const var

/* Macro to implement safe object destruction. */
#define WHACK(obj) if (obj != NULL) {obj->whack(obj); obj = NULL;}

/* No argument object initialization macros. */
#define _CCALL(lib,obj,init) lib##_##obj##_##init
#define INIT(lib, obj, var, action) \
	if ( (var = _CCALL(lib,obj,Init)()) == NULL ) action

/* Macro to clear and initialize the status of an object. */
#define OBJ_STATE(lib, obj) struct lib##_##obj##_##State

#define INIT_STATE(S, lib, obj)					   \
	memset(S + offsetof(OBJ_STATE(lib, obj), libid), '\0',	   \
		sizeof(*S) - offsetof(OBJ_STATE(lib,obj), libid)); \
	S->libid = lib##_##LIBID;				   \
	S->objid = lib##_##obj##_##OBJID

/* Library memory debugging. */
#if defined(DMALLOC)
#include <dmalloc.h>
#endif


/* HurdLib numeric object identifier. */
#define HurdLib_LIBID 1


/* Numeric object identifiers. */
#define HurdLib_Origin_OBJID		1
#define HurdLib_Fibsequence_OBJID	2
#define HurdLib_Buffer_OBJID		3
#define HurdLib_String_OBJID		4
#define HurdLib_Config_OBJID		5
#define HurdLib_File_OBJID		6
#define HurdLib_Gaggle_OBJID		7
#define HurdLib_Process_OBJID		8
#endif
