CC = gcc
CFLAGS = -Wall -lpthread

sumnchange.exe: sumnchange.o
	$(CC) $(CFLAGS) sumnchange.o -o sumnchange.exe

sumnchange.o: EduardaFerreira-CompConc-lab06-sumnchange.c
	$(CC) $(CFLAGS) -c EduardaFerreira-CompConc-lab06-sumnchange.c -o sumnchange.o

clean:
	rm $(wildcard *.o) $(wildcard *.exe)

test: sumnchange.exe
	./sumnchange.exe 4