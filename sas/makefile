all: frogger
frogger: main.o crocodile.o 
	gcc -o frogger main.o crocodile.o -lncurses
main.o: main.c 
	gcc -c main.c -o main.o
crocodile.o: crocodile.c
	gcc -c crocodile.c -o crocodile.o
clean:
	rm -f *.o frogger