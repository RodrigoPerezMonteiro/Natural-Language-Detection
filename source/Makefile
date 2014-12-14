CFLAGS =
OBJECTS = main.c 
CC = gcc

all: mp2

mp2: $(OBJECTS)
	$(CC) $(CFLAGS) -o mp2 $(OBJECTS) -lm


clean:
	rm -f *.o mp2 *~ 

compilar:
	tar cfz mp2.tgz Makefile *.c *.h
