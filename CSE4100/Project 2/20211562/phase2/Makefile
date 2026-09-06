CC = gcc
CFLAGS = -Og
LDLIBS = -lpthread

PROGS = myshell

all: $(PROGS)

myshell: myshell.o csapp.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

myshell.o: myshell.c
	$(CC) $(CFLAGS) -c -o $@ $<

csapp.o: csapp.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *~ $(PROGS) *.o