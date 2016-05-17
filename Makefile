# Daniel Porteous porteousd
# Makefile
# 2016

serverOBJ     	= server.o game.o instances.o logging.o
clientOBJ		= client.o
serverEXE    	= server
clientEXE		= client
CC      		= gcc
CFLAGS  		= -pthread -std=gnu99
# -O2 -m32

default: server client

server:   $(serverOBJ) Makefile
	$(CC) $(CFLAGS) -o $(serverEXE) $(serverOBJ)

client:   $(clientOBJ) Makefile
	$(CC) $(CFLAGS) -o $(clientEXE) $(clientOBJ)

clean:
	rm -f $(clientOBJ) $(clientEXE) $(serverOBJ) $(serverEXE)

server.o: game.h instances.h Makefile
game.o: game.h instances.h
instances.o: instances.h
logging.o: logging.h