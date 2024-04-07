#Makefile ChessServer

GCC = gcc
DEBUG = -g -DDEBUG
CFLAGS = -Wall -ansi -std=c99 $(DEBUG)
INCLUDE = openssl
LFLAGS = -ansi -std=c99 -Wall -g -I$(INCLUDE)

TARGET = Login

all: ChessServer

Login.o: login.c login.h
	$(GCC) login.c $(CFLAGS) -c -o Login.o -lm

ChessServer: Login.o ChessServer.c
	gcc Login.o ChessServer.c $(CFLAGS) -o ChessServer -lm -lpthread


clean:
	rm -rf ChessServer
	rm -rf ./*.o