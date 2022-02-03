CC = gcc
CFLAGS = -Wall -lpthread
OBJS = sem_prod_cons.o helperfunctions.o logging.o

sem_prod_cons.exe: objects
	$(CC) $(CFLAGS) $(OBJS) -o $@

objects: $(OBJS)

sem_prod_cons.o: sem_prod_cons.c
	$(CC) $(CFLAGS) -c $^

helperfunctions.o: helperfunctions.c
	$(CC) $(CFLAGS) -c $^

logging.o: logging.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm $(wildcard *.o) $(wildcard *.exe)

clearlogs:
	rm logs/*

run: sem_prod_cons.exe  # fixando params só p/ testar
	./sem_prod_cons.exe 1 1

test: sem_prod_cons.exe  # todos os casos de teste pedidos
	./sem_prod_cons.exe 1 1; ./sem_prod_cons.exe 5 1; ./sem_prod_cons.exe 1 4; ./sem_prod_cons.exe 6 7