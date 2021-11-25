CC = gcc
CFLAGS = -Wall -lpthread

mat_mul.exe: mat_mul.o
	cc mat_mul.o -o mat_mul.exe

mat_mul.o: EduardaFerreira-CompConc-lab02-mat_mul.c
	cc -c EduardaFerreira-CompConc-lab02-mat_mul.c -o mat_mul.o

clean:
	rm $(wildcard *.o) $(wildcard *.exe)
