/**
 * @file types.c
 *
 * @date Apr 23, 2014
 * @author Craig Hesling
 *
 * Contains standard types, macros, and functions used throughout the library.
 * Most notably, this is the home of the @ref value_t.
 */
#include <stdio.h> /* snprintf() */
#include <stddef.h> /* size_t */
#include <stdlib.h> /* atol() */
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
//	char buf[40]; // for long int: log10(2^128) ~ 39
//	size_t index;

	if((src_str_len == 0) || (src_str_len > 40)) pferror("string_to_value","string length given is not within usage range");

	/* Prep. the number string to use in atol() */
	// copy number digits
//	for (index = 0; (index < src_str_len) && (index < (40-1)) /*need null byte*/ && IS_DIGIT(src_str[index]); index++) {
//		buf[index] = src_str[index];
//	}
	// place null-byte
//	buf[index] = '\0';

	/* Create the value using atol() */
	val.type = VAL_LINT; ///< @note Here we assume it is a Long Int
	val.data.lint = (sys_int_long) atol(src_str); ///< @warning Uses system atol() function at index 0 on the bare string given
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

/*---------------------------------------------*
 *                 misc                        *
 *---------------------------------------------*/

/**
 * Find the index of the matching closing paren in a string.
 * @param str_len Overall string length.
 * @param str String to search in.
 * @param start Index of the open paren to match.
 * @return The index of the matching paren or PINDEX_BAD if no match was found.
 * @note Assumes start is the index of the open paren to match and then moves forward.
 */
pindex_t
find_matching (size_t str_len,
               const char *str,
               pindex_t start) {
	pcount_t level;

	// Increment the index past the initial open paren
	if( (++start) >= str_len ) return PINDEX_BAD;
	// Stepped up a level
	level = 1;

	// parse only for parens and stop when level returns to 0
    for (;(start < str_len) && (level > 0);start++) {
        assert(str[start] != '\0');

        // modify level for parens
        if (str[start] == '(') level++;
        else if (str[start] == ')') level--;

        // when level is 0, we found the matching paren
        if (level == 0) return start;
    }

    // If we didn't find level 0 above, then we ran out of characters to scan
    return PINDEX_BAD;
}

/* vim: set ts=4 sw=4 expandtab: */
