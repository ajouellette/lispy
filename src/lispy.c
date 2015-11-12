/*
 * lispy
 *
 * A simple lisp implementation
 * currently only math operations implemented
 *
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <editline/readline.h>
#include "mpc.h"
#include "lvalue.h"

/* Read number from parse tree */
lval *lval_read_num(mpc_ast_t *tree);
/* read expression from parse tree */
lval *lval_read(mpc_ast_t *tree);
/* Add x (lval) to v (a list of lvals) */
lval *lval_add(lval *v, lval *x);
/* Evaluate an S-expression */
lval *lval_eval_sexpr(lval *v);
lval *lval_eval(lval *v);
/* Is the string all blank */
bool is_blank(char *string);

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
	puts("Type \'exit\' to exit\n");

	while (1) {

		// display prompt and get input
		char *input = readline("lispy> ");
		if (strcmp(input, "exit") == 0)
			return 0;
		if (is_blank(input))
			continue;

		// add input to history
		add_history(input);

		// parse input and evaluate
		mpc_result_t r;
		if (mpc_parse("<stdin>", input, Lispy, &r)) {
			lval *x = lval_read(r.output);
			lval_println(x);
			lval_del(x);
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


lval *lval_read_num(mpc_ast_t *tree)
{
	errno = 0;
	double x = strtod(tree->contents, NULL);
	return errno != ERANGE ?
		lval_num(x) : lval_err("invalid number");
}

lval *lval_read(mpc_ast_t *tree)
{
	// symbol or number
	if (strstr(tree->tag, "number")) { return lval_read_num(tree); }
	if (strstr(tree->tag, "symbol")) { return lval_sym(tree->contents); }

	// create list
	lval *x = NULL;
	if (strcmp(tree->tag, ">") == 0) { x = lval_sexpr(); }
	if (strstr(tree->tag, "sexpr")) { x = lval_sexpr(); }

	// fill in list
	for (int i = 0; i < tree->children_num; i++) {
		if (strcmp(tree->children[i]->contents, "(") == 0) continue;
		if (strcmp(tree->children[i]->contents, ")") == 0) continue;
		if (strcmp(tree->children[i]->contents, "{") == 0) continue;
		if (strcmp(tree->children[i]->contents, "}") == 0) continue;
		if (strcmp(tree->children[i]->tag, "regex") == 0) continue;
		x = lval_add(x, lval_read(tree->children[i]));
	}

	return x;
}

lval *lval_add(lval *v, lval *x)
{
	v->count++;
	v->cell = realloc(v->cell, sizeof(lval*) * v->count);
	v->cell[v->count - 1] = x;
	return v;
}

lval *lval_eval_sexpr(lval *v)
{
	// evaluate children
	for (int i = 0; i < v->count; i++) {
		v->cell[i] = lval_eval(v->cell[i]);
	}

	return v;
}

lval *lval_eval(lval *v)
{
	return v;
}

bool is_blank(char *string)
{
	int length = strlen(string);
	for (int i = 0; i < length; i++) {
		if (!isblank(string[i])) {
			return false;
		}
	}
	return true;
}
