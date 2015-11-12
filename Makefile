CC = gcc
OUT = bin
CFLAGS = -W -Wall -g -lm -ledit -I include/ -I mpc/

objects = lispy.o lvalue.o eval.o mpc.o

all: $(objects)
	mkdir -p $(OUT)
	$(CC) $(CFLAGS) $(objects) -o $(OUT)/lispy

mpc.o: mpc/mpc.c mpc/mpc.h
	$(CC) $(CFLAGS) -c mpc/mpc.c

lispy.o: src/lispy.c include/lvalue.h include/eval.h
	$(CC) $(CFLAGS) -c src/lispy.c

lvalue.o: src/lvalue.c include/lvalue.h
	$(CC) $(CFLAGS) -c src/lvalue.c

eval.o: src/eval.c include/eval.h
	$(CC) $(CFLAGS) -c src/eval.c

clean:
	rm -f *.o
	rm -rf bin/
