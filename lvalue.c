/*
 * definitions for the internal data type
 *
 */

#include <stdio.h>
#include "lvalue.h"

lval lval_num(double x)
{
	lval v;
	v.type = LVAL_NUM;
	v.num = x;
	return v;
}

lval lval_err(int x)
{
	lval v;
	v.type = LVAL_ERR;
	v.err = x;
	return v;
}

void lval_print(lval v)
{
	switch (v.type) {
		// print number
		case LVAL_NUM: printf("%g", v.num); break;

		// error
		case LVAL_ERR:
			if (v.err == LERR_DIV_ZERO) {
				printf("Error: division by zero");
			} else if (v.err == LERR_BAD_OP) {
				printf("Error: invalid operation");
			} else if (v.err == LERR_BAD_NUM) {
				printf("Error: invalid number");
			} else {
				printf("Internal error!");
			}
		break;
	}
}
void lval_println(lval v) { lval_print(v); putchar('\n'); }

