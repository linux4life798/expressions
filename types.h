/**
 * @file types.h
 *
 * @date Apr 23, 2014
 * @author Craig Hesling
 *
 * Contains standard types, macros, and functions used throughout the library.
 * Most notably, this is the home of the @ref value_t.
 */
#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stddef.h> /* size_t */
#include <limits.h> // for LONG_MIN
#include <ctype.h>  // character handling functions - isalpha(), isdigit(), isspace()


/*---------------------------------------------*
 *           System Integration                *
 *---------------------------------------------*/
typedef int long sys_int_long;
#define SYS_INT_LONG_T_MIN LONG_MIN
#define SYS_INT_LONG_T_MAX LONG_MAX
/// The length in chars for a long int string. 4 byte long - @f$ {\tt ceiling} \left( {\tt log10}(2^{4 \times 8 - 1}) \right) + 1@f$
#define SYS_INT_LONG_T_STR_SIZE 11

#define SIZE_T_MAX ( ~( (size_t) (0) ) )
#define PINDEX_MAX SIZE_T_MAX ///< The max value of pindex_t
#define PINDEX_BAD PINDEX_MAX ///< Special value of pindex_t that signals a bad value.
typedef size_t pindex_t;      ///< Type for an index into an expression string during parsing.
typedef size_t pcount_t;      ///< Type for an item count during parsing.

#define IS_WHITESPACE(c) ( ((c) == ' ') || ((c) == '\t') )
#define IS_DIGIT(c)      ( ( '0' <= (c) ) && ( (c) <= '9') ) ///< \note Is defined in @see types.c.
#define IS_ALPHA(c)      isalpha(c)
#define IS_ALNUM(c)      isalnum(c)


/*---------------------------------------------*
 *               values                        *
 *---------------------------------------------*/
/** Represents a numeric value or an error.
 * This is the structure that abstractly represents numeric values
 *  including undefined values and errors they may be associated with.
 */
struct value {
	enum value_types {
		VAL_ERROR,
		VAL_UNDEF,
		VAL_INF,
		VAL_LINT
	} type;
	union {
		sys_int_long lint;
	} data;
};
typedef struct value value_t;

value_t
value_new_type(enum value_types type);

value_t
value_new_lint(sys_int_long lint);

value_t
string_to_value(size_t src_str_len, char const *src_str);

void
value_to_string (char *dst_str, value_t src_val);


/*---------------------------------------------*
 *                 misc                        *
 *---------------------------------------------*/

pindex_t
find_matching (size_t str_len,
               const char *str,
               pindex_t start);

#endif /* TYPES_H_INCLUDED */

/* vim: set ts=4 sw=4 expandtab: */
