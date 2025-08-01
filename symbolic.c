/**
 * @file symbolic.c
 *
 * @date May 3, 2014
 * @author Craig Hesling
 */
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "errors.h"
#include "symbolic.h"

/*---------------------------------------------*
 *               symbolic                      *
 *---------------------------------------------*/
//expression_t sym_names[26];
//
//#define sym_to_names_index(x)


/** New symbolic object whose name is set to name.
 */
sym_t
sym_new_name(char *name) {
	sym_t sym;
	assert(name);
	assert(strlen(name) < SYMBOLIC_NAME_SIZE);

	///@todo Should probably check that the name contains valid chars (printable)
	strncpy(sym.name, name, SYMBOLIC_NAME_SIZE);
	return sym;
}

/** Create symbol from a string.
 * @param src_str_len The length of the actual buffer (not the number size).
 * @param src_str The source string.
 * @return The symbol in the source string.
 * @note Must have first char be digit
 * @note Currently we only do Long Ints
 */
sym_t
string_to_sym (size_t src_str_len, char const *src_str) {
	sym_t    sym;
	pindex_t index = 0;
    pindex_t index_tmp;           ///< Used to explore the presence of a symbol parameter

    // default values
    sym.name[0] = '\0';
    sym.p = NULL;

    // navigate over possible whitespace to next real character -- using a main index
    for (; (index < src_str_len) && IS_WHITESPACE(src_str[index]); index++)
    	;

    // find end of symbol name -- using index_tmp
    for (index_tmp = index; (index_tmp < src_str_len) && IS_ALNUM(src_str[index_tmp]); index_tmp++)
    	;
    // index_tmp should now be placed one past the symbol name

    // check if the name will fit in the sym_t name
    if ((index_tmp - index) >= SYMBOLIC_NAME_SIZE) {
    	rerror("Symbol Error - Symbol name is too large");
    }
    // store the name
    memcpy(sym.name, &src_str[index], (size_t)(index_tmp - index));
    sym.name[(index_tmp - index)] = '\0';

    // update index from the explorer temp index
    index = index_tmp;

    // navigate over possible whitespace to next real character -- using a index
    for (; (index < src_str_len) && IS_WHITESPACE(src_str[index]); index++)
    	;
    // index should now be placed one past the symbol name or white space after symbol name

    // check for presence of '(' signifying the start of a symbol parameter
    if ( (index < src_str_len) && (src_str[index] == '(') ) {
    	size_t end;

//        pferror("string_to_expression","Symbol functions not currently supported");

    	// find end of parameter string
    	end = find_matching(src_str_len, src_str, index);

    	// check if the parameter string is not valid
    	if (end == PINDEX_BAD) {
    		rerror("Syntax Error - Unmatched parenthesis around symbol parameter");
    	}

    	// parameter must be valid

    	// store the parameter expression
    	sym.p = string_to_expression((end+1) - index, &src_str[(end+1) - index]);
    }

    return sym;
}

/** Generate a string from a sym_t.
 * @param dst_str String to write to.
 * @param src_sym Symbolic type to read from.
 */
void
sym_to_string (char *dst_str, sym_t src_sym) {
	assert(dst_str);

	// if no parameter
	if (src_sym.p == NULL) {
		// has no parameter
		sprintf(dst_str, "%s", src_sym.name);
	} else {
		// has parameter
		char buf[SYMBOLIC_P_STR_SIZE];
		expression_to_string(buf,src_sym.p);
		sprintf(dst_str, "%s(%s)", src_sym.name, buf);
	}
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
