# 🐸 Frogger Resurrection

A terminal-based reimagining of the classic Frogger arcade game, built in C as a final project for the **Operating Systems and Network Programming** course.

The game runs as a **client-server application** over TCP sockets: the server handles all game logic and rendering, while the client captures player input and forwards it to the server in real time.

---

## 📁 Project Structure

```
Frogger_Resurrection/
├── music/                  # WAV audio files for menus, levels, and game events
├── versione_thread/        # Thread-based implementation (POSIX threads)
└── versione_processi/      # Process-based implementation (fork/IPC)
```

Both versions share the same gameplay and architecture but differ in how concurrency is managed:

| Version | Concurrency model |
|---|---|
| `versione_thread` | POSIX threads (`pthread`) |
| `versione_processi` | Processes with IPC |

---

## 🎮 Gameplay

- Guide your frog from the bottom of the screen to the **dens** at the top.
- Cross the **river** by jumping on moving **crocodiles** — falling into the water costs a life.
- Avoid **projectiles** fired by crocodiles.
- Throw **grenades** (Spacebar) to destroy incoming projectiles.
- Fill all 5 dens to win the round.
- Each run has a countdown timer; running out of time costs a life.
- The game tracks your **score** and saves the top 10 results to a file.

### Difficulty levels

| Level | Timer |
|---|---|
| Easy | 60 seconds |
| Medium | 45 seconds |
| Hard | 30 seconds |

### Controls

| Key | Action |
|---|---|
| `↑` / `W` | Move up |
| `↓` / `S` | Move down |
| `←` / `A` | Move left |
| `→` / `D` | Move right |
| `Space` | Throw grenade (left and right) |

---

## 🛠️ Dependencies

- **ncursesw** — terminal UI rendering
- **SDL2** — audio engine
- **SDL2_mixer** — audio mixing

On Debian/Ubuntu:

```bash
sudo apt install libncurses5-dev libncursesw5-dev libsdl2-dev libsdl2-mixer-dev
```

The `versione_thread` Makefile includes an interactive dependency checker that will prompt you to install missing packages automatically.

---

## 🔨 Build

Navigate into either version directory and run `make`:

```bash
cd versione_thread   # or versione_processi
make
```

This produces two executables:

- `frogger_server` — game server (renders the game and runs all logic)
- `frogger_client` — player client (captures and sends keyboard input)

To clean build artifacts:

```bash
make clean
```

---

## 🚀 Running the Game

The server and client must be started in **two separate terminals** on the same machine.

**Terminal 1 — start the server:**
```bash
./frogger_server
```

The server will wait for a client connection on port **8088**.

**Terminal 2 — start the client:**
```bash
./frogger_client
```

Once the client connects, the welcome animation plays and the main menu appears in the **server terminal**.

> **Note:** the game window requires a terminal of at least **200×70** characters. Use a maximised terminal or reduce the font size if the screen appears corrupted.
---

## 🏗️ Architecture

```
┌─────────────────────────────────┐        TCP (port 8088)       ┌──────────────────┐
│           SERVER                │ ◄──────────────────────────► │     CLIENT       │
│                                 │                              │                  │
│  • Game logic (frog, crocs,     │   ← player position/actions  │ • Reads keyboard │
│    grenades, projectiles, dens) │   → frog state updates       │ • Sends inputs   │
│  • ncurses rendering            │                              │                  │
│  • Audio (SDL2)                 │                              │                  │
│  • Score management             │                              │                  │
└─────────────────────────────────┘                              └──────────────────┘
```

Each game entity (frog, crocodile, grenade, projectile) runs in its own **thread** (or process in `versione_processi`). All entities communicate through a shared buffer protected by mutexes.

---

## 📊 Score System

- **+100 points** each time the frog reaches a den.
- Scores are saved to `scores.dat` after each game session.
- The in-game **High Scores** menu displays the top 10 results with date and difficulty.

---

## 🎵 Music

Custom WAV tracks play during different game states:

| File | When played |
|---|---|
| `WELCOME.wav` | Welcome animation |
| `MENU1.wav` | Main menu |
| `LIVELLO1(130BPM).wav` | Easy difficulty |
| `LIVELLO2(145BPM).wav` | Medium difficulty |
| `LIVELLO3(170BPM).wav` | Hard difficulty |
| `GAMEOVER.wav` | Game over screen |
| `YOUWIN.wav` | Victory screen |
