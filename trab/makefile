CC = gcc
CFLAGS = -Wall -lpthread -lm
OBJS = trapezio.o testfunctions.o

trapezio.exe: objects
	$(CC) $(CFLAGS) $(OBJS) -o $@

objects: $(OBJS)

trapezio.o: trapezio.c
	$(CC) -c $(CFLAGS) trapezio.c

testfunctions.o: testfunctions.c
	$(CC) -c $(CFLAGS) testfunctions.c

run: trapezio.exe
	./trapezio.exe

clean:
	rm $(wildcard *.o) $(wildcard *.exe)
