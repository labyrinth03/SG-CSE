main : main.o array.o rangearray.o
	g++ -o main main.o array.o rangearray.o

main.o : main.cpp
	g++ -c main.cpp
array.o : Array.h array.cpp
	g++ -c array.cpp
rangearray.o : RangeArray.h rangearray.cpp
	g++ -c rangearray.cpp

.PHONY : clean
clean :
	rm -rf *.o