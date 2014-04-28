/** A compact math expression engine
 * @file expression.c
 *
 * @date Apr 6, 2014
 * @author Craig Hesling
 */
#include <stdio.h>
#include <stdlib.h> // has atol() among others
//#include <limits.h>
#include <string.h>

#include "types.h"
#include "errors.h"
#include "expression.h"


/*---------------------------------------------*
 *     expression_t allocation functions       *
 *---------------------------------------------*/

/** New blank expression.
 * This function creates the most basic empty expression.
 * \return A new empty expression
 */
expression_t
expression_new (void) {
    expression_t exp = (expression_t) malloc(sizeof(struct expression));
    assert(exp); // throw error - expression_new: malloc could not do allocation
    return exp;
}

expression_t
expression_new_value (value_t val) {
    expression_t exp = expression_new ();
    exp->type = EXP_VALUE;
    exp->data.val = val;
    return exp;
}

expression_t
expression_new_tree (char op,
                     expression_t left,
                     expression_t right) {

    expression_t exp = expression_new ();
    exp->type = EXP_SYMBOLIC;
    exp->data.tree.op = op;
    exp->data.tree.left = left;
    exp->data.tree.right = right;
    return exp;
}

/** Free an expression.
 * \param exp The expression to free
 */
void
expression_free (expression_t exp) {
    assert(exp);
    free(exp);
}



/*---------------------------------------------*
 *     expression_t manipulation functions     *
 *---------------------------------------------*/

/* Evaluate Expression recursively */
value_t
expression_evaluate (expression_t exp) {
    if (exp->type == EXP_VALUE) return exp->data.val;
    else if (exp->type == EXP_TREE)
    {
    	value_t ret_val;
    	value_t left_val, right_val;

    	ret_val.type = VAL_LINT;
		left_val  = expression_evaluate(exp->data.tree.left);
		right_val = expression_evaluate(exp->data.tree.right);
        switch (exp->data.tree.op) {
        case '+':
			ret_val.data.lint = left_val.data.lint + right_val.data.lint;
            break;
        case '-':
        	ret_val.data.lint =  left_val.data.lint - right_val.data.lint;
            break;
        case '*':
        	ret_val.data.lint =  left_val.data.lint * right_val.data.lint;
            break;
        case '/':
        	ret_val.data.lint =  left_val.data.lint / right_val.data.lint;
            break;
        default:
            /* throw error - invalid operation in expression */
        	ret_val.type = VAL_ERROR;
            break;
        }
        return ret_val;
    }
    else //if(exp->type == EXP_SYMBOLIC) // Optimize for speed not errors
    {
    	assert((exp->type == EXP_SYMBOLIC));
    	assert(0); //crash
    }

    /* throw error - invalid state in expression */
    //return -1; //error
	assert(0);
	return value_new_type(VAL_ERROR);
}


/** Expression to String */
void
expression_to_string (char *dst_str,
					  expression_t src_exp) {
	switch (src_exp->type) {
	case EXP_VALUE:
		value_to_string(dst_str, src_exp->data.val);
        break;
	case EXP_TREE:
		{
			exp_buf left, right;
			expression_to_string (left, src_exp->data.tree.left);
			expression_to_string (right, src_exp->data.tree.right);
			snprintf(dst_str, EXP_BUF_SIZE, "(%s %c %s)", left, src_exp->data.tree.op, right);
		}
		break;
	case EXP_SYMBOLIC:
		sym_to_string(dst_str, src_exp->data.sym);
        break;
	default:
		assert(0);
		break;
	}
}



/*---------------------------------------------*
 * Helper functions for String to Expression   *
 *---------------------------------------------*/
#define IS_WHITESPACE(c) ( ((c) == ' ') || ((c) == '\t') )
#define IS_DIGIT(c)      ( ( '0' <= (c) ) && ( (c) <= '9') ) ///< \note Is defined in @see types.c.

#define SIZE_T_MAX ( ~( (size_t) (0) ) )

size_t
number_len(size_t str_len, char *str) {
    size_t index;
    for(index = 0; (index < str_len) && IS_DIGIT(str[index]); index++)
        ;
    return index;
}

/** String to Expression.
 *
 * @param str_len Length of given string
 * @param str String to parse
 * @return The expression_t representation of the inputed string
 * @test Test for negative values
 * @callgraph
 * @todo Fix parser in \ref string_to_expression.
 * @bug This parser is broken.
 */
expression_t
string_to_expression (size_t str_len,
					  char const *str) {
    size_t index;
    int level_change = 0; // flag set to 1 if level ever goes above level_base
    int level = 0;        // indicates runtime depth of open parens '('  -- 0 for none
    int level_base = 0;   // indicates current accepted level of open parens (will increase to "rip off" outside parens)

    int oparen_count = 0;
    size_t oparen_index = SIZE_T_MAX;
    size_t oparen_level = SIZE_T_MAX;

    int cparen_count = 0;
    size_t cparen_index = SIZE_T_MAX;
    size_t cparen_level = SIZE_T_MAX;

    int op_count = 0;             // indicates how many operations have been seen on current pass
    size_t op_index = -1;         // index of operation at lowest level found
    size_t op_level = SIZE_T_MAX; // level of the current operation found

    int num_count = 0;   // indicates how many numbers have been seen on current pass
    size_t num_index = -1; // index of number at lowest level found
    size_t num_level = SIZE_T_MAX; // level of the current number found

    exp_buf left_operand;

    for(index = 0; index < str_len; index++) {
        char c = str[index];

        switch(c) {

        /* Parentheses */
        case '(':
            oparen_count++;
            if (level < op_level) {
                oparen_index = index;
                oparen_level = level;
            }

            if(++level > level_base) level_change = 1;
            break;
        case ')':
            cparen_count++;
            if (level < op_level) {
                cparen_index = index;
                cparen_level = level;
            }
            level--;
            break;

        /* Operation chars */
        case '+':
        case '-':
        case '*':
        case '/':
            op_count++;
            if (level < op_level) {
                op_index = index;
                op_level = level;
            }
            break;

        /* White space chars */
        case ' ':
        case '\t':
            /* Ignore - Continue Past */
            break;

        /* End of strings chars */
        case '\0':
        case '\n':
        case '\r':
            /* Throw Error - encountered end of string char! */
        	pferror("string_to_expression", "Parsing Error - Encountered a string terminating character early");
            break;

        default:

            if( IS_DIGIT(c) )
            {
                size_t number_index = index;

                num_count++;
                if (level < num_level) {
                    num_index = index;
                    num_level = level;
                }

                // get over number
                for (; (index < str_len) && IS_DIGIT(str[index]); index++ )
                    ;
                // one too far
                index--;

            }
            else {
                // Throw Error - Unknown character encountered
            	rerror("Syntax Error - Unrecognized character \'%c\'", c);
            }
            break;
        }


    }

	/* Analyze what happened */

	/* TODO (craig#1#04/07/2014): Syntax Sanity Check. Check level==0, level_changed if op_level is set... */

	/* If operation was detected, use it */
	if (op_index != SIZE_T_MAX) {
		// (size of str to left of op) + (# of parens to add) + (1 null byte)
		size_t left_buf_size = (op_index + op_level + sizeof('\0'));
		// (str size) - (op_index + 1) + (# of parens to add) + (1 null byte)
		size_t right_buf_size = str_len - op_index - 1 + op_level + sizeof('\0');

		// We found the primary left-most operation
		expression_t left, right;

		/* Create temporary sub expression string buffers */
		char left_buf[ left_buf_size ];
		char right_buf[ right_buf_size ];

		/* Build left expression buffer */
		memset(left_buf + ( left_buf_size - (op_level + sizeof('\0')) ), ')', op_level);
		strncpy(left_buf, str, op_index);

		/* Build right expression buffer */
		memset(right_buf, '(', op_level);
		strncpy(right_buf + op_level, str + op_index + 1, str_len - op_index - 1);

		/* Recurse on the left and right expressions */
		left  = string_to_expression(strlen(left_buf), left_buf);
		right = string_to_expression(strlen(right_buf), right_buf);

		/* Create symbolic expression from the sub expressions and the operation */
		return expression_new_tree(str[op_index], left, right);
	}
	/* If number detected, use it */
	else if (num_index != SIZE_T_MAX) {
		if (num_count == 1) {
			/* Create value expression using the single number */
			return expression_new_value( string_to_value(str_len - num_index, &str[num_index]) );
		}
		 else {
		 	 // Throw Error - detected more than one number, but no operator was detected
			 rerror("Syntax Error - More than one number detected without an operation");
		 }
	}
	/* They gave us crap -- Error out */
	else {
		/* Throw Error -- Syntax error */
		rerror("Syntax Error - No numbers or operations detected");
	}

	return NULL;
}

/* vim: set ts=4 sw=4 expandtab: */
