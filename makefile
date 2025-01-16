frogger: main.o crocodile.o frog.o game.o sprite.o colors.o 
	gcc -o frogger main.o crocodile.o frog.o game.o sprite.o colors.o -lncursesw
main.o: main.c game.h
	gcc -c main.c -o main.o
game.o: game.c game.h crocodile.h colors.h struct.h
	gcc -c game.c -o game.o
crocodile.o: crocodile.c crocodile.h struct.h
	gcc -c crocodile.c -o crocodile.o 
frog.o: frog.c frog.h struct.h
	gcc -c frog.c -o frog.o
sprite.o: sprite.c sprite.h colors.h crocodile.h
	gcc -c sprite.c -o sprite.o
colors.o: colors.c colors.h	
	gcc -c colors.c -o colors.o
clean:
	rm -f *.o frogger