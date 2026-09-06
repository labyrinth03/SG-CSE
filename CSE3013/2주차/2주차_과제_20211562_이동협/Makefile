cc=gcc
cflags = -W -g
target = main

objects = main.o calculate.o array.o

$(target) : $(objects)
		$(cc) $(cflags) -o $(target) $(objects)

%.o : %.c
		$(cc) $(cflags) -c -o $@ $<

main.o calculate.o array.o : header.h

.PHONY : clean
clean :
		rm $(target) $(objects)

