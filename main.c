/*
 * errors.c
 *
 *  Created on: Apr 6, 2014
 *      Author: Craig Hesling
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "errors.h"
#include "expression.h"

void
rtests(void) {
	long int i = 0;
	char buf[] = "-199999";
	i = atol(buf);
	printf("Values: %s = %ld\n", buf, i);
}

void
test0(void) {
    struct expression e0, e1, e2, e3, e4; //, e5, e6, e7, e8, e9;
    exp_buf buf;

    e0.type = e1.type = e2.type = EXP_VALUE;
    e3.type = e4.type = EXP_SYMBOLIC;

    e0.data.val = 10;
    e1.data.val = 20;
    e2.data.val = 128;

    e3.data.sym.op = e4.data.sym.op = '+';

    e3.data.sym.left = &e0;
    e3.data.sym.right = &e1;

    e4.data.sym.left = &e3;
    e4.data.sym.right = &e2;

    expression_to_string(buf, &e4);

	printf("Expression: %s = %ld\n", buf, expression_evaluate(&e4));
}

void
test1(char *str) {
	char buf[512] = {0};
	expression_t e1;

	if (!str) {
		fprintf(stderr, "test1: Error - str is NULL\n");
		exit (2);
	}

	if (strlen(str) < 1) {
		fprintf(stderr, "test1: Error - str is not even 1 char long\n");
		exit (2);
	}

	e1 = string_to_expression(strlen(str), str);
	expression_to_string(buf,  e1);
	printf("Expression: %s = %ld\n", buf, expression_evaluate(e1));
}

int main(int argc, char *argv[]) {
	char buf[512] = {0};
	expression_t e1;

	puts("# Random Tests:");
	rtests();

	puts("# test0:");
	test0();

	puts("# main:");
	if (argc != 2) {
		fprintf(stderr, "main: Error - Need expression as only argument for test1()\n");
		return 1;
	}
	puts("# test1 ( argv[1] ):");
	test1(argv[1]);

	return 0;
}
