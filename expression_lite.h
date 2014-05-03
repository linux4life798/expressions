/**
 * @file expression_lite.h
 *
 * @date May 3, 2014
 * @author Craig Hesling
 *
 * Does not include any other project header file. This ensures that there are no circular dependencies.
 */
#ifndef _EXPRESSION_LITE_H_
#define _EXPRESSION_LITE_H_

#include <stddef.h> // size_t

// Indicate that expression_t is a pointer type
typedef struct expression *expression_t;

/*---------------------------------------------*
 *     expression_t allocation functions       *
 *---------------------------------------------*/

expression_t
expression_new (void);

void
expression_free (expression_t exp);


/*---------------------------------------------*
 *     expression_t manipulation functions     *
 *---------------------------------------------*/

value_t
expression_evaluate (expression_t exp);

void
expression_to_string (char *dst_str,
		              expression_t src_exp);

expression_t
string_to_expression (size_t str_len,
					  char const *str);

#endif /* _EXPRESSION_LITE_H_ */
