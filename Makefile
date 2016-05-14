# Daniel Porteous porteousd
# Makefile
# 2016

OBJ     = server.o game.o threads.o
CC      = gcc
EXE     = server
CFLAGS  = -Wall -pthread
# -O2 -m32

simulation:   $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

clean:
	rm -f $(OBJ) $(EXE)

usage: $(EXE)
	./$(EXE)

server.o: game.h threads.h Makefile
game.o: game.h threads.h
threads.o: threads.h