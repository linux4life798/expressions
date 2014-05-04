/** A compact math expression engine
 * @file expression.h
 *
 * @date Apr 6, 2014
 * @author Craig Hesling
 */
#ifndef EXPRESSION_H_INCLUDED
#define EXPRESSION_H_INCLUDED

#include <stddef.h> /* size_t */

#include "types.h"
#include "symbolic.h"

/*---------------------------------------------*
 *     Misc. String Buffer Definitions         *
 *---------------------------------------------*/
#define EXP_BUF_SIZE 256
typedef char exp_buf[EXP_BUF_SIZE];


/*---------------------------------------------*
 *     expression tree structure               *
 *---------------------------------------------*/
/* The tree operation element for the expression structure */
/** Expression data container for the expanded expression.
 * Expanded tree expressions are composed of an operation that joins a left sub-expression and a right rub-expression.
 */
struct expression_data_tree {
	/** The joining operation of the two two sub expressions.
	 * \note Previously implemented using enumerations, but proved to be more of a burden.
	 */
    char op; // '+', '-', '*', or '/'
    expression_t left;  ///< Left sub-expression
    expression_t right; ///< Right sub-expression
};


/*---------------------------------------------*
 *     expression components                   *
 *---------------------------------------------*/

/** Expression types.
 * This is an enumeration of \ref expression_t's types.
 * Specific types correspond to specific data containers in \ref expression_data.
 */
enum expression_type {
//    EXP_UNDEF, ///< \note Might remove
    EXP_VALUE,    ///< Contains a simple numeric values and undefines
    EXP_TREE,     ///< An expression over an operation and sub-expression
    EXP_SYMBOLIC  ///< Symbolic named references
};

/** Expression data container.
 * This union allows for the use of all the different expression data containers that corresponding to each \ref expression_type.
 */
union expression_data {
	value_t                     val;   ///< Value for EXP_VALUE type.
	struct expression_data_tree tree;  ///< Data for EXP_TREE type.
	sym_t                       sym;   ///< Data for EXP_SYMBOLIC type.
};

/*---------------------------------------------*
 *     expression structure                    *
 *---------------------------------------------*/
/** The stored representation of mathematical expressions.
 * An expression is either an expanded value of an operation on two other sub-expressions, a direct value (evaluated expression), or a symbolic expression.
 * An expression consists of an \ref expression_type and it's \ref expression_data.
 */
/// \note New types must have an entry in the \ref type enumeration and an associated entry in the \ref data union.
struct expression {
	enum expression_type  type; ///< The expression's selected type
    union expression_data data; ///< The expression's data corresponding to it's \ref type.
};

/*---------------------------------------------*
 *     expression_t allocation functions       *
 *---------------------------------------------*/

expression_t
expression_new (void);

expression_t
expression_new_value (value_t val);

expression_t
expression_new_tree (char op,
                     expression_t left,
                     expression_t right);

expression_t
expression_new_sym (sym_t sym);

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

#endif // EXPRESSION_H_INCLUDED

/* vim: set ts=4 sw=4 expandtab: */
