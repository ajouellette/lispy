#ifndef _lvalue_h
#define _lvalue_h

/* Enum for possible lval types */
enum { LVAL_NUM, LVAL_ERR };

/* Enum for possible errors */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Lisp value */
typedef struct {
	int type;     // 0 if number, 1 if error
	double num;     // value of number
	int err;      // error code
} lval;

/* Create new lval number  */
lval lval_num(double x);
/* Create new lval error */
lval lval_err(int x);

/* Print an lval */
void lval_print(lval v);
void lval_println(lval v);

#endif
