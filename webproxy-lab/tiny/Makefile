CC = gcc
CFLAGS = -O0 -Wall -I . -g
# CFLAGS = -O2 -Wall -I . -g

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: tiny cgi

tiny: tiny.c csapp.o
	$(CC) $(CFLAGS) -o tiny tiny.c csapp.o jobqueue.o accepthandler.o recvsocket.o requesthandler.o sendsocket.o $(LIB)

csapp.o: csapp.c
	$(CC) $(CFLAGS) -c csapp.c

cgi:
	(cd cgi-bin; make)
build:
	$(CC) $(CFLAGS) -c jobqueue.c accepthandler.c recvsocket.c requesthandler.c sendsocket.c tiny.c csapp.c cgi-bin/adder.c
clean:
	rm -f *.o tiny *~
	(cd cgi-bin; make clean)

