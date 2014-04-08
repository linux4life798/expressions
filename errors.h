/*
 * errors.h
 *
 *  Created on: Apr 7, 2014
 *      Author: Craig Hesling
 */

#ifndef ERRORS_H_
#define ERRORS_H_

#include <assert.h> /* assert() */

#ifdef DEBUG
	// Make sure assert is enabled
	#ifdef NDEBUG
		#undef NDEBUG
	#endif

#else
	// disable assert
	#ifdef NDEBUG
		#define NDEBUG
	#endif
#endif


/*
 * Runtime Error
 */
void
rerror(char const *msg);

/*
 * Program Fault Error
 */
void
pferror(const char *function_name,
		char const *msg);

#endif /* ERRORS_H_ */
