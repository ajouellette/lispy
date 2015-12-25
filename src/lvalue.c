/*
 * definitions for the internal data type
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvalue.h"
#include "eval.h"

lval *lval_num(double x)
{
	lval *v = malloc(sizeof(lval));
	v->type = LVAL_NUM;
	v->num = x;
	return v;
}

lval *lval_err(char *m)
{
	lval *v = malloc(sizeof(lval));
	v->type = LVAL_ERR;
	v->err = malloc(strlen(m) + 1);
	strcpy(v->err, m);
	return v;
}

lval *lval_sym(char *s)
{
	lval *v = malloc(sizeof(lval));
	v->type = LVAL_SYM;
	v->sym = malloc(strlen(s) + 1);
	strcpy(v->sym, s);
	return v;
}

lval *lval_fun(lbuiltin func)
{
	lval *v = malloc(sizeof(lval));
	v->type = LVAL_FUN;
	v->fun = func;
	return v;
}

lval *lval_sexpr(void)
{
	lval *v = malloc(sizeof(lval));
	v->type = LVAL_SEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

lval *lval_qexpr(void)
{
	lval *v = malloc(sizeof(lval));
	v->type = LVAL_QEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

void lval_del(lval *v)
{
	switch (v->type) {
		// do nothing for a number or function
		case LVAL_NUM: break;
		case LVAL_FUN: break;

		// free string data
		case LVAL_ERR: free(v->err); break;
		case LVAL_SYM: free(v->sym); break;

		// delete all elements of an S or Q expr
		case LVAL_SEXPR:
		case LVAL_QEXPR:
			for (int i = 0; i  < v->count; i++) {
				lval_del(v->cell[i]);
			}
			free(v->cell);
		break;
	}
	free(v);
}

lval *lval_copy(lval *v)
{
	lval *x = malloc(sizeof(lval));
	x->type = v->type;
	
	switch (v->type) {
		// directly copy numbers and functions
		case LVAL_NUM: x->num = v->num; break;
		case LVAL_FUN: x->fun = v->fun; break;

		// copy strings
		case LVAL_ERR:
			x->err = malloc(strlen(v->err) + 1);
			strcpy(x->err, v->err); break;
		case LVAL_SYM:
			x->err = malloc(strlen(v->err) + 1);
			strcpy(x->err, v->err); break;

		// copy lists by copying each element
		case LVAL_SEXPR:
		case LVAL_QEXPR:
			x->count = v->count;
			x->cell = malloc(sizeof(lval*) * v->count);
			for (int i = 0; i < v->count; i++) {
				x->cell[i] = lval_copy(v->cell[i]);
			}
		break;
	}

	return x;
}

void lval_print(lval *v)
{
	switch (v->type) {
		// print number with %g to avoid trailing zeroes
		case LVAL_NUM:   printf("%g", v->num); break;
		case LVAL_ERR:   printf("Error: %s", v->err); break;
		case LVAL_SYM:   printf("%s", v->sym); break;
		case LVAL_FUN:   printf("<function>"); break;
		case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
		case LVAL_QEXPR: lval_expr_print(v, '{', '}'); break;
	}
}

void lval_expr_print(lval *v, char open, char close)
{
	putchar(open);
	// print lvals in list
	for (int i = 0; i < v->count; i++) {
		lval_print(v->cell[i]);
		if (i != v->count - 1) {
			putchar(' ');
		}
	}
	putchar(close);
}

void lval_println(lval *v)
{
	lval_print(v);
	putchar('\n');
}

lenv *lenv_new(void)
{
	lenv *e = malloc(sizeof(lenv));
	e->count = 0;
	e->syms = NULL;
	e->vals = NULL;
	return e;
}

void lenv_del(lenv *e)
{
	for (int i = 0; i < e->count; i++) {
		free(e->syms[i]);
		lval_del(e->vals[i]);
	}
	free(e->syms);
	free(e->vals);
	free(e);
}

lval *lenv_get(lenv *e, lval *k)
{
	// iterate over all items in environment
	for (int i = 0; i < e->count; i++) {
		// check if stored string matches the symbol string
		if (strcmp(e->syms[i], k->sym) == 0) {
			return lval_copy(e->vals[i]);
		}
	}
	// return error
	return lval_err("undefined symbol");
}

void lenv_put(lenv *e, lval *k, lval *v)
{
	// iterate over all items in environmnet
	for (int i = 0; i < e->count; i++) {
		// check if symbol is defined and update value
		if (strcmp(e->syms[i], k->sym) == 0) {
			lval_del(e->vals[i]);
			e->vals[i] = lval_copy(v);
			return;
		}
	}
	// make new entry
	e->count++;
	e->vals = realloc(e->vals, sizeof(lval*) * e->count);
	e->syms = realloc(e->syms, sizeof(lval*) * e->count);

	e->vals[e->count - 1] = lval_copy(v);
	e->syms[e->count - 1] = malloc(strlen(k->sym) + 1);
	strcpy(e->syms[e->count - 1], k->sym);
}

void lenv_add_builtin(lenv *e, char *name, lbuiltin func)
{
	lval *k = lval_sym(name);
	lval *v = lval_fun(func);
	lenv_put(e, k, v);
	lval_del(k);
	lval_del(v);
}

void lenv_add_builtins(lenv *e)
{
	// list functions
	lenv_add_builtin(e, "list", builtin_list);
	lenv_add_builtin(e, "head", builtin_head);
	lenv_add_builtin(e, "tail", builtin_tail);
	lenv_add_builtin(e, "eval", builtin_eval);
	lenv_add_builtin(e, "join", builtin_join);
	// math functions
	lenv_add_builtin(e, "+", builtin_add);
	lenv_add_builtin(e, "-", builtin_sub);
	lenv_add_builtin(e, "*", builtin_mul);
	lenv_add_builtin(e, "/", builtin_div);
	lenv_add_builtin(e, "^", builtin_pow);
	lenv_add_builtin(e, "%", builtin_mod);
}
