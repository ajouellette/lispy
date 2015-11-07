# Lispy
A simple Lisp implementation in C.

This project was inspired by Daniel Holden's book 
[*Build Your Own Lisp*](http://www.buildyourownlisp.com).

# Requirements
- readline
- [mpc](https://github.com/orangeduck/mpc)

# Usage
Run `make` to build.

Executable file `lispy` will be in `bin/`.

## Examples
```
lispy> + (* 3 7) (- 23 5)
39
lispy> ^ 2 (+ 3 7)
1024
lispy> * 12.02 3
36.03
```
