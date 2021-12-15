CC = gcc

all: client.c server.c user.c
	$(CC) -o client client.c user.c
	$(CC) -pthread -o server server.c user.c

clean: 
	rm client server