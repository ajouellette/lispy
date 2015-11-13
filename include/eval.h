#ifndef _EVAL_H
#define _EVAL_H
#include "mpc.h"
#include "lvalue.h"

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

lval* lval_take(lval* v, int i);

lval* lval_pop(lval* v, int i);

/*
 * Evaluate a built-in operation
 */
lval* builtin_op(lval* a, char* op);

#endif
