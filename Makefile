CXX = gcc

all: main

main: lines_struct.c lines_struct.h
	$(CXX) main.c -o main 

clean:
	rm -rf main.o