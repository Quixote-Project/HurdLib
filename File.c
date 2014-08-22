/** \file
 * This file contains the implementation header for the file
 * manipulation object.
 */

/**************************************************************************
 * (C)Copyright 2014, The Open Hurderos Foundation. All rights reserved.
 *
 * Please refer to the file named COPYING in the top of the source tree
 * for licensing information.
 **************************************************************************/

/* Local defines. */
/* Size of I/O buffer. */
#define FILE_BUFSIZE 4069

/* Include files. */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "HurdLib.h"
#include "Origin.h"
#include "Buffer.h"
#include "String.h"
#include "File.h"


/* Verify library/object header file inclusions. */
#if !defined(HurdLib_LIBID)
#error Library identifier not defined.
#endif

#if !defined(HurdLib_File_OBJID)
#error Object identifier not defined.
#endif

/* State initialization macro. */
#define STATE(var) CO(File_State, var) = this->state


/** HurdLib_File private state information. */
struct HurdLib_File_State
{
	/* The root object. */
	Origin root;

	/* Library identifier. */
	uint32_t libid;

	/* Object identifier. */
	uint32_t objid;

	/* Object status. */
	_Bool poisoned;

	/* Error code. */
	int error;

	/* File handle. */
	int fh;

	/* File buffer. */
	unsigned char bufr[FILE_BUFSIZE];
};


/**
 * Internal private method.
 *
 * This method is responsible for initializing the HurdLib_File_State
 * structure which holds state information for each instantiated object.
 *
 * \param S A pointer to the object containing the state information which
 *        is to be initialized.
 */

static void _init_state(const File_State const S) {

	S->libid = HurdLib_LIBID;
	S->objid = HurdLib_File_OBJID;

	S->poisoned = false;
	S->error    = 0;
	S->fh	    = -1;

	memset(S->bufr, '\0', sizeof(FILE_BUFSIZE));

	return;
}


/**
 * External public method.
 *
 * This method implements opening a file in read-only mode.
 *
 * \param this	A pointer to the object representing the file
 *		to be opened.
 *
 * \param fname	The pathname of the file to be opened.
 *
 * \return	A boolean value is returned to indicate the status of
 *		the file open.  A boolean indicates an error
 *		condition.
 */

static _Bool open_ro(const File const this, const char * const fname)

{
	const File_State const S = this->state;


	S->fh = open(fname, O_RDONLY);
	if ( S->fh == -1 ) {
		S->error    = errno;
		S->poisoned = true;
		return false;
	}

	return true;
}


/**
 * External public method.
 *
 * This method implements opening a file in read/write mode.
 *
 * \param this	A pointer to the object representing the file
 *		to be opened.
 *
 * \param fname	The pathname of the file to be opened.
 *
 * \return	A boolean value is returned to indicate the status of
 *		the file open.  A boolean indicates an error
 *		condition.
 */

static _Bool open_rw(const File const this, const char * const fname)

{
	const File_State const S = this->state;

	struct stat statbuf;


	if ( stat(fname, &statbuf) < 0 && (errno == ENOENT) ) {
		if ( creat(fname, S_IRUSR | S_IWUSR | S_IRGRP) < 0 ) {
			S->error    = errno;
			S->poisoned = true;
			return false;
		}
	}

	S->fh = open(fname, O_RDWR);
	if ( S->fh == -1 ) {
		S->error    = errno;
		S->poisoned = true;
		return false;
	}

	return true;
}


/**
 * External public method.
 *
 * This method implements opening a file in write-only mode.
 *
 * \param this	A pointer to the object representing the file
 *		to be opened.
 *
 * \param fname	The pathname of the file to be opened.
 *
 * \return	A boolean value is returned to indicate the status of
 *		the file open.  A boolean indicates an error
 *		condition.
 */

static _Bool open_wo(const File const this, const char * const fname)

{
	const File_State const S = this->state;


	S->fh = open(fname, O_WRONLY);
	if ( S->fh == -1 ) {
		S->error    = errno;
		S->poisoned = true;
		return false;
	}

	return true;
}


/**
 * External public method.
 *
 * This method implements reading the contents of a file from the
 * current file pointer.   The amount read is designated by the count
 * arguement.  If the count arguement is zero the file is read until an
 * end-of-file indication is given.  Otherwise the number of bytes
 * bytes is read.
 *
 * \param this	A pointer to the object which the file is to be
 *		read into.
 *
 * \param bufr	The buffer object which is to receive the contents of
 *		the file.
 *
 * \param cnt	The number of bytes read interpreted as noted in the
 *		function description.
 *
 * \return	A boolean value is used to indicate whether or not
 *		the read was successful.
 */

static _Bool read_Buffer(const File const this, const Buffer const bufr, \
			 size_t cnt)

{
	const File_State const S = this->state;

	_Bool retn = false;

	int amt_read = 0;

	unsigned int rounds,
		     residual;


	if ( S->poisoned || (S->fh == -1) )
		return false;
	if ( bufr->poisoned(bufr) ) {
		S->poisoned = true;
		return false;
	}


	/* Read the entire contents of the file from the current position. */
	if ( cnt == 0 ) {
		do {
			amt_read = read(S->fh, S->bufr, FILE_BUFSIZE);
			if ( amt_read == -1 ) {
				S->error = errno;
				goto done;
			}
			if ( amt_read != 0 )
				bufr->add(bufr, S->bufr, amt_read);
		}
		while ( amt_read != 0 );
 
		if ( !bufr->poisoned(bufr) )
			retn = true;
		goto done;
	}

	
	/* Read the specified number of bytes. */
	rounds	 = cnt / FILE_BUFSIZE;
	residual = cnt % FILE_BUFSIZE;
	while ( rounds-- ) {
		amt_read = read(S->fh, S->bufr, FILE_BUFSIZE);
		if ( amt_read == -1 ) {
			S->error = errno;
			goto done;
		}
		if ( amt_read > 0 )
			bufr->add(bufr, S->bufr, amt_read);
	}

	if ( residual > 0 ) {
		amt_read = read(S->fh, S->bufr, residual);
		if ( amt_read == -1 ) {
			S->error = errno;
			goto done;
		}
		if ( amt_read != 0 )
			bufr->add(bufr, S->bufr, amt_read);

		if ( !bufr->poisoned(bufr) )
			retn = true;
	}


done:
	memset(S->bufr, '\0', FILE_BUFSIZE);

	if ( !retn )
		S->poisoned = true;
	return retn;
}


/**
 * External public method.
 *
 * This method implements reading the contents of a file into a
 * a Buffer object.
 *
 * \param this	A pointer to the object which the file is to be
 *		read into.
 *
 * \return	A boolean value is used to indicate whether or not
 *		the read was successful.
 */

static _Bool slurp(const File const this, const Buffer const bufr)

{
	const File_State const S = this->state;

	_Bool retn = false;

	struct stat statbuf;


	if ( S->poisoned || (S->fh == -1) )
		return false;
	if ( bufr->poisoned(bufr) )
		goto done;

	if ( this->seek(this, 0) == -1 ) {
		S->error = errno;
		goto done;
	}

	retn = this->read_Buffer(this, bufr, 0);


 done:
	if ( !retn )
		S->poisoned = true;
	return retn;
}


/**
 * External public method.
 *
 * This method implements reading a String object from a file.  The
 * delimiter of a string object is up to and including a newline
 * character.  The newline is replaced with a null character.
 *
 * \param this	A pointer to the object representing the file from which
 *		the String is to be read.
 *
 * \param str	The String object which is to be populated.
 *
 * \return	A boolean value is used to indicate whether or not
 *		the read was successful.
 */

static _Bool read_String(CO(File, this), CO(String, str))

{
	STATE(S);

	char inbufr[2];

	int retn = 0;


	if ( S->poisoned || (S->fh == -1) )
		return false;
	if ( str->poisoned(str) ) {
		S->poisoned = true;
		return false;
	}


	inbufr[1] = '\0';
	do {
		retn = read(S->fh, &inbufr[0], 1);
		switch ( retn ) {
			case -1:
				S->error    = errno;
				S->poisoned = true;
				goto done;
			case 0:
				goto done;
		}
		if ( inbufr[0] != '\n' )
			str->add(str, inbufr);
	}
	while ( inbufr[0] != '\n' );
 

 done:
	if ( str->poisoned(str) ) {
		S->poisoned = true;
		return false;
	}

	if ( retn == 0 )
		return false;
	return true;
}


/**
 * External public method.
 *
 * This method implements writing the contents of a buffer to the
 * opened file.
 *
 * \param this	A pointer to the object being written to.
 *
 * \return	A boolean value is returned to indicate the status
 *		of the write.  A false value indicates an error
 *		was experienced.
 */

static _Bool write_Buffer(const File const this, const Buffer const buffer)

{
	const File_State const S = this->state;

	ssize_t size = buffer->size(buffer);


	if ( S->poisoned || (S->fh == -1) )
		return false;
	if ( buffer->poisoned(buffer) ) {
		S->poisoned = true;
		return false;
	}

	if ( write(S->fh, buffer->get(buffer), size) != size ) {
		S->error    = errno;
		S->poisoned = true;
		return false;
	}

	return true;
}


/**
 * External public method.
 *
 * This method implements seeking to specific locations in a file.
 *
 * \param this	A pointer to the object whose file position is being
 *		changed.
 *
 * \param locn	The position in the file to be seeked to.  Seeking
 *		to the end of the file is requested by specifying a
 *		value of -1.
 *
 * \return	The location in the file after the seek operation has
 *		completed.  If an error was encountered the value of -1
 *		is returned.
 */

static off_t seek(const File const this, off_t locn)

{
	const File_State const S = this->state;

	int whence = SEEK_SET;

	off_t posn;


	if ( S->poisoned )
		return -1;

	if ( locn == -1 ) {
		locn   = 0;
		whence = SEEK_END;
	}

	if ( (posn = lseek(S->fh, locn, whence)) == -1 ) {
		S->error    = errno;
		S->poisoned = true;
		return -1;
	}

	return posn;
}


/**
 * External public method.
 *
 * This method implements the reset of a file object.  The filehandle
 * is closed which prepares the object for re-use.  This function also
 * resets the error status on the object.
 *
 * \param this	A pointer to the object which is to be destroyed.
 */

static void reset(const File const this)

{
	const File_State const S = this->state;


	if ( S->fh != -1 ) {
		close(S->fh);
		S->fh = -1;
	}
	S->poisoned = false;

	return;
}


/**
 * External public method.
 *
 * This method returns the status of the object.
 *
 * \param this	The object whose status is being requested.
 */
static _Bool poisoned(const File const this)

{
	return this->state->poisoned;
}

	
/**
 * External public method.
 *
 * This method implements a destructor for a HurdLib_File object.
 *
 * \param this	A pointer to the object which is to be destroyed.
 */

static void whack(const File const this)

{
	const File_State const S = this->state;


	if ( S->fh != -1 )
		close(S->fh);

	S->root->whack(S->root, this, S);
	return;
}

	
/**
 * External constructor call.
 *
 * This function implements a constructor call for a HurdLib_File object.
 *
 * \return	A pointer to the initialized HurdLib_File.  A null value
 *		indicates an error was encountered in object generation.
 */

extern File HurdLib_File_Init(void)

{
	Origin root;

	File this = NULL;

	struct HurdLib_Origin_Retn retn;


	/* Get the root object. */
	root = HurdLib_Origin_Init();

	/* Allocate the object and internal state. */
	retn.object_size  = sizeof(struct HurdLib_File);
	retn.state_size   = sizeof(struct HurdLib_File_State);
	if ( !root->init(root, HurdLib_LIBID, HurdLib_File_OBJID, &retn) )
		return NULL;
	this	    	  = retn.object;
	this->state 	  = retn.state;
	this->state->root = root;

	/* Initialize aggregate objects. */

	/* Initialize object state. */
	_init_state(this->state);

	/* Method initialization. */
	this->open_ro	= open_ro;
	this->open_rw	= open_rw;
	this->open_wo	= open_wo;

	this->read_Buffer	= read_Buffer;
	this->slurp		= slurp;
	this->read_String	= read_String;
	this->write_Buffer	= write_Buffer;

	this->seek	= seek;

	this->reset	= reset;
	this->poisoned	= poisoned;
	this->whack	= whack;

	return this;
}
