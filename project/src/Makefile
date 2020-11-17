CC=g++
CFLAGS=-Wall -std=c++17 -O2
DEBUG=-DDEBUG -g
OBJECTS=server.o
.PHONY: all server clean
server: server.o
	$(CC) $(CFLAGS) server.cpp -o server.o	
debug: server.o
	$(CC) $(CFLAGS) $(DEBUG) server.cpp -o server.o
clean:
	rm -f $(OBJECTS)