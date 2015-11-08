/*
 * lispy
 *
 * A simple lisp implementation
 * currently only math operations implemented
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <editline/readline.h>
#include "mpc/mpc.h"
#include "lvalue.h"

/* Evaluate a parse tree */
lval eval(mpc_ast_t *tree);
/* Evaluate x <operation> y */
lval eval_op(lval x, char *op, lval y);

int main(int argc, char *argv[])
{
	// create parsers
	mpc_parser_t *Number  = mpc_new("number");
	mpc_parser_t *Symbol  = mpc_new("symbol");
	mpc_parser_t *Sexpr   = mpc_new("sexpr");
	mpc_parser_t *Expr    = mpc_new("expr");
	mpc_parser_t *Lispy   = mpc_new("lispy");

	// define language
	mpca_lang(MPCA_LANG_DEFAULT,
	"                                                             \
	number  : /-?[0-9]+\\.?[0-9]*/ ;                              \
	symbol  : '+' | '-' | '*' | '/' | '%' | '^' ;                 \
	sexpr   : '(' <expr>* ')' ;                                   \
	expr    : <number> | <symbol> | <sexpr> ;                     \
	lispy   : /^/ <expr>* /$/ ;                                   \
	", Number, Symbol, Sexpr, Expr, Lispy);

	puts("Lispy version 0.0.0.1.1");
	puts("Press Ctrl-c to exit\n");

	while (1) {

		// display prompt and get input
		char *input = readline("lispy> ");

		// add input to history
		add_history(input);

		// parse input and evaluate
		mpc_result_t r;
		if (mpc_parse("<stdin>", input, Lispy, &r)) {
			// on success print result
			lval result = eval(r.output);
			lval_println(result);
			mpc_ast_delete(r.output);
		} else {
			// otherwise print error
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}

		free(input);
	}

	// cleanup
	mpc_cleanup(5, Number, Symbol, Sexpr, Expr, Lispy);
	return 0;
}

lval eval(mpc_ast_t *tree)
{
	// if tagged as number, return it, checking for errors
	if (strstr(tree->tag, "number")) {
		errno = 0;
		double x = strtod(tree->contents, NULL);
		return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
	}

	// the operator is the 2nd child
	char *op = tree->children[1]->contents;

	lval x = eval(tree->children[2]);

	int i = 3;
	while (strstr(tree->children[i]->tag, "expr")) {
		x = eval_op(x, op, eval(tree->children[i]));
		i++;
	}

	return x;
}

lval eval_op(lval x, char *op, lval y)
{
	// if either value is an error, return it
	if (x.type == LVAL_ERR) { return x; }
	if (y.type == LVAL_ERR) { return y; }

	// otherwise fo math
	if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
	if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
	if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
	// division is special
	if (strcmp(op, "/") == 0) {
		return y.num == 0
			? lval_err(LERR_DIV_ZERO)
			: lval_num(x.num / y.num);
	}
	if (strcmp(op, "%") == 0) {
		return y.num == 0
			? lval_err(LERR_DIV_ZERO)
			: lval_num(fmod(x.num, y.num));
	}
	if (strcmp(op, "^") == 0) { return lval_num(pow(x.num, y.num)); }

	return lval_err(LERR_BAD_OP);
}
