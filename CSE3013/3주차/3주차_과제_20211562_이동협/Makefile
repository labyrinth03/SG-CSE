main : main.o str.o
	g++ -o main main.o str.o

main.o : main.cpp
	g++ -c main.cpp
str.o : Str.h str.cpp
	g++ -c str.cpp

.PHONY : clean
clean :
	rm -rf *.o