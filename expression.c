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
#include <ctype.h> // character handling functions - isalpha(), isdigit(), isspace()

#include "types.h"
#include "errors.h"
#include "symbolic.h"
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
    exp->type = EXP_TREE;
    exp->data.tree.op = op;
    exp->data.tree.left = left;
    exp->data.tree.right = right;
    return exp;
}

expression_t
expression_new_sym (sym_t sym) {
    expression_t exp = expression_new ();
    exp->type = EXP_SYMBOLIC;
    exp->data.sym = sym;
    return exp;
}

/** Free an expression.
 * \param exp The expression to free
 */
void
expression_free (expression_t exp) {
    assert(exp);

    if (exp->type == EXP_SYMBOLIC) {
    	//sym_free(exp->data.sym);
    }
    else if (exp->type == EXP_TREE) {
    	expression_free(exp->data.tree.left);
    	expression_free(exp->data.tree.right);
    }

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
    	pferror("expression_evaluate","SYMBOLIC types evaluator is unimplemented");
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

size_t
number_len(size_t str_len, char *str) {
    size_t index;
    for(index = 0; (index < str_len) && IS_DIGIT(str[index]); index++)
        ;
    return index;
}

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


/** Same as strncpy, but throws an error if a null character is reached in src.
 *
 * @param dest Destination string buffer.
 * @param src Source string buffer.
 * @param count The number of bytes to copy.
 */
void
str_cpy(char *dest, const char *src, size_t count) {
	size_t index;
	assert(src);
	assert(dest);
	for(index = 0; index < count; index++) {
		if (src[index] == '\0') {
			pferror("str_cpy", "encountered NULL byte while copying \"%s\"", src);
		}
		dest[index] = src[index];
	}
}

/** Convert String to an Expression.
 * Parses a string into an expression.
 *
 * @section parsing_algorithm Algorithm
 * @subsection parsing_overview Parsing Overview
 * 	- Step 1: Recording details about the string while scanning from left to right.
 * 	- Step 2: There are two real cases. The string is a single number (with no operations)
 * 		or it is two sub-expressions joined by an operation.\n
 * 		Cases:
 * 		- If it is a simple number, we know how to parse it and create a value expression.
 * 		- If it is not just a number, we want split the string about the operation at the lowest level.
 * 			(in practice, we end up finding the one that is farthest left of the lowest level)
 * 			We then construct a tree expression with the chosen operation and the sub-expression results
 * 			from running the function recursively on each of the sub-strings.
 *
 * @param str_len Length of given string
 * @param str String to parse
 * @return The expression_t representation of the inputed string
 * @test Test for negative values
 * @callgraph
 * @note level was type int - changed to unsigned to temporarily quiet compiler
 * @bug Cannot parse negative numbers
 */
expression_t
string_to_expression (size_t str_len,
					  char const *str) {

    size_t index;         // main index into str
//    int level_change = 0; // flag set to 1 if level ever goes above level_base
    size_t level = 0;     // indicates runtime depth of open parens '('  -- 0 for none
//    int level_base = 0;   // indicates current accepted level of open parens (will increase to "rip off" outside parens)

    /* Statistics about Open Parens seen during parsing */
    int oparen_count = 0; // total number seen
//    size_t oparen_index = SIZE_T_MAX;
//    size_t oparen_level = SIZE_T_MAX;

    /* Statistics about Closed Parens seen during parsing */
    int cparen_count = 0; // total number seen
//    size_t cparen_index = SIZE_T_MAX;
//    size_t cparen_level = SIZE_T_MAX;

    /* Statistics about Operations seen during parsing */
    int op_count = 0;             // indicates how many operations have been seen on current pass
    size_t op_index = -1;         // index of operation at lowest level found
    size_t op_level = SIZE_T_MAX; // level of the current operation found

    /* Statistics about Whole Numbers seen during parsing */
    int num_count = 0;   // indicates how many numbers have been seen on current pass
    size_t num_index = -1; // index of number at lowest level found
    size_t num_level = SIZE_T_MAX; // level of the current number found
    size_t num_len;                // length in chars of the number found

    /* Statistics about Symbols found */
    // Note: The lowest level matters when parsing a symbol fn. of a symbol. Ex. "sin( pi )"
    int    sym_count = 0;          // indicates how many numbers have been seen on current pass
    size_t sym_index = -1;         // index of symbol at lowest level found
    size_t sym_level = SIZE_T_MAX; // level of the current symbol found
    size_t sym_len;

//    exp_buf left_operand;

    /*
     * Stage 1
     * Scan through elements of string from left to right once
     */
    for(index = 0; index < str_len; index++) {
    	/* Grab current char */
        char c = str[index];

        /* Identify the char grabbed */
        switch(c) {

        /* Parentheses */
        case '(':
            oparen_count++;
//            if (level < op_level) {
//                oparen_index = index;
//                oparen_level = level;
//            }

//            if(++level > level_base) level_change = 1;
            level++;
            break;
        case ')':
            cparen_count++;
//            if (level < op_level) {
//                cparen_index = index;
//                cparen_level = level;
//            }
            level--;
            break;

        /* Operation chars */
        case '+':
        case '-':
        case '*':
        case '/':
        	// add one to op counter
            op_count++;
            // if lower level op was found
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

        	// if we encounter the start of a number
            if( IS_DIGIT(c) )
            {
                size_t start_index = index;

                num_count++;

                // get over number
                for (; (index < str_len) && IS_DIGIT(str[index]); index++ )
                    ;
                // one too far - since index will be incremented again in the for loop
                index--;

                if (level < num_level) {
                    num_index = start_index;
                    num_level = level;
                    num_len   = (index+1)-start_index;
                }

            }
            // if we encounter the start of a symbol name
            else if( IS_ALPHA(c) ) {
                ///@todo Implement symbol name parsing
                pindex_t start_index = index;
                pindex_t sym_index_tmp; ///< Used to explore the presence of a symbol parameter

                // found one more symbol
                sym_count++;

                // get over symbol name
                for (; (index < str_len) && IS_ALPHA(str[index]); index++)
                	;
                // one too far - since index will be incremented again in the for loop
                index--;

                // navigate to next real character -- using a temp index
                for (sym_index_tmp = index+1; (sym_index_tmp < str_len) && IS_WHITESPACE(str[sym_index_tmp]); sym_index_tmp++)
                	;

                // check for presence of '(' signifying the start of a symbol parameter
                if ( (sym_index_tmp < str_len) && (str[sym_index_tmp] == '(') ) {
                	size_t end;
                	// find end of parameter string
                	end = find_matching(str_len, str, sym_index_tmp);

                	// check if the parameter string is not valid
                	if (end == PINDEX_BAD) {
                		rerror("Syntax Error - Unmatched parenthesis around symbol parameter");
                	}

                	// parameter is valid
                	// move index over symbol parameter also
                	index = end;
                }

                // record sym if lowest level
                if(level < sym_level) {
                	sym_index = start_index;
                	sym_level = level;
                	sym_len   = (index+1) - start_index;
                }
            }
            else {
                // Throw Error - Unknown character encountered
            	rerror("Syntax Error - Unrecognized character \'%c\'", c);
            }
            break;

        } // switch() end

    } // for() end

    /*
     * Stage 2
     * Analyze what happened
     */

    // Two Main Cases: 1) Found an operation -- multiple terms 2) Else, found just a number

	/* TODO (craig#1#04/07/2014): Syntax Sanity Check. Check level==0, level_changed if op_level is set... */
    ///@todo Do series of additional syntax checks here.
    ///@todo Syntax Check: Check that level==0
    ///@todo Syntax Check: Check that oparen_count==cparen_count

	/* If operation was detected, use it */
	if (op_index != SIZE_T_MAX) {
		// OpIndex + OpLevel + NullByte
		// (size of str to left of op) + (# of parens to add) + (1 null byte)
		size_t left_buf_size = op_index + op_level + sizeof(char);
		// StrLen - ( OpIndex + 1 ) + Level + NullByte
		// str_len - (op_index + 1) + (# of parens to add) + (1 null byte)
		size_t right_buf_size = str_len - (op_index + 1) + op_level + sizeof(char);

		// We found the primary left-most operation
		expression_t left, right;

		/* Create temporary sub expression string buffers */
		char left_buf[ left_buf_size ];
		char right_buf[ right_buf_size ];

		/* Build left expression buffer */
		memset(left_buf + ( left_buf_size - (op_level + sizeof(char)) ), ')', op_level); // place op_level many closing parens to match the op_level open ones
		str_cpy(left_buf , str , op_index); // will copy op_index bytes
		left_buf[left_buf_size-1] = '\0'; // place ending NULL byte
//		strncpy(left_buf, str, op_index); // will copy op_index bytes then one NULL byte (NO NULL BYTE PLACED when n char limit reached)

		/* Build right expression buffer */
		memset(right_buf, '(', op_level);
		// to=(at the beginning, just past any open parens) from=(one past op_index) count=(diff. of the original length and the count up to and including the operation)
		str_cpy(right_buf+op_level , str+op_index+1 , str_len-(op_index+1));
		right_buf[right_buf_size-1] = '\0'; // place ending NULL byte
//		strncpy(right_buf + op_level, str + op_index + 1, str_len - op_index - 1);

		/* Recurse on the left and right expressions */
		left  = string_to_expression(strlen(left_buf), left_buf);
		right = string_to_expression(strlen(right_buf), right_buf);

		/* Create symbolic expression from the sub expressions and the operation */
		return expression_new_tree(str[op_index], left, right);
	}

	/* If only a symbol detected, use it */
	else if (sym_index != SIZE_T_MAX) {
		return expression_new_sym( string_to_sym(sym_len, &str[sym_index]) );
	}

	/* If number detected, use it */
	else if (num_index != SIZE_T_MAX) {
		if (num_count == 1) {
			/* Create value expression using the single number */
			return expression_new_value( string_to_value(num_len, &str[num_index]) ); // since we give the exact start location of num, buffer size should just be number length
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
