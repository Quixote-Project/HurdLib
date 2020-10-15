/**************************************************************************
 * Copyright (c) 2020, Enjellic Systems Development, LLC. All rights reserved.
 **************************************************************************/

/* Object identification. */
#define HURD_OPTIONS_ID 0x00000100


/* Object type definitions. */
typedef struct HURD_Options * Options;

typedef struct HURD_Options_State * Options_State;

struct HURD_Options
{
	/* External methods. */
	void (*print)(Options);
	void (*whack)(Options);

	/* Private state. */
	Options_State state;
};


/* External function definitions. */
extern HCLINK Options HURD_Options_Init(void);
