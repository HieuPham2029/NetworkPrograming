CFLAGS = -c -Wall
CC = gcc
LIBS = -lm

all: user

user: user.o
	${CC} user.o -o user

user.o: user.c	
	${CC} ${CFLAGS} user.c

clean: 
	re -f *.o *~