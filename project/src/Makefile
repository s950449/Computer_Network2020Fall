CC=clang++
CFLAGS=-Wall -std=c++17 -O2 -L/usr/lib -lmagic
REGEX=-DRegex
DEBUG=-DDEBUG -g
OBJECTS=server.o
.PHONY: all server debug
server: server.o
	$(CC) $(CFLAGS) $(REGEX) server.cpp -o server.o	
debug: server.o
	$(CC) $(CFLAGS) $(DEBUG) server.cpp -o server.o
clean:
	rm -f $(OBJECTS)