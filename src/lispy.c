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

/* Evaluate an expression */
lval *lval_eval(lval *v);
/* Evaluate an S-expression */
lval *lval_eval_sexpr(lval *v);
lval* lval_take(lval* v, int i);
lval* lval_pop(lval* v, int i);
/* Evaluate a built-in operation */
lval* builtin_op(lval* a, char* op);

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

	// Read, Evaluate, Print loop
	while (1) {

		// display prompt and get input
		char *input = readline("lispy> ");
		if (strcmp(input, "exit") == 0) {
			free(input);
			break;
		}
		if (is_blank(input)) {
			continue;
		}

		// add input to history
		add_history(input);

		// parse input
		mpc_result_t r;
		if (mpc_parse("<stdin>", input, Lispy, &r)) {
			// on success evaluate and print
			lval *x = lval_eval(lval_read(r.output));
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
	// make sure that conversion was successful
	return errno != ERANGE ?
		lval_num(x) : lval_err("invalid number");
}

lval *lval_read(mpc_ast_t *tree)
{
	// return symbol or number
	if (strstr(tree->tag, "number")) {
		return lval_read_num(tree);
	} else if (strstr(tree->tag, "symbol")) {
		return lval_sym(tree->contents);
	}

	// create list
	lval *x = NULL;
	if (strcmp(tree->tag, ">") == 0) {
		x = lval_sexpr();
	}
	if (strstr(tree->tag, "sexpr")) {
		x = lval_sexpr();
	}

	// fill in list
	for (int i = 0; i < tree->children_num; i++) {
		if (strcmp(tree->children[i]->contents, "(") == 0) continue;
		if (strcmp(tree->children[i]->contents, ")") == 0) continue;
		if (strcmp(tree->children[i]->contents, "{") == 0) continue;
		if (strcmp(tree->children[i]->contents, "}") == 0) continue;
		if (strcmp(tree->children[i]->tag, "regex") == 0)  continue;

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

	// error checking
	for (int i = 0; i < v->count; i++) {
		if (v->cell[i]->type == LVAL_ERR) {
			return lval_take(v, i);
		}
	}

	// Empty expression
	if (v->count == 0) {
		return v;
	}

	// single expression
	if (v->count == 1) {
		return lval_take(v, 0);
	}

	// ensure first element is symbol
	lval* f = lval_pop(v, 0);
	if (f->type != LVAL_SYM) {
		lval_del(f);
		lval_del(v);
		return lval_err("s-expression does not start with a symbol");
	}

	// builtin operations
	lval* result = builtin_op(v, f->sym);
	lval_del(f);

	return result;
}

lval* lval_eval(lval* v) {
	// Evaluate S-expressions
	if (v->type == LVAL_SEXPR) {
		return lval_eval_sexpr(v);
	}
	// All other lval types remain the same
	return v;
}

lval* lval_pop(lval* v, int i) {
	// Find the item at "i"
	lval* x = v->cell[i];

	// Shift memory after the item at "i" over the top
	memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->count - i - 1));

	// Decrease the count of items in the list
	v->count--;

	// Reallocate the memory used
	v->cell = realloc(v->cell, sizeof(lval*) * v->count);
	return x;
}

lval* lval_take(lval* v, int i) {
	lval* x = lval_pop(v, i);
	lval_del(v);
	return x;
}

lval* builtin_op(lval* a, char* op) {

	// Ensure all arguments are numbers
	for (int i = 0; i < a->count; i++) {
		if (a->cell[i]->type != LVAL_NUM) {
			lval_del(a);
			return lval_err("cannot operate on non-number");
		}
	}

	// Pop the first element
	lval* x = lval_pop(a, 0);

	// If no arguments and sub then perform unary negation
	if ((strcmp(op, "-") == 0) && a->count == 0) {
		x->num = -x->num;
	}

	// While there are still elements remaining
	while (a->count > 0) {

		// Pop the next element
		lval* y = lval_pop(a, 0);

		if (strcmp(op, "+") == 0) {
			x->num += y->num;
		}
		if (strcmp(op, "-") == 0) {
			x->num -= y->num;
		}
		if (strcmp(op, "*") == 0) {
			x->num *= y->num;
		}
		if (strcmp(op, "/") == 0) {
			if (y->num == 0) {
				lval_del(x);
				lval_del(y);
				x = lval_err("division by zero");
				break;
			}
			x->num /= y->num;
		}
		if (strcmp(op, "%") == 0) {
			if (y->num == 0) {
				lval_del(x);
				lval_del(y);
				x = lval_err("division by zero");
				break;
			}
			x->num = fmod(x->num, y->num);
		}
		if (strcmp(op, "^") == 0) {
			x->num = pow(x->num, y->num);
		}

		lval_del(y);
	}
	
	lval_del(a);
	return x;
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

