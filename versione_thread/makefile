# Regola principale per la creazione dell'eseguibile
frogger: main.o menu.o crocodile.o frog.o game.o sprite.o colors.o map.o thread.o score.o music.o
	gcc -w  -g main.o menu.o crocodile.o frog.o game.o sprite.o colors.o map.o thread.o score.o music.o -o frogger -pthread -lncursesw -lmenuw -lSDL2 -lSDL2_mixer -fsanitize=address

# Regole per la compilazione dei file oggetto
main.o: main.c librerie.h struct.h thread.h
	gcc -w -g -c main.c -o main.o -pthread

menu.o: menu.c menu.h struct.h librerie.h
	gcc -w -g -c menu.c -o menu.o

crocodile.o: crocodile.c crocodile.h struct.h thread.h librerie.h
	gcc -w -g -c crocodile.c -o crocodile.o

frog.o: frog.c frog.h struct.h thread.h librerie.h
	gcc -w -g -c frog.c -o frog.o

game.o: game.c game.h music.h
	gcc -w -g -c game.c -o game.o

sprite.o: sprite.c sprite.h colors.h crocodile.h librerie.h
	gcc -w -g -c sprite.c -o sprite.o

colors.o: colors.c colors.h librerie.h
	gcc -w -g -c colors.c -o colors.o

map.o: map.c map.h struct.h sprite.h librerie.h
	gcc -w -g -c map.c -o map.o

thread.o: thread.c thread.h librerie.h
	gcc -w -g -c thread.c -o thread.o -pthread

score.o: score.c score.h librerie.h
	gcc -w -g -c score.c -o score.o

music.o: music.c music.h game.h librerie.h 
	gcc -w -g -c music.c -o music.o

# Regola per la pulizia dei file
clean:
	rm -f *.o frogger

