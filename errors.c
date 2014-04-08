/*
 * errors.c
 *
 *  Created on: Apr 7, 2014
 *      Author: Craig Hesling
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "errors.h"

/*
 * Runtime Error
 */
void
rerror(char const *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

/*
 * Program Fault Error
 */
void
pferror(const char *function_name,
		char const *msg) {
	fprintf(stderr, "%s: Prgm Fault Error! - %s\n", function_name, msg);
	exit(2);
}
