/** \file
 * This file contains the implementation of a Buffer object.  This object
 * implements a dynamically sized character buffer.
 */

/**************************************************************************
 * (C)Copyright 2006, The Open Hurderos Foundation. All rights reserved.
 *
 * Please refer to the file named COPYING in the top of the source tree
 * for licensing information.
 **************************************************************************/

/* Include files. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "HurdLib.h"
#include "Origin.h"
#include "Fibsequence.h"
#include "Buffer.h"


/* Verify library/object header file inclusions. */
#if !defined(HurdLib_LIBID)
#error Library identifier not defined.
#endif

#if !defined(HurdLib_Buffer_OBJID)
#error Object identifier not defined.
#endif


/** Buffer private state information. */
struct HurdLib_Buffer_State
{
	/* The root object. */
	Origin root;

	/* Library identifier. */
	uint32_t libid;

	/* Object identifier. */
	uint32_t objid;

	/* Object status. */
	_Bool poisoned;

	/* The current allocation size. */
	size_t used;

 	/* A pointer to the memory buffer implemented by the object. */
	unsigned char *bf;

	/* The Fibonacci sequence used to implement dynamic object size. */
	Fibsequence seqn;
};


/**
 * Internal private method.
 *
 * This method is responsible for initializing the HurdLib_Buffer_State
 * structure which holds state information for each instantiated object.
 *
 * \param state A pointer to the object containing the state information which
 *        is to be initialized.
 */

static void _init_state(Buffer_State const S) {

	S->libid = HurdLib_LIBID;
	S->objid = HurdLib_Buffer_OBJID;

	S->poisoned = false;
	S->used = 0;
	S->bf   = NULL;

	return;
}


/**
 * Internal private method.
 *
 * This method allocates or re-allocates memory allocation for this
 * buffer.
 * 
 * \param S	A pointer to the state of the buffer whose memory allocation
 *		is being modified.
 *
 * \return	A boolean value is used to indicate whether or not the
 *		re-allocation was successful.  A true value indicates
 *		success.
 */

static _Bool _do_alloc(const Buffer_State const S)

{

	S->bf = realloc(S->bf, S->seqn->get(S->seqn));
	if (S->bf == NULL ) {
		S->poisoned = true;
		return false;
	}

	return true;
}


/**
 * External public method.
 *
 * This method implements the addition of bytes to the buffer.  If
 * the buffer supports dynamic sizing the size of the buffer in the
 * object's private state structure is expanded to accomodate the
 * required addition to the buffer.
 *
 * \param this	A pointer to the buffer object which bytes are being
 *		added to.
 *
 * \param src	A pointer to the area of memory from which the bytes are
 *		to be copied.
 *
 * \param cnt	The number of characters to move from the source area
 *		to the buffer.
 *
 * \return	A boolean value is used to indicate the success or
 *		failure of the addition of elements to the buffer.  A
 *		true value indicates success.
 */

static _Bool add(const Buffer const this, unsigned char const * const src, \
		size_t const cnt)

{
	auto Buffer_State S = this->state;

	auto size_t free = S->seqn->get(S->seqn) - S->used;


	if ( S->poisoned )
		return false;

	if ( cnt > free )
		do {
			S->seqn->next(S->seqn);
			free = S->seqn->get(S->seqn) - S->used;
		} while ( cnt > free );

	if ( !_do_alloc(S) )
		return false;
	memcpy(S->bf + S->used, src, cnt);
	S->used += cnt;

	return true;
}


/**
 * External public method.
 *
 * This method implements the addition of bytes to the buffer by a
 * second Buffer object.  It is simply a fronting function for the
 * add method of the object.
 *
 * \param this	A pointer to the buffer object which bytes are being
 *		added to.
 *
 * \param bf	The Buffer object which is to be added to the current
 *		Buffer.
 *
 * \return	A boolean value is used to indicate the success or
 *		failure of the addition of elements to the buffer.  A
 *		true value indicates success.
 */

static _Bool add_Buffer(const Buffer const this, const Buffer const bf)

{
	return add(this, bf->get(bf), bf->size(bf));
}


/**
 * External public method.
 *
 * This method implements the addition of bytes to a buffer based on a
 * hexadecimal string.  This method ultimately calls the add method so
 * dynamic buffer sizing is supported.
 *
 * The detection of a character which is not in the character set
 * [0-9a-f] will generate an error condition on the object.  The method
 * also tests to make sure an even number of characters are in the
 * input text string.  An odd number of characters also sets an
 * error condition on the object.
 *
 * \param this	A pointer to the buffer object which the text string
 *		encoded bytes will be added to.
 *
 * \param text	A pointer a character string containing the hex characters
 *		to be encoded into the buffer.
 *
 * \return	A boolean value is used to indicate the success or
 *		failure of the addition of elements to the buffer.  A
 *		true value indicates success.
 */

static _Bool add_hexstring(const Buffer const this, char const * const hexbufr)

{
	auto _Bool retn = false,
		   first_nybble = true;

	auto unsigned char nybble,
			   byte[1];

	auto size_t lp,
		    hexbufr_length;


	/* Don't move forward if the object has had an error. */
	if ( this->state->poisoned )
		goto done;

	/* Sanity check the string. */
	if ( hexbufr == NULL ) {
		this->state->poisoned = true;
		goto done;
	}

	hexbufr_length = strlen(hexbufr);
	if ( (hexbufr_length == 0) || ((hexbufr_length % 2) != 0) ) {
		this->state->poisoned = true;
		goto done;
	}


	/*
	 * The somewhat brute force conversion occurs below.  The output
	 * buffer is big-endian.  An invalid character causes a failure
	 * of the conversion.
	 */
	for (lp= 0; lp < hexbufr_length; ++lp) {
		switch ( toupper(hexbufr[lp]) )
		{
			case '0':
				nybble = 0;
				break;
			case '1':
				nybble = 1;
				break;
			case '2':
				nybble = 2;
				break;
		    	case '3':
				nybble = 3;
				break;
		    	case '4':
				nybble = 4;
				break;
		    	case '5':
				nybble = 5;
				break;
	    		case '6':
				nybble = 6;
				break;
		    	case '7':
				nybble = 7;
				break;
		    	case '8':
				nybble = 8;
				break;
	    		case '9':
				nybble = 9;
				break;
	    		case 'A':
				nybble = 10;
				break;
		    	case 'B':
				nybble = 11;
				break;
	    		case 'C':
				nybble = 12;
				break;
		    	case 'D':
				nybble = 13;
				break;
		    	case 'E':
				nybble = 14;
				break;
		    	case 'F':
				nybble = 15;
				break;
	    		default:
				this->state->poisoned = true;
				goto done;
		}

		if ( first_nybble ) {
			byte[0] = nybble << 4;
			first_nybble = false;
		}		
		else {
			byte[0] = (byte[0] | nybble);
			add(this, byte, sizeof(byte));
			first_nybble = true;
		}
	}
	retn = true;


done:
	return retn;
}


/**
 * External public method.
 *
 * This method implements reducing the effective size of the internal
 * memory buffer. It does this by decrementing the used size count of
 * the buffer.  NOTE that it does not physically reallocate the size
 * of the memory allocation for the internal buffer.
 *
 * \param this	A pointer to the buffer object whose size is being
 *		reduced.
 *
 * \param cnt	The amount by which the object is to be contracted.
 */

static void shrink(const Buffer const this, size_t const cnt)

{
	auto Buffer_State S = this->state;


	if ( S->poisoned )
		return;

	/* Safety check. */
	/* HURTODO: Need to add warning if conditional succeeds. */
	if ( cnt > S->used )
		return;

	S->used -= cnt;
	return;
}


/**
 * External public method.
 *
 * This method returns the size of the buffer in bytes.

 * \param this	A pointer to the buffer object whose size is being
 *		returned.
 *
 * \return	The size of the buffer.  Note that this may be different
 *		from the actual physical allocated size of the buffer.
 */

static size_t size(const Buffer const this)

{
	if ( this->state->poisoned )
		return 0;
	return this->state->used;
}


/**
 * External public method.
 *
 * This method resets the buffer so that any additional characters will
 * be added at the beginning of the buffer.  It should be noted that this
 * method does not modify the amount of memory allocated to the buffer.
 *
 * \param this	A pointer to the buffer object which is to be reset.
 *
 */

static void reset(const Buffer const this)

{
	auto Buffer_State S = this->state;


	if ( S->poisoned )
		return;

	memset(S->bf, '\0', S->used);
	S->used = 0;

	return;
}


/**
 * External public method.
 *
 * This methhod implements an accessor for returning the address of
 * the memory buffer.
 *
 * \param this	A pointer to the buffer whose address is to be returned.
 *
 * \return	A pointer to the address of the internal memory buffer.
 */

static unsigned char *get(const Buffer const this)

{
	if ( this->state->poisoned )
		return NULL;
	return this->state->bf;
}


/**
 * External public method.
 *
 * This method implements printing of the conten ts of the buffer.  For
 * lack of a better definition of 'printing' for a binary buffer the
 * hexadecimal representation of the buffer is printed.
 *
 * \param	A pointer to the buffer which is to be printed.
 */

static void print(const Buffer const this)

{
	auto size_t lp = 0;

	auto Buffer_State S = this->state;


	if ( this->state->poisoned ) {
		fputs("* POISONED *\n", stderr);
		return;
	}

	while ( lp < S->used ) {
		fprintf(stdout, "%02x", *(S->bf+lp));
		++lp;
	}
	fputc('\n', stdout);

	return;
}


/**
 * External public method.
 *
 * This method implements dumping out the current state of the buffer.
 * All of the members of the internal state object are output in their
 * appropriate form.
 * 
 * \param this		The buffer object whose state will be dumped.
 *
 * \param offset	Te offset to be added to the output.
 */

static void dump(const Buffer const this, int offset)

{
	auto Buffer_State S = this->state;

	auto Origin root = this->state->root;


	if ( offset == 0 )
		offset = 1;

	root->iprint(root, offset, __FILE__ " dump: %p\n", this);
	root->iprint(root, offset, "\tbufr: %p\n", S->bf);
	root->iprint(root, offset, "\tused: %u\n", S->used);
	root->iprint(root, offset, "\tallocated: %u\n", S->seqn->get(S->seqn));
	root->iprint(root, offset, "\tstatus: %s\n", S->poisoned ? \
		     "POISONED" : "OK");

	root->iprint(root, offset, "\tContents: ");
	if ( S->poisoned ) {
		S->poisoned = false;
		this->print(this);
	}
	else	
		this->print(this);
	root->iprint(root, offset, "\n");
  
	offset += 1;
	S->seqn->dump(S->seqn, offset);

	return;
}


/**
 * External public method.
 *
 * This function returns the status of the object.
 *
 * \param this	A point to the object whose status is being requested.
 */
static _Bool poisoned(const Buffer const this)

{
	return this->state->poisoned;
}


/**
 * External public method.
 *
 * This function implements a destructor for a Buffer object.
 *
 * \param this	A pointer to the object which is to be destroyed.
 */

static void whack(const Buffer const this)

{
	auto Buffer_State S = this->state;

	if ( S->bf != NULL )
		memset(S->bf, '\0', S->seqn->get(S->seqn));
	free(S->bf);

	S->seqn->whack(S->seqn);
	S->root->whack(S->root, this, S);

	return;
}

	
/**
 * External constructor call.
 *
 * This function implements a constructor call for a Buffer object.
 *
 * \return	A pointer to the initialized Buffer.
 */

extern Buffer HurdLib_Buffer_Init(void)

{
	auto Origin root;

	auto Buffer this = NULL;

	auto struct HurdLib_Origin_Retn retn;


	/* Get the root object. */
	root = HurdLib_Origin_Init();

	/* Allocate the object and internal state. */
	retn.object_size  = sizeof(struct HurdLib_Buffer);
	retn.state_size	  = sizeof(struct HurdLib_Buffer_State);
	if ( !root->init(root, HurdLib_LIBID, HurdLib_Buffer_OBJID, &retn) )
		return NULL;
	this	    	  = retn.object;
	this->state 	  = retn.state;
	this->state->root = root;

	/* Initialize aggregate objects. */
	if ( (this->state->seqn = HurdLib_Fibsequence_Init()) == NULL ) {
		root->whack(root, this, this->state);
		return NULL;
	}

	/* Initialize object state. */
	_init_state(this->state);

	/* Method initialization. */
	this->add     	    = add;
	this->add_Buffer    = add_Buffer;
	this->add_hexstring = add_hexstring;

	this->get     	    = get;
	this->shrink  	    = shrink;
	this->size    	    = size;
	this->reset	    = reset;
	this->print   	    = print;
	this->dump    	    = dump;
	this->poisoned	    = poisoned;
	this->whack   	    = whack;

	return this;
}
