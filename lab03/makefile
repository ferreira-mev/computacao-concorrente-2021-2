CC = gcc
CFLAGS = -Wall -lpthread

count_elems.exe: count_elems.o
	$(CC) $(CFLAGS) count_elems.o -o count_elems.exe

count_elems.o: EduardaFerreira-CompConc-lab03-count_elems.c
	$(CC) $(CFLAGS) -c EduardaFerreira-CompConc-lab03-count_elems.c -o count_elems.o

clean:
	rm $(wildcard *.o) $(wildcard *.exe)

test: count_elems.exe
	./count_elems.exe -1 1