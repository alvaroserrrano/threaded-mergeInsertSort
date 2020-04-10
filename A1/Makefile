CC = gcc
CFLAGS = -g -Wall -Werror -pedantic
CFLAGS+= -pthread
# CFLAGS = -std=c11

LDLIBS += -lpthread
LDLIBS += -lrt

a1: a1.o array.o error.o random.o linked.o

a1.o: a1.c array.h linked.h

array.o: array.c apue.h array.h random.h

linked.o: linked.c apue.h linked.h random.h array.h

error.o: error.c apue.h

random.o: random.c random.h apue.h

clean:
	rm -f *.o a1 *.csv

run:
	./source
