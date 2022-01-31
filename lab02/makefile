CC = gcc
CFLAGS = -Wall -lpthread

mat_mul.exe: mat_mul.o
	$(CC) $(CFLAGS) mat_mul.o -o mat_mul.exe

mat_mul.o: EduardaFerreira-CompConc-lab02-mat_mul.c
	$(CC) $(CFLAGS) -c EduardaFerreira-CompConc-lab02-mat_mul.c -o mat_mul.o

run: mat_mul.exe
	./mat_mul.exe
# for testing, before adding command line args

clean:
	rm $(wildcard *.o) $(wildcard *.exe)
