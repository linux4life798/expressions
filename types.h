/**
 * @file types.h
 *
 * @date Apr 23, 2014
 * @author Craig Hesling
 */
#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stddef.h> // size_t
#include <limits.h> // for LONG_MIN
#include "expression.h" // used in sym_t


/*---------------------------------------------*
 *           System Integration                *
 *---------------------------------------------*/
typedef int long sys_int_long;
#define SYS_INT_LONG_T_MIN LONG_MIN
#define SYS_INT_LONG_T_MAX LONG_MAX
/// The length in chars for a long int string. 4 byte long - @f$ {\tt ceiling} \left( {\tt log10}(2^{4 \times 8 - 1}) \right) + 1@f$
#define SYS_INT_LONG_T_STR_SIZE 11


/*---------------------------------------------*
 *       undef - Undefined Values Types        *
 *---------------------------------------------*/
enum undef {
	UNDEF_ERROR, ///< Unknown Error
	UNDEF_UNDEF, ///< Undefined Value
	UNDEF_INF    ///< Infinity
};
typedef enum undef undef_t;

void
undef_to_string (char *dst_str, undef_t src_undef);

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
		//undef_t      undef;
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
 *               symbolic                      *
 *---------------------------------------------*/
/**
 * A named symbol type.
 * @todo Fix symbolic functionality through program.
 * @bug Symbolic functionality is broken through the program.
 */
struct sym {
	char name;          ///< The symbol name
	struct expression *p; ///< The symbol parameter
};
typedef struct sym sym_t;

sym_t
sym_new(void);

sym_t
sym_new_name(char name);

void
sym_free(sym_t sym);

void
sym_to_string(char *dst_str, sym_t src_sym);

//void
//sym_set(char name, value_t num);
//
//value_t
//sym_get(sym_t sym);


#endif /* TYPES_H_INCLUDED */

/* vim: set ts=4 sw=4 expandtab: */
