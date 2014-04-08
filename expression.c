/*
 * errors.c
 *
 *  Created on: Apr 6, 2014
 *      Author: Craig Hesling
 */



#include <stdio.h>
#include <stdlib.h>
//#include <limits.h>
#include <string.h>

#include "errors.h"
#include "expression.h"


/***********************************************
 * expression_t allocation interface functions
 ***********************************************/

expression_t
expression_new (void) {
    expression_t exp = (expression_t) malloc(sizeof(struct expression));
    /* if (!exp) // throw error - expression_new: malloc could not do allocation */
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
expression_new_sym (char op,
                    expression_t left,
                    expression_t right) {

    expression_t exp = expression_new ();
    exp->type = EXP_SYMBOLIC;
    exp->data.sym.op = op;
    exp->data.sym.left = left;
    exp->data.sym.right = right;
    return exp;
}

void
expression_free (expression_t exp) {
    free(exp);
}



/***********************************************
 * expression_t manipulation functions
 ***********************************************/

/* Evaluate Expression recursively */
value_t
expression_evaluate (expression_t exp) {
    if (exp->type == EXP_VALUE) return exp->data.val;
    else /*if (exp->type == EXP_SYMBOLIC)*/ // Optimize for speed not errors
    {
        switch (exp->data.sym.op) {
        case '+':
            return expression_evaluate(exp->data.sym.left) + expression_evaluate(exp->data.sym.right);
            break;
        case '-':
            return expression_evaluate(exp->data.sym.left) - expression_evaluate(exp->data.sym.right);
            break;
        case '*':
            return expression_evaluate(exp->data.sym.left) * expression_evaluate(exp->data.sym.right);
            break;
        case '/':
            return expression_evaluate(exp->data.sym.left) / expression_evaluate(exp->data.sym.right);
            break;
        default:
            /* throw error - invalid operation in expression */
            break;
        }
    }

    /* throw error - invalid state in expression */
    //return -1; //error
}


#define EXP_BUF_SIZE 256
typedef char exp_buf[EXP_BUF_SIZE];

/* Expression to String */
void
expression_to_string (char *str,
					  expression_t exp) {
    if (exp->type == EXP_VALUE) {
            snprintf(str, EXP_BUF_SIZE, "%ld", exp->data.val);
    }
    else /*if (exp->type == EXP_SYMBOLIC)*/ // Optimize for speed not errors
    {
        exp_buf left, right;

        expression_to_string (left, exp->data.sym.left);
        expression_to_string (right, exp->data.sym.right);
        snprintf(str, EXP_BUF_SIZE, "(%s %c %s)", left, exp->data.sym.op, right);
    }
}



/*
 * Helper functions for String to Expression
 */
#define IS_WHITESPACE(c) ( ((c) == ' ') || ((c) == '\t') )
#define IS_DIGIT(c)      ( ( '0' <= (c) ) && ( (c) <= '9') )

#define SIZE_T_MAX ( ~( (size_t) (0) ) )

size_t
number_len(size_t str_len, char *str) {
    size_t index;
    for(index = 0; (index < str_len) && IS_DIGIT(str[index]); index++)
        ;
    return index;
}

/* Must have first char be digit */
value_t
string_to_value(size_t str_len, char const *str) {
	char buf[40]; // log10(2^128) ~ 39
	int index;

	// copy number digits
	for (index = 0; (index < str_len) && (index < (40-1)) /*need null byte*/ && IS_DIGIT(str[index]); index++) {
		buf[index] = str[index];
	}
	// place null-byte
	buf[index] = '\0';

	return atol(buf);
}



/* String to Expression */
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
		return expression_new_sym(str[op_index], left, right);
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

}
