CC=g++
CFLAGS=-I.

all: readImg.o

readImg.o: readImg.cpp
	$(CC) readImg.cpp -g -o readImg.o

.PHONY: clean
clean:
	rm -f *.o