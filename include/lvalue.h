#ifndef _LVALUE_H
#define _LVALUE_H

/*
 * Lisp value
 */
typedef struct lval {
	int type;       // error, number, symbol, or s-expr
	double num;     // value of number
	char *err;      // error message
	char *sym;      // symbol
	/* counter and pointer to a list of lvals */
	int count;
	struct lval **cell;
} lval;
/*
 * Enum for possible Lisp value types
 */
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };

/*
 * Return pointer to a new lval number
 */
lval *lval_num(double x);
/*
 * Return pointer to a new lval error
 */
lval *lval_err(char *m);
/*
 * Return pointer to a new lval symbol
 */
lval *lval_sym(char *s);
/*
 * Return pointer to a new lval S-expression
 */
lval *lval_sexpr(void);
/*
 * Return a pointer to a new Q-expression
 */
lval *lval_qexpr(void);

/*
 * Free memory used by a lval
 */
void lval_del(lval *v);

/*
 * Print an expression
 */
void lval_expr_print(lval *v, char open, char close);
/*
 * Print a lval
 */
void lval_print(lval *v);
/*
 * Print with a trailing newline
 */
void lval_println(lval *v);

#endif
