#include "mpc.h"
#include "lvalue.h"
#include "eval.h"
double max(double x, double y);
double min(double x, double y);

lval *lval_read_num(mpc_ast_t *tree)
{
	errno = 0;
	double x = strtod(tree->contents, NULL);
	// make sure that conversion was successful
	return errno != ERANGE ?
		lval_num(x) : lval_err("invalid number");
}

lval *lval_read(mpc_ast_t *tree)
{
	// return symbol or number
	if (strstr(tree->tag, "number")) {
		return lval_read_num(tree);
	} else if (strstr(tree->tag, "symbol")) {
		return lval_sym(tree->contents);
	}

	// create list
	lval *x = NULL;
	if (strcmp(tree->tag, ">") == 0) {
		x = lval_sexpr();
	}
	if (strstr(tree->tag, "sexpr")) {
		x = lval_sexpr();
	} else if (strstr(tree->tag, "qexpr")) {
		x = lval_qexpr();
	}

	// fill in list
	for (int i = 0; i < tree->children_num; i++) {
		if (strcmp(tree->children[i]->contents, "(") == 0) continue;
		if (strcmp(tree->children[i]->contents, ")") == 0) continue;
		if (strcmp(tree->children[i]->contents, "{") == 0) continue;
		if (strcmp(tree->children[i]->contents, "}") == 0) continue;
		if (strcmp(tree->children[i]->tag, "regex") == 0)  continue;

		x = lval_add(x, lval_read(tree->children[i]));
	}

	return x;
}

lval *lval_add(lval *v, lval *x)
{
	v->count++;
	v->cell = realloc(v->cell, sizeof(lval*) * v->count);
	v->cell[v->count - 1] = x;
	return v;
}
lval *lval_eval_sexpr(lval *v)
{
	// evaluate children
	for (int i = 0; i < v->count; i++) {
		v->cell[i] = lval_eval(v->cell[i]);
	}

	// error checking
	for (int i = 0; i < v->count; i++) {
		if (v->cell[i]->type == LVAL_ERR) {
			return lval_take(v, i);
		}
	}

	// Empty expression
	if (v->count == 0) {
		return v;
	}

	// single expression
	if (v->count == 1) {
		return lval_take(v, 0);
	}

	// ensure first element is symbol
	lval* f = lval_pop(v, 0);
	if (f->type != LVAL_SYM) {
		lval_del(f);
		lval_del(v);
		return lval_err("s-expression does not start with a symbol");
	}

	// builtin operations
	lval* result = builtin_op(v, f->sym);
	lval_del(f);

	return result;
}

lval* lval_eval(lval* v) {
	// Evaluate S-expressions
	if (v->type == LVAL_SEXPR) {
		return lval_eval_sexpr(v);
	}
	// All other lval types remain the same
	return v;
}

lval* lval_pop(lval* v, int i) {
	// Find the item at "i"
	lval* x = v->cell[i];

	// Shift memory after the item at "i" over the top
	memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->count - i - 1));

	// Decrease the count of items in the list
	v->count--;

	// Reallocate the memory used
	v->cell = realloc(v->cell, sizeof(lval*) * v->count);
	return x;
}

lval* lval_take(lval* v, int i) {
	lval* x = lval_pop(v, i);
	lval_del(v);
	return x;
}

lval* builtin_op(lval* a, char* op) {

	// Ensure all arguments are numbers
	for (int i = 0; i < a->count; i++) {
		if (a->cell[i]->type != LVAL_NUM) {
			lval_del(a);
			return lval_err("cannot operate on non-number");
		}
	}

	// Pop the first element
	lval* x = lval_pop(a, 0);

	// If no arguments and sub then perform unary negation
	if (a->count == 0) {
		if (strcmp(op, "-") == 0) {
			x->num = -x->num;
		} else {
			x = lval_err("invalid syntax");
		}
	}

	// While there are still elements remaining
	while (a->count > 0) {

		// Pop the next element
		lval* y = lval_pop(a, 0);

		if (strcmp(op, "+") == 0) {
			x->num += y->num;
		}
		if (strcmp(op, "-") == 0) {
			x->num -= y->num;
		}
		if (strcmp(op, "*") == 0) {
			x->num *= y->num;
		}
		if (strcmp(op, "/") == 0) {
			if (y->num == 0) {
				lval_del(x);
				lval_del(y);
				x = lval_err("division by zero");
				break;
			}
			x->num /= y->num;
		}
		if (strcmp(op, "%") == 0) {
			if (y->num == 0) {
				lval_del(x);
				lval_del(y);
				x = lval_err("division by zero");
				break;
			}
			x->num = fmod(x->num, y->num);
		}
		if (strcmp(op, "^") == 0) {
			x->num = pow(x->num, y->num);
		}
		if (strcmp(op, "min") == 0) {
			x->num = min(x->num, y->num);
		}
		if (strcmp(op, "max") == 0) {
			x->num = max(x->num, y->num);
		}

		lval_del(y);
	}
	
	lval_del(a);
	return x;
}

double max(double x, double y)
{
	if (x > y) {
		return x;
	} else {
		return y;
	}
}

double min(double x, double y)
{
	if (x < y) {
		return x;
	} else {
		return y;
	}
}
