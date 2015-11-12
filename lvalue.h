#ifndef _lvalue_h
#define _lvalue_h

/*
 * Lisp value
 */
typedef struct {
	int type;       // 0 if number, 1 if error
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
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR };

/*
 * Create new lval number, return pointer
 */
lval *lval_num(double x);
/*
 * Create new lval error, return pointer
 */
lval *lval_err(char *m);
/*
 * Return pointer to new lval symbol
 */
lval *lval_sym(char *s);
/*
 * Return pointer to new lval S-expr
 */
lval *lval_sexpr(void);

/*
 * Clean up a lval
 */
void lval_del(lval *v);

/*
 * Print an lval
 */
void lval_expr_print(lval *v, char open, char close);
void lval_print(lval *v);
void lval_println(lval *v);

#endif
