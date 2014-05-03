/**
 * @file types.c
 *
 * @date Apr 23, 2014
 * @author Craig Hesling
 */
#include <stdio.h> // snprintf()
#include <stdlib.h> // size_t and atol()
#include "errors.h"
#include "expression.h" // used in sym_t
#include "types.h"


/*---------------------------------------------*
 *               values                        *
 *---------------------------------------------*/

value_t
value_new_type(enum value_types type) {
	value_t val;
	val.type = type;
	return val;
}

value_t
value_new_lint(sys_int_long lint) {
	value_t val;
	val.type = VAL_LINT;
	val.data.lint = lint;
	return val;
}


/** Create value from a string.
 * @param src_str_len The length of the actual buffer (not the number size).
 * @param src_str The source string.
 * @return The value in the source string.
 * @note Must have first char be digit
 * @note Currently we only do Long Ints
 */
value_t
string_to_value(size_t src_str_len, char const *src_str) {
	value_t val;
	char buf[40]; // for long int: log10(2^128) ~ 39
	size_t index;

	/* Prep. the number string to use in atol() */
	// copy number digits
	for (index = 0; (index < src_str_len) && (index < (40-1)) /*need null byte*/ && IS_DIGIT(src_str[index]); index++) {
		buf[index] = src_str[index];
	}
	// place null-byte
	buf[index] = '\0';

	/* Create the value using atol() */
	val.type = VAL_LINT; ///< @note Here we assume it is a Long Int
	val.data.lint = (sys_int_long) atol(buf); ///< @warning Uses system atol() function
	return val;
}

void
value_to_string (char *dst_str, value_t src_val) {
	switch (src_val.type) {
	case VAL_ERROR:
		sprintf(dst_str, "ERROR");
		break;
	case VAL_UNDEF:
		sprintf(dst_str, "UNDEF");
		//undef_to_string(dst_str, src_val.data.undef);
		break;
	case VAL_INF:
		sprintf(dst_str, "INF");
		break;
	case VAL_LINT:
		//lint_to_string(dst_str, src_val.data.lint);
		snprintf(dst_str, SYS_INT_LONG_T_STR_SIZE + 1, "%ld", src_val.data.lint);
		break;
	default:
		assert(0); ///< @warning Unknown types of @ref value_t will just assert(false) here.
		break;
	}
}

/* vim: set ts=4 sw=4 expandtab: */
