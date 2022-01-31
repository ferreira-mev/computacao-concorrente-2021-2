CC = gcc
CFLAGS = -Wall -lpthread

sentence_sandwich_redux.exe: sentence_sandwich_redux.o
	$(CC) $(CFLAGS) sentence_sandwich_redux.o -o sentence_sandwich_redux.exe

sentence_sandwich_redux.o: EduardaFerreira-CompConc-lab09-sentence_sandwich_redux.c
	$(CC) $(CFLAGS) -c EduardaFerreira-CompConc-lab09-sentence_sandwich_redux.c -o sentence_sandwich_redux.o

clean:
	rm $(wildcard *.o) $(wildcard *.exe)

run: sentence_sandwich_redux.exe
	./sentence_sandwich_redux.exe