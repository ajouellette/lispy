/*
 * definitions for the internal data type
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvalue.h"

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
		// do nothing for a number
		case LVAL_NUM: break;

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

void lval_print(lval *v)
{
	switch (v->type) {
		// print number with %g to avoid trailing zeroes
		case LVAL_NUM:   printf("%g", v->num); break;
		case LVAL_ERR:   printf("Error: %s", v->err); break;
		case LVAL_SYM:   printf("%s", v->sym); break;
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

