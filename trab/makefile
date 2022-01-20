CC = gcc
CFLAGS = -Wall -lpthread -lm

trapezio.exe: trapezio.o
	cc $(CFLAGS) trapezio.o -o trapezio.exe

trapezio.o: EduardaFerreira-CompConc-trab-trapezio.c
	cc -c $(CFLAGS) EduardaFerreira-CompConc-trab-trapezio.c -o trapezio.o

run: trapezio.exe
	./trapezio.exe

clean:
	rm $(wildcard *.o) $(wildcard *.exe) $(wildcard *.txt)