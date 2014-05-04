/**
 * @file symbolic.h
 *
 * @date May 3, 2014
 * @author Craig Hesling
 *
 * A light structure that is mostly public access.
 *
 * The point of the sym structure is to create a string based reference
 * to an object without being directly connected to the object.
 * This allows for
 *
 */
#ifndef _SYMBOLIC_H_
#define _SYMBOLIC_H_

#include <stddef.h> /* size_t */
#include "expression_lite.h" // just need pointer expression_t

#define SYMBOLIC_NAME_SIZE 10
#define SYMBOLIC_P_STR_SIZE 16

/**
 * A named symbol type.
 * A symbol can refer to a variable or function.
 * The field p is NULL when no parameter exists.
 */
struct sym {
	char name[SYMBOLIC_NAME_SIZE]; ///< The symbol name
	struct expression *p;          ///< The symbol parameter
};
typedef struct sym sym_t;

sym_t
sym_new_name(char *name);

sym_t
string_to_sym (size_t src_str_len, char const *src_str);

void
sym_to_string(char *dst_str, sym_t src_sym);

//void
//sym_set(char name, value_t num);
//
//value_t
//sym_get(sym_t sym);

#endif /* _SYMBOLIC_H_ */

/* vim: set ts=4 sw=4 expandtab: */
