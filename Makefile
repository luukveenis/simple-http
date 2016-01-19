#makefile for project 1 works both in linux and unix system now

.c.o:
	gcc -g -c $?

# compile client and server
all: client server

# compile client only 
client: client.o  util.o
	gcc -g -o client client.o  util.o

#compile server only
server: server.o util.o
	gcc -g -o server server.o util.o

clean:
	rm *.o client server
