CC = gcc
CFLAGS = -Wall -lpthread -lm
OBJS = trapezoidal_rule.o testfunctions.o helperfunctions.o

trapezoidal_rule.exe: objects
	$(CC) $(CFLAGS) $(OBJS) -o $@

objects: $(OBJS)

trapezoidal_rule.o: trapezoidal_rule.c
	$(CC) -c $(CFLAGS) trapezoidal_rule.c

testfunctions.o: testfunctions.c
	$(CC) -c $(CFLAGS) testfunctions.c

helperfunctions.o: helperfunctions.c
	$(CC) -c $(CFLAGS) helperfunctions.c

run: trapezoidal_rule.exe
	./trapezoidal_rule.exe

clean:
	rm $(wildcard *.o) $(wildcard *.exe)
