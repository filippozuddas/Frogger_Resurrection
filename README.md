# Frogger Resurrection

Progetto finale del corso di Sistemi Operativi e Programmazione di Rete.

## Descrizione

Frogger Resurrection è una rivisitazione del classico gioco Frogger, implementato in C. Il progetto offre due versioni del server di gioco:

- **versione_processi**: server basato su processi.
- **versione_thread**: server basato su thread e buffer condiviso.

Entrambe le versioni utilizzano ncurses per l'interfaccia utente e SDL2_mixer per la riproduzione audio.

## Requisiti

- GNU Make e gcc
- ncursesw (libncursesw5-dev) e libmenuw
- SDL2 (libsdl2-dev) e SDL2_mixer (libsdl2-mixer-dev)
- pthreads (per versione_thread)
- pkg-config

## Struttura del repository

```
music/
versione_processi/
versione_thread/
README.md
.gitignore
```

- `music/` contiene i file WAV delle musiche di gioco.
- `versione_processi/` implementazione server-client basata su processi.
- `versione_thread/` implementazione server-client basata su thread.

## Compilazione

### Versione basata su processi

```bash
cd versione_processi
make
```

Genera due eseguibili:

- `frogger_server`: server di gioco.
- `frogger_client`: client di gioco.

### Versione basata su thread

```bash
cd versione_thread
make
```

Genera due eseguibili:

- `frogger_server`: server di gioco.
- `frogger_client`: client di gioco.

## Esecuzione

1. In un terminale, avvia il server:

   ```bash
   ./frogger_server
   ```

2. In un altro terminale, avvia il client:

   ```bash
   ./frogger_client
   ```

Puoi alternare tra le directory `versione_processi` e `versione_thread` a seconda della versione desiderata.

**Nota**: le musiche vengono lette dalla cartella `music/` relativa al percorso di esecuzione del server. Assicurati di partire dal livello superiore (`../music/...`) quando avvii il server.

## Pulizia

Per rimuovere file oggetto e binari:

```bash
make clean
```

all'interno della cartella `versione_processi` o `versione_thread`.
