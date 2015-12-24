#ifndef _LVALUE_H
#define _LVALUE_H
/*
 * Definitions for Lisp values (lval)
 * and Lisp environments (lenv)
 *
 */
struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;


/*
 * Lisp value
 * a value can be one of the following:
 * number, error, s-expression, q-expression, symbol, or function
 *
 */
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR, LVAL_FUN };

/* 
 * lbuiltin is a function pointer that takes lenv* and lval* as
 * arguments and returns an lval*
 */
typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval {
	int type;

	double num;       // value of number
	char *err;        // error message
	char *sym;        // symbol
	lbuiltin fun;     // function
	/* list of lvals */
	int count;
	lval **cell;
};

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
 * Return pointer to a new lval function
 */
lval *lval_fun(lbuiltin func);
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
 * Copy an lval to a new lval
 */
lval *lval_copy(lval *v);

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


/*
 * Lisp environment
 * contains definitions of symbols
 */
struct lenv {
	int count;
	char **syms;     // symbols defined in env
	lval **vals;     // values of symbols
};

/*
 * Create a new environment
 */
lenv *lenv_new(void);
/*
 * Delete an environment
 */
void lenv_del(lenv *e);
/*
 * Get a value from the environment.
 * checks if the symbol is defined, if it is returns the value
 * if not returns an error
 */
lval *lenv_get(lenv *e, lval *k);
/*
 * Put a value into the environment.
 * if the symbol is already defined, update the value
 * else, define the new symbol
 */
void lenv_put(lenv *e, lval *k, lval *v);
/*
 * Add a builtin function to the environment
 */
void lenv_add_builtin(lenv *e, char *name, lbuiltin func);
/*
 * Add all builtins to environment
 */
void lenv_add_builtins(lenv *e);

#endif
