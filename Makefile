# Daniel Porteous porteousd
# Makefile
# 2016

serverOBJ     	= server.o game.o threads.o
clientOBJ		= client.o
serverEXE    	= server
clientEXE		= client
CC      		= gcc
CFLAGS  		= -Wall -pthread
# -O2 -m32

default: server client

server:   $(serverOBJ) Makefile
	$(CC) $(CFLAGS) -o $(serverEXE) $(serverOBJ)

client:   $(clientOBJ) Makefile
	$(CC) $(CFLAGS) -o $(clientEXE) $(clientOBJ)

clean:
	rm -f $(clientOBJ) $(clientEXE) $(serverOBJ) $(serverEXE)

server.o: game.h threads.h Makefile
game.o: game.h threads.h
threads.o: threads.h