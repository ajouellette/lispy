#ifndef _EVAL_H
#define _EVAL_H
#include "mpc.h"
#include "lvalue.h"

/* Macros to help with eval functions */
#define LASSERT(args, cond, err) \
	if (!(cond)) { lval_del(args); return lval_err(err); }

/*
 * Read number from parse tree
 */
lval *lval_read_num(mpc_ast_t *tree);

/*
 * Read expression from parse tree
 */
lval *lval_read(mpc_ast_t *tree);

/*
 * Add x (lval) to v (a list of lvals)
 */
lval *lval_add(lval *v, lval *x);

/*
 * Evaluate an expression
 */
lval *lval_eval(lval *v);

/*
 * Evaluate an S-expression
 */
lval *lval_eval_sexpr(lval *v);

/*
 * TODO:
 */
lval *lval_take(lval *v, int i);

lval *lval_pop(lval *v, int i);

/*
 * Evaluate a built-in operation
 */
lval *builtin(lval *a, char *op);
/*
 * Evaluate a built-in math op
 */
lval *builtin_math(lval *a, char *op);
/*
 * Return the first element of a q-expr
 */
lval *builtin_head(lval *a);
/*
 * Return q-expr with first element removed
 */
lval *builtin_tail(lval *a);
/*
 * Joins one or more q-expressions into one
 */
lval *builtin_join(lval *a);
/*
 * Returns a q-expr from one or more s-expressions
 */
lval *builtin_list(lval *a);
/*
 * Evaluate a q-expr as if it was an s-expr
 */
lval *builtin_eval(lval *a);
/*
 * Returns the number of elements of a q-expr
 */
lval *builtin_length(lval *a);

lval *lval_join(lval *x, lval *y);

/*
 * Min and Max functions
 */
double max(double x, double y);
double min(double x, double y);

#endif
