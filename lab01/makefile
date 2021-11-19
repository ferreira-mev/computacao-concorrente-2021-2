CC = gcc
CFLAGS = -Wall -lpthread

a5-square-elements.exe: a5-square-elements.o
	cc a5-square-elements.o -o a5-square-elements.exe

a5-square-elements.o: a5-square-elements.c
	cc -c a5-square-elements.c -o a5-square-elements.o

run: a5-square-elements.exe
	./a5-square-elements.exe

clean:
	rm $(wildcard *.o) $(wildcard *.exe)

test_vector:
	@for i in {0..9999}; do echo "$$i" >> test_vector.txt; done