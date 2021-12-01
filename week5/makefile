all: client server

client: client.c
	gcc -o client client.c user.c

server: server.c
	gcc -o server server.c user.c

clean:
	rm -f *.o *~