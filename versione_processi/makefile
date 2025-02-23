#uso -lncursesw per utilizzare i wide character
frogger: main.o menu.o crocodile.o frog.o game.o sprite.o colors.o map.o communication.o music.o
	gcc -g -o  frogger main.o menu.o crocodile.o frog.o game.o sprite.o colors.o map.o communication.o music.o -lncursesw -lmenuw -lSDL2 -lSDL2_mixer

main.o: main.c game.h menu.h
	gcc -c main.c -o main.o

menu.o: menu.c menu.h struct.h
	gcc -c menu.c -o menu.o

game.o: game.c game.h crocodile.h colors.h struct.h map.h sprite.h communication.h music.h
	gcc -c game.c -o game.o

crocodile.o: crocodile.c crocodile.h struct.h communication.h
	gcc -c crocodile.c -o crocodile.o

frog.o: frog.c frog.h struct.h communication.h
	gcc -c frog.c -o frog.o

sprite.o: sprite.c sprite.h colors.h crocodile.h
	gcc -c sprite.c -o sprite.o

colors.o: colors.c colors.h
	gcc -c colors.c -o colors.o

map.o: map.c map.h struct.h sprite.h
	gcc -c map.c -o map.o

communication.o : communication.c communication.h game.h
	gcc -c communication.c -o communication.o

music.o: music.c music.h game.h
	gcc -c music.c -o music.o

clean:
	rm -f *.o frogger
