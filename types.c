/**
 * @file types.c
 *
 * @date Apr 23, 2014
 * @author Craig Hesling
 */
#include <stdio.h> // snprintf()
#include <stdlib.h> // size_t and atol()
#include "errors.h"
#include "types.h"

/*---------------------------------------------*
 *       undef - Undefined Values Types        *
 *---------------------------------------------*/
void
undef_to_string (char *dst_str, undef_t src_undef) {
	switch (src_undef) {
	case UNDEF_UNDEF:
		/* Say "Undef" */
		dst_str[0] = 'U';
		dst_str[1] = 'n';
		dst_str[2] = 'd';
		dst_str[3] = 'e';
		dst_str[4] = 'f';
		dst_str[5] = '\0';
		break;
	case UNDEF_INF:
		/* Say "INF" */
		dst_str[0] = 'I';
		dst_str[1] = 'N';
		dst_str[2] = 'F';
		dst_str[3] = '\0';
		break;
	case UNDEF_ERROR:
	default:
		/* Say "ERR" */
		dst_str[0] = 'E';
		dst_str[1] = 'R';
		dst_str[2] = 'R';
		dst_str[3] = '\0';
		break;
	}
}


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


#define IS_DIGIT(c)      ( ( '0' <= (c) ) && ( (c) <= '9') ) /// @note Is defined in @see expression.c also.

/**
 * @note Must have first char be digit
 * @note Currently we only do Long Ints
 */
value_t
string_to_value(size_t src_str_len, char const *src_str) {
	value_t val;
	char buf[40]; // for long int: log10(2^128) ~ 39
	size_t index;

	// copy number digits
	for (index = 0; (index < src_str_len) && (index < (40-1)) /*need null byte*/ && IS_DIGIT(src_str[index]); index++) {
		buf[index] = src_str[index];
	}
	// place null-byte
	buf[index] = '\0';

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

/*---------------------------------------------*
 *               symbolic                      *
 *---------------------------------------------*/
//expression_t sym_names[26];
//
//#define sym_to_names_index(x)

/** New symbolic object with default name.
 * @note This is mainly for future compatibility
 */
sym_t
sym_new(void){
	sym_t sym;
	sym.name = 'x'; ///< Default symbol is x
	return sym;
}

/** New symbolic object whose name is set to name.
 */
sym_t
sym_new_name(char name) {
	sym_t sym;
	/* Check that the name is within range */
	assert(('A' <= name) && (name <= 'Z'));
	sym.name = name;
	return sym;
}

/** Free a sym_t
 * @param sym The sym_t to free
 * @note Do nothing
 * @remark This is primarily for future compatibility
 */
void
sym_free(sym_t sym) {
	sym = sym; // do nothing
	return ;
}

/** Generate a string from a sym_t.
 * @param dst_str String to write to.
 * @param src_sym Symbolic type to read from.
 */
void
sym_to_string (char *dst_str, sym_t src_sym) {
	dst_str[0] = src_sym.name;
	dst_str[1] = '\0';
}

//void
//sym_reset(void) {
//	int i;
//	for (i = 0; i < 26; i++) {
//		sym_names[i].type = VAL_SYM;
//		sym_names[i].data.sym.name = (i + 'A');
//	}
//}
//
//void
//sym_set(char name, value_t val) {
//    /* Check that the name is within range */
//    assert(('A' <= name) && (name <= 'Z'));
//    sym_names[name - 'A'] = val;
//}
//
//value_t
//sym_get(sym_t sym) {
//	return sym_names[sym.name - 'A'];
//}
//
///** Evaluate the symbol forward.
// * Evaluates the symbol as far as possible towards making a non VAL_SYM @ref value::type .
// * This means it will can follow symbols to other symbols until we get a non-VAL_SYM @ref value::type or the symbol is itself.
// * @note This is done recursively.
// * @param sym Symbol to evaluate
// * @return The evaluated symbol
// */
//value_t
//sym_evaluate(sym_t sym) {
//	value_t *val = sym_get(sym);
//
//	// if we are pointing to a good number or NAN
//	if (val->type != VAL_SYM) {
//		return *val;
//	}
//
//	// if we are pointing to our-self
//	if (sym.name == val->data.sym.name) {
//        return *val;
//	}
//
//	// must be pointing to another symbol
//	return sym_evaluate(val->data.sym); // better be inbounds
//}

/* vim: set ts=4 sw=4 expandtab: */
