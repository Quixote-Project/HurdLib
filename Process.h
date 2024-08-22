/** \file
 *
 */

/**************************************************************************
 * Copyright (c) 2024, Enjellic Systems Development, LLC. All rights reserved.
 *
 *
 * Please refer to the file named COPYING in the top of the source tree
 * for licensing information.
 **************************************************************************/

#ifndef HurdLib_Process_HEADER
#define HurdLib_Process_HEADER


/* Object type definitions. */
typedef struct HurdLib_Process * Process;

typedef struct HurdLib_Process_State * Process_State;

/**
 * External Process object representation.
 */
struct HurdLib_Process
{
	/* External methods. */
	_Bool (*add_argument)(const Process, const char *);
	_Bool (*set_executable)(const Process, const char *);

	_Bool (*run)(const Process);
	_Bool (*run_command)(const Process, int, char **);
	_Bool (*run_command_line)(const Process, int, char **);

	void (*whack)(const Process);

	/* Private state. */
	Process_State state;
};


/* Process constructor call. */
extern Process HurdLib_Process_Init(void);

#endif
