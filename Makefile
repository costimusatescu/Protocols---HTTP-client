CC=gcc
CFLAGS=-I.

client: client.c parson.c parson.h requests.c helpers.c
		$(CC) -o client client.c parson.c requests.c helpers.c -Wall
		
run: client
		./client

clean:
		rm -f *.o client
