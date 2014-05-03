/**
 * @file main.c
 *
 * @date Apr 6, 2014
 * @author Craig Hesling
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "errors.h"
#include "types.h"
#include "workspace.h"
#include "expression.h"

#define BLACK  30
#define RED    31
#define GREEN  32
#define YELLOW 33
#define BLUE   34
#define PURPLE 35
#define TEAL   36
#define GREY   37
#define WHITE  38
#define xCOLOR(color,str) "\033["#color"m"str"\033[m"
#define COLOR(color,str) xCOLOR(color, #str)

#define EXPECT(exp) COLOR(GREEN, exp)
#define RESULT(res) COLOR(PURPLE, res)

/** Random Tests.
 * Tests the atol() system function.
 */
void
rtests(void) {
	long int i = 0;
	char buf[] = "-199999";

	/// Execute atol().
	i = atol(buf);

	/* Show results */
	printf("Values: %s = %ld\n", buf, i);
}

/** Tests \ref expression_to_string and then \ref expression_evaluate functions.
 * It tests expression_to_string and expression_evaluate on a manually created expression.
 * The expression is ( (10 + 20) + 128 ) where e4 is the outer most root node and e3 is inner parent node of e0 and e1.
 *
 *
 * @dot
 * digraph G{
 * e4 [label="e4\n+"];
 * e3 [label="e3\n+"];
 * e0 [label="e0\n10"];
 * e1 [label="e1\n20"];
 * e2 [label="e2\n128"];
 * e4 -> e3;
 * e4 -> e2;
 * e3 -> e0;
 * e3 -> e1;
 * }
 * @enddot
 */
void
test0(void) {
    struct expression e0, e1, e2, e3, e4; //, e5, e6, e7, e8, e9;
    exp_buf buf;
    value_t result;

    // e0, e1, e2 are all leaf values
    e0.type = e1.type = e2.type = EXP_VALUE;

    // e3, e4 are tree operations
    e3.type = e4.type = EXP_TREE;

    /// Call \ref value_new_lint for e0 e1 and e2.
    /// @code
    puts("## Try value_new_lint()");
    e0.data.val = value_new_lint(10);
    e1.data.val = value_new_lint(20);
    e2.data.val = value_new_lint(128);
    /// @endcode

    // e3 and e4: +
    e3.data.tree.op = e4.data.tree.op = '+';

    // e3: e0 + e1
    e3.data.tree.left = &e0;
    e3.data.tree.right = &e1;

    // e4 is root node
    // e4: e3 + e2
    e4.data.tree.left = &e3;
    e4.data.tree.right = &e2;

    /// Call \ref expression_to_string on e4.
    /// @code
    puts("## Try expression_to_string()");
    // get string of root node e4
    expression_to_string(buf, &e4);
    puts("## Show resultant string");
    puts("Expect: "EXPECT("((10 + 20) + 128)"));
    printf("Expression String: "RESULT("%s")"\n", buf);
    /// @endcode

    /// Call \ref expression_evaluate on e4.
    /// @code
    puts("## Try expression_evaluate()");
    result = expression_evaluate(&e4);
    assert(result.type == VAL_LINT);
    puts("## Show resultant values");
    puts("Expect: "EXPECT("((10 + 20) + 128) = 158"));
	printf("Expression: "RESULT("%s = %ld")"\n", buf, result.data.lint);
	/// @endcode
}


/** Tests \ref string_to_expression on the string argument and then tests \ref expression_to_string on it's output.
 * @param str The expression string to use.
 */
void
test1(char *str) {
	char buf[512] = {0};
	expression_t e1;
	value_t val;

	/* Check that there was a valid str arguent given */
	if (!str) {
		fprintf(stderr, "test1: Error - str is NULL\n");
		exit (2);
	}
	if (strlen(str) < 1) {
		fprintf(stderr, "test1: Error - str is not even 1 char long\n");
		exit (2);
	}

	printf("Given String: \"%s\"\n", str);

	/// Call \ref string_to_expression on str
	/// @code
	e1 = string_to_expression (strlen(str), str);
	/// @endcode

	/// Call \ref expression_to_string on result, e1
	/// @code
	expression_to_string (buf,  e1);
	/// @endcode

	printf("Expression(just string): "RESULT("%s")"\n", buf);

	// Show results
	val = expression_evaluate(e1);
	printf("Expression(both string and value): "RESULT("%s = %ld")"\n", buf, val.data.lint);

	expression_free(e1);
}
/// @callgraph
int main(int argc, char *argv[]) {
	workspace_init();

	//char buf[512] = {0};
	//expression_t e1;

	puts("# Random Tests:");
	rtests();

	printf("\n\n\n");

	puts("# test0:");
	test0();

	printf("\n\n\n");

	puts("# test1 (\"129\"):");
	test1("129");


	printf("\n\n\n");

	puts("# test1 (\"1 + 1\"):");
	test1("1 + 1");


	printf("\n\n\n");

	puts("# main:");
	if (argc != 2) {
		fprintf(stderr, "main: Error - Need expression as only argument for test1()\n");
		return 1;
	}
	puts("# test1 ( argv[1] ):");
	test1(argv[1]);

	return 0;
}
