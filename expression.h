/*
 * errors.h
 *
 *  Created on: Apr 6, 2014
 *      Author: Craig Hesling
 */

#ifndef EXPRESSION_H_INCLUDED
#define EXPRESSION_H_INCLUDED


#define EXP_BUF_SIZE 256
typedef char exp_buf[EXP_BUF_SIZE];

typedef long int value_t;
typedef struct expression *expression_t;

/* The symbolic structure for the expression structure */
struct expression_data_sym {
    /*enum expression_operation {
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV
    } op;*/

    char op; // '+', '-', '*', or '/'

    expression_t left;
    expression_t right;
};

/* An expression is either a symbolic value of an operation on two other expressions or a direct value (evaluated expression) */
typedef struct expression {
    enum expression_type {
        EXP_VALUE,
        EXP_SYMBOLIC
    } type;

    union {
        struct expression_data_sym sym;
        value_t val;
    } data;
} *expression_t;


/* expression access functions */

expression_t
expression_new (void);

expression_t
expression_new_value (value_t val);

expression_t
expression_new_sym (char op,
                    expression_t left,
                    expression_t right);

void
expression_free (expression_t exp);


/* manipulation functions */

value_t
expression_evaluate (expression_t exp);

void
expression_to_string (char *str, expression_t exp);

expression_t
string_to_expression (size_t str_len,
					  char const *str);

#endif // EXPRESSION_H_INCLUDED
