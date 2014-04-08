/*
 * errors.c
 *
 *  Created on: Apr 7, 2014
 *      Author: Craig Hesling
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include "errors.h"

/*
 * Runtime Error
 */
void
rerror(char const *fmt,
	   ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	fprintf (stderr, "\n");
	va_end (args);
	exit(1);
}

/*
 * Program Fault Error
 */
void
pferror(const char *function_name,
		char const *fmt,
		...) {
	va_list args;
	va_start(args, fmt);
	fprintf  (stderr, "%s: Prgm Fault Error! - ", function_name);
	vfprintf (stderr, fmt, args);
	fprintf  (stderr, "\n");
	va_end (args);
	exit(2);
}
