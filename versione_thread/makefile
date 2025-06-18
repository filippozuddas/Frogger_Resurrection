# Controllo delle dipendenze
check_deps:
	@command -v sdl2-config >/dev/null 2>&1 || { \
		echo "SDL2 non trovato. Vuoi installarlo? [S/n]"; \
		read ans; \
		if [ "$$ans" = "S" ] || [ "$$ans" = "s" ] || [ -z "$$ans" ]; then \
			sudo apt-get update && sudo apt-get install libsdl2-dev; \
		else \
			echo "SDL2 è richiesto per la compilazione!"; exit 1; \
		fi \
	}
	@pkg-config --exists SDL2_mixer || { \
		echo "SDL2_mixer non trovato. Vuoi installarlo? [S/n]"; \
		read ans; \
		if [ "$$ans" = "S" ] || [ "$$ans" = "s" ] || [ -z "$$ans" ]; then \
			sudo apt-get update && sudo apt-get install libsdl2-mixer-dev; \
		else \
			echo "SDL2_mixer è richiesto per la compilazione!"; exit 1; \
		fi \
	}
	@ldconfig -p | grep libncursesw >/dev/null 2>&1 || { \
		echo "ncursesw non trovato. Vuoi installarlo? [S/n]"; \
		read ans; \
		if [ "$$ans" = "S" ] || [ "$$ans" = "s" ] || [ -z "$$ans" ]; then \
			sudo apt-get update && sudo apt install libncurses5-dev libncursesw5-dev; \
		else \
			echo "ncursesw è richiesto per la compilazione!"; exit 1; \
		fi \
	}

# Regola principale per creare entrambi gli eseguibili
all: check_deps frogger_server frogger_client

# Regola per la creazione dell'eseguibile frogger (server)
frogger_server: main.o menu.o crocodile.o frog.o game.o sprite.o colors.o map.o thread.o score.o music.o socket.o
	gcc -w -g $^ -o frogger_server -pthread -lncursesw -lmenuw -lSDL2 -lSDL2_mixer 

# Regola per la creazione dell'eseguibile client
frogger_client: client.o socket.o
	gcc -w -g $^ -o frogger_client -pthread -lncursesw 

# Regole per la compilazione dei file oggetto
main.o: main.c librerie.h
	gcc -w -g -c main.c -o main.o -pthread

menu.o: menu.c menu.h librerie.h
	gcc -w -g -c menu.c -o menu.o

crocodile.o: crocodile.c crocodile.h librerie.h
	gcc -w -g -c crocodile.c -o crocodile.o

frog.o: frog.c frog.h librerie.h
	gcc -w -g -c frog.c -o frog.o

game.o: game.c game.h librerie.h
	gcc -w -g -c game.c -o game.o

sprite.o: sprite.c sprite.h librerie.h
	gcc -w -g -c sprite.c -o sprite.o

colors.o: colors.c colors.h librerie.h
	gcc -w -g -c colors.c -o colors.o

map.o: map.c map.h librerie.h
	gcc -w -g -c map.c -o map.o

thread.o: thread.c thread.h librerie.h
	gcc -w -g -c thread.c -o thread.o -pthread

score.o: score.c score.h librerie.h
	gcc -w -g -c score.c -o score.o

music.o: music.c music.h librerie.h
	gcc -w -g -c music.c -o music.o

# Nuove regole per i file socket e client
socket.o: socket.c socket.h struct.h
	gcc -w -g -c socket.c -o socket.o

client.o: client.c socket.h
	gcc -w -g -c client.c -o client.o

# Regola per la pulizia dei file
clean:
	rm -f *.o frogger_server frogger_client