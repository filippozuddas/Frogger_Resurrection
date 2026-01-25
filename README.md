# 🐸 Frogger Resurrection

**Progetto finale del corso di Sistemi Operativi e Programmazione di Rete**

Un remake del classico gioco arcade Frogger, sviluppato in C utilizzando la libreria ncurses per l'interfaccia grafica testuale. Il progetto include due implementazioni parallele: una basata su **processi** e una basata su **thread**.

---

## 📖 Descrizione

Frogger Resurrection è un gioco arcade in cui il giocatore controlla una rana che deve attraversare un fiume pieno di coccodrilli per raggiungere le tane sicure dall'altra parte. Il gioco include:

- 🎮 **Gameplay classico** con controlli intuitivi
- 🐊 **Coccodrilli** che si muovono sul fiume come piattaforme
- 💣 **Sistema di granate** per difendersi
- 🎵 **Musica ed effetti sonori** per ogni livello
- 📊 **Sistema di punteggio** con salvataggio persistente
- 🔧 **Tre livelli di difficoltà** selezionabili
- 🌐 **Architettura client-server** con comunicazione via socket

---

## ✨ Caratteristiche

- **Due versioni del gioco:**
  - `versione_processi`: Utilizza processi separati con comunicazione tramite pipe
  - `versione_thread`: Utilizza thread con memoria condivisa
  
- **Interfaccia grafica TUI** basata su ncurses con supporto Unicode
- **Sistema audio** con SDL2 per musiche di sottofondo
- **Multiplayer locale** tramite architettura client-server su socket TCP
- **Menu interattivo** con animazioni di benvenuto
- **Gestione punteggi** con classifica salvata su file

---

## 📋 Requisiti

### Dipendenze di sistema

- **GCC** (compilatore C)
- **Make** (build system)
- **ncursesw** (libreria per interfaccia TUI con supporto wide characters)
- **SDL2** (libreria multimediale)
- **SDL2_mixer** (estensione SDL2 per l'audio)

### Installazione dipendenze (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential libncurses5-dev libncursesw5-dev libsdl2-dev libsdl2-mixer-dev
```

---

## 🚀 Compilazione

### Versione con Processi

```bash
cd versione_processi
make all
```

### Versione con Thread

```bash
cd versione_thread
make all
```

Questo genererà due eseguibili:
- `frogger_server` - Il server di gioco principale
- `frogger_client` - Il client per il secondo giocatore

### Pulizia

```bash
make clean
```

---

## 🎮 Esecuzione

### Avvio del gioco

1. **Avvia prima il server** (finestra principale di gioco):
   ```bash
   ./frogger_server
   ```
   Il server si avvierà e attenderà la connessione del client sulla porta **8088**.

2. **Avvia poi il client** (in un altro terminale):
   ```bash
   ./frogger_client
   ```

3. Una volta connesso il client, il gioco inizierà con l'animazione di benvenuto.

### Controlli

| Tasto | Azione |
|-------|--------|
| `↑` / `W` | Muovi su |
| `↓` / `S` | Muovi giù |
| `←` / `A` | Muovi a sinistra |
| `→` / `D` | Muovi a destra |
| `SPAZIO` | Lancia granata |
| `Q` | Esci dal gioco |

---

## 📁 Struttura del Progetto

```
Frogger_Resurrection/
├── README.md
├── music/                          # File audio del gioco
│   ├── GAMEOVER.wav
│   ├── LIVELLO1(130BPM).wav
│   ├── LIVELLO2(145BPM).wav
│   ├── LIVELLO3(170BPM).wav
│   ├── MENU1.wav
│   ├── WELCOME.wav
│   └── YOUWIN.wav
├── versione_processi/              # Implementazione con processi
│   ├── main.c                      # Entry point del server
│   ├── game.c/h                    # Logica principale del gioco
│   ├── frog.c/h                    # Gestione della rana
│   ├── crocodile.c/h               # Gestione dei coccodrilli
│   ├── menu.c/h                    # Sistema di menu
│   ├── map.c/h                     # Rendering della mappa
│   ├── sprite.c/h                  # Gestione degli sprite
│   ├── colors.c/h                  # Gestione dei colori
│   ├── music.c/h                   # Sistema audio
│   ├── socket.c/h                  # Comunicazione socket
│   ├── communication.c/h           # Comunicazione tra processi
│   ├── client.c                    # Client per il secondo giocatore
│   ├── struct.h                    # Definizioni delle strutture dati
│   ├── makefile                    # Script di compilazione
│   └── scores.dat                  # File dei punteggi
└── versione_thread/                # Implementazione con thread
    ├── main.c                      # Entry point del server
    ├── game.c/h                    # Logica principale del gioco
    ├── frog.c/h                    # Gestione della rana
    ├── crocodile.c/h               # Gestione dei coccodrilli
    ├── menu.c/h                    # Sistema di menu
    ├── map.c/h                     # Rendering della mappa
    ├── sprite.c/h                  # Gestione degli sprite
    ├── colors.c/h                  # Gestione dei colori
    ├── music.c/h                   # Sistema audio
    ├── socket.c/h                  # Comunicazione socket
    ├── thread.c/h                  # Gestione dei thread
    ├── score.c/h                   # Gestione dei punteggi
    ├── client.c                    # Client per il secondo giocatore
    ├── struct.h                    # Definizioni delle strutture dati
    ├── librerie.h                  # Header con tutte le librerie
    ├── globali.h                   # Variabili globali
    └── makefile                    # Script di compilazione
```

---

## 🏗️ Architettura

### Versione Processi

```
┌─────────────────┐     PIPE      ┌─────────────────┐
│  Processo Main  │◄────────────►│ Processo Frog   │
│   (Server)      │              └─────────────────┘
│                 │     PIPE      ┌─────────────────┐
│                 │◄────────────►│ Processi Croc   │
│                 │              │   (N_CROC)      │
└────────┬────────┘              └─────────────────┘
         │ SOCKET
         ▼
┌─────────────────┐
│     Client      │
└─────────────────┘
```

- Ogni coccodrillo è un **processo separato** che comunica con il main tramite **pipe**
- La rana è gestita da un **processo dedicato**
- Il client si connette via **socket TCP** sulla porta 8088

### Versione Thread

```
┌─────────────────────────────────────┐
│           Processo Main             │
│  ┌──────────┐  ┌──────────────────┐ │
│  │ Thread   │  │ Thread Crocodile │ │
│  │  Frog    │  │    (N_CROC)      │ │
│  └──────────┘  └──────────────────┘ │
│      ▲                   ▲          │
│      └─── Memoria Condivisa ───┘    │
└────────────────┬────────────────────┘
                 │ SOCKET
                 ▼
         ┌─────────────────┐
         │     Client      │
         └─────────────────┘
```

- Tutti gli elementi di gioco sono gestiti tramite **thread** nello stesso processo
- La sincronizzazione avviene tramite **mutex** e **memoria condivisa**
- Migliori prestazioni grazie al minor overhead di comunicazione

---

## 📊 Meccaniche di Gioco

- **Obiettivo**: Guida la rana attraverso il fiume saltando sui coccodrilli fino alle tane sicure
- **Vite**: Inizi con 3 vite, ne perdi una se cadi in acqua o vieni colpito
- **Punteggio**: Guadagni punti raggiungendo le tane; bonus per il tempo rimanente
- **Timer**: Ogni livello ha un tempo limite che diminuisce con la difficoltà
- **Difficoltà**: 
  - **Facile**: Coccodrilli lenti, tempo abbondante
  - **Medio**: Velocità e tempo bilanciati
  - **Difficile**: Coccodrilli veloci, tempo ridotto

---

## 🎵 Audio

Il gioco include tracce audio originali:
- Musica di benvenuto
- Tema del menu principale  
- 3 tracce per i diversi livelli (130, 145, 170 BPM)
- Effetti per vittoria e game over

---

## 👨‍💻 Autore

Progetto sviluppato come parte del corso di **Sistemi Operativi e Programmazione di Rete**.

---

## 📄 Licenza

Questo progetto è stato sviluppato a scopo didattico.
