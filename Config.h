/** \file
 * This file contains API definitions for the Config object which implements
 * parsing of a multi-section configuration file.  It should be included by
 * any applications which desire to create or use this object.
 */

/**************************************************************************
 * (C)Copyright 2009, The Open Hurderos Foundation. All rights reserved.
 *
 * Please refer to the file named COPYING in the top of the source tree
 * for licensing information.
 **************************************************************************/

#ifndef HurdLib_Config_HEADER
#define HurdLib_Config_HEADER


/* Object type definitions. */
typedef struct HurdLib_Config * Config;

typedef struct HurdLib_Config_State * Config_State;

/**
 * External Config object representation.
 */
struct HurdLib_Config
{
	/* External methods. */
	_Bool (*parse)(const Config, const char *);
	_Bool (*add_variable)(const Config, char *);
	_Bool (*add_value)(const Config, char *);
	_Bool (*add_section)(const Config, char *);
	_Bool (*set_section)(const Config, const char *);
	char * (*get)(const Config, const char *);
	char * (*get_ignore)(const Config, const char *);
	void (*dump)(const Config);
	void (*whack)(const Config);

	/* Private state. */
	Config_State state;
};


/* Config constructor call. */
extern HCLINK Config HurdLib_Config_Init(void);

#endif
