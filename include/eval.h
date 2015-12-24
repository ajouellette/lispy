#ifndef _EVAL_H
#define _EVAL_H
#include "mpc.h"
#include "lvalue.h"
/*
 * Evaluation functions
 *
 */

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
lval *lval_eval(lenv *e, lval *v);

/*
 * Evaluate an S-expression
 */
lval *lval_eval_sexpr(lenv *e, lval *v);

/*
 * TODO:
 */
lval *lval_take(lval *v, int i);

lval *lval_pop(lval *v, int i);

/*
 * Evaluate a built-in operation
 */
lval *builtin(lenv *e, lval *a, char *f);
/*
 * Evaluate a built-in math op
 */
lval *builtin_op(lenv *e, lval *a, char *f);
/*
 * Return the first element of a q-expr
 */
lval *builtin_head(lenv *e, lval *a);
/*
 * Return q-expr with first element removed
 */
lval *builtin_tail(lenv *e, lval *a);
/*
 * Joins one or more q-expressions into one
 */
lval *builtin_join(lenv *e, lval *a);
/*
 * Returns a q-expr from one or more s-expressions
 */
lval *builtin_list(lenv *e, lval *a);
/*
 * Evaluate a q-expr as if it was an s-expr
 */
lval *builtin_eval(lenv *e, lval *a);
/*
 * Returns the number of elements of a q-expr
 */
lval *builtin_length(lenv *e, lval *a);
/*
 * Builtin math functions
 */
lval *builtin_add(lenv *e, lval *a);
lval *builtin_sub(lenv *e, lval *a);
lval *builtin_mul(lenv *e, lval *a);
lval *builtin_div(lenv *e, lval *a);

lval *lval_join(lval *x, lval *y);

/*
 * Min and Max functions
 */
double max(double x, double y);
double min(double x, double y);

#endif
