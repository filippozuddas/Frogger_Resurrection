# Il target 'all' costruisce sia il server che il client
all: frogger_server frogger_client

# Oggetti necessari per il SERVER
SERVER_OBJS = main.o game.o menu.o crocodile.o frog.o sprite.o colors.o map.o communication.o music.o socket.o

# Oggetti necessari per il CLIENT
CLIENT_OBJS = client.o socket.o

# Regola per creare l'eseguibile del SERVER
frogger_server: $(SERVER_OBJS)
	gcc -w -g -o frogger_server $(SERVER_OBJS) -lncursesw -lmenuw -lSDL2 -lSDL2_mixer

# Regola per creare l'eseguibile del CLIENT
frogger_client: $(CLIENT_OBJS)
	gcc -w -g -o frogger_client $(CLIENT_OBJS) -lncursesw

# --- Regole di compilazione per i file .o ---

# --- SERVER ---
main.o: main.c game.h menu.h
	gcc -w -c main.c -o main.o
game.o: game.c game.h socket.h
	gcc -w -c game.c -o game.o
frog.o: frog.c frog.h
	gcc -w -c frog.c -o frog.o
menu.o: menu.c menu.h
	gcc -w -c menu.c -o menu.o
crocodile.o: crocodile.c crocodile.h
	gcc -w -c crocodile.c -o crocodile.o
sprite.o: sprite.c sprite.h
	gcc -w -c sprite.c -o sprite.o
colors.o: colors.c colors.h
	gcc -w -c colors.c -o colors.o
map.o: map.c map.h
	gcc -w -c map.c -o map.o
communication.o: communication.c communication.h
	gcc -w -c communication.c -o communication.o
music.o: music.c music.h
	gcc -w -c music.c -o music.o

# --- CLIENT ---
client.o: client.c socket.h struct.h
	gcc -w -c client.c -o client.o

# --- CONDIVISI ---
socket.o: socket.c socket.h struct.h
	gcc -w -c socket.c -o socket.o

# Pulizia
clean:
	rm -f *.o frogger_server frogger_client