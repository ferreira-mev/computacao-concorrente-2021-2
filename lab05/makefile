CC = gcc
CFLAGS = -Wall -lpthread

sentence_sandwich.exe: sentence_sandwich.o
	$(CC) $(CFLAGS) sentence_sandwich.o -o sentence_sandwich.exe

sentence_sandwich.o: EduardaFerreira-CompConc-lab05-sentence_sandwich.c
	$(CC) $(CFLAGS) -c EduardaFerreira-CompConc-lab05-sentence_sandwich.c -o sentence_sandwich.o

clean:
	rm $(wildcard *.o) $(wildcard *.exe)

run: sentence_sandwich.exe
	./sentence_sandwich.exe