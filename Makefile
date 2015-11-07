all:
	mkdir -p bin
	gcc -W -Wall -g -o bin/lispy lispy.c lvalue.c mpc/mpc.c -ledit -lm

clean:
	rm -rf bin/
