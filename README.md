# Dijkstra Algorithm - Adaptive City Routing

An efficient C++ based routing engine designed to find optimal paths in a city graph based on different modes (Shortest Distance, Fastest Time, Cheapest Route). This project features an interactive GUI built with **Dear ImGui** and is cross-platform compatible, including **WebAssembly** support.

## Features
- **Interactive GUI**: User-friendly interface for inputting nodes and selecting modes.
- **Dijkstra's Algorithm**: High-performance routing logic with step-by-step relaxation tables.
- **Multi-Mode**: Optimize for distance, travel time, or cost.
- **Cross-Platform**: Runs natively on Linux and in the Browser via WebAssembly.

---

## Live Demo
The project is hosted live on GitHub Pages:
**[View Live Program](https://yasirFakir.github.io/dijkstra-algorithm/)**

---

## Build Instructions: Linux

### 1. Install Dependencies
Ensure you have `cmake`, `sdl2`, and OpenGL development headers installed.
```bash
# Example for Arch Linux
sudo pacman -S base-devel cmake sdl2 mesa
```

### 2. Build the Project
```bash
mkdir build
cd build
cmake ..
make
```

### 3. Run
```bash
./dijkstra-algorithm
```

---

## Build Instructions: WebAssembly (Wasm)

Requires the [Emscripten SDK (emsdk)](https://emscripten.org/docs/getting_started/downloads.html).

```bash
mkdir build_wasm
cd build_wasm
emcmake cmake .. -DEMSCRIPTEN=ON
make
```

---

## Project Structure
- `src/`: Core C++ logic and GUI implementation.
- `assets/`: Graph data and map files.
- `web/`: WebAssembly entry point and hosting files.
- `docs/`: Technical documentation and algorithm details.
