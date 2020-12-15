CC=clang++
CFLAGS=-Wall -std=c++17 -O2 -L/usr/lib -lmagic -lssl -lcrypto
REGEX=-DRegex
DEBUG=-DDEBUG -g
SSL=-DSSL_ENABLE
TESTING=-DLOCAL -DVERBOSE
OBJECTS=server.o
.PHONY: all server debug
server: server.o
	$(CC) server.cpp $(CFLAGS) $(REGEX) -o server.o	
debug: server.o
	$(CC) server.cpp $(CFLAGS) $(DEBUG) -o server.o
testing: server.o
	$(CC) server.cpp  $(CFLAGS) $(REGEX) $(TESTING) -o server.o		
ssl: server.o
	$(CC) server.cpp  $(CFLAGS) $(REGEX) $(TESTING) $(SSL) -o server.o	
clean:
	rm -f $(OBJECTS)