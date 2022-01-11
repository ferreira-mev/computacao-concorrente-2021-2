CC = gcc
CFLAGS = -Wall -lpthread

sumnchange.exe: sumnchange.o
	cc sumnchange.o -o sumnchange.exe

sumnchange.o: EduardaFerreira-CompConc-lab06-sumnchange.c
	cc -c EduardaFerreira-CompConc-lab06-sumnchange.c -o sumnchange.o

clean:
	rm $(wildcard *.o) $(wildcard *.exe)

test: sumnchange.exe
	./sumnchange.exe 4