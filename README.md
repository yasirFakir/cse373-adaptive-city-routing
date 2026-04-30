# CSE373-Adaptive-City-Routing

An efficient C++ based routing engine designed to find optimal paths in a city graph based on different modes (Shortest Distance, Fastest Time). This project features an interactive GUI built with **Dear ImGui** and is cross-platform compatible, including **WebAssembly** support.

## Features
- **Interactive GUI**: User-friendly interface for inputting nodes and selecting modes.
- **Dijkstra's Algorithm**: High-performance routing logic.
- **Multi-Mode**: Optimize for distance or travel time.
- **Cross-Platform**: Runs natively on Linux, Windows, and in the Browser.

---

## Build Instructions: Arch Linux

### 1. Install Dependencies
Arch Linux requires the `base-devel` group and specific libraries for SDL2 and OpenGL.
```bash
sudo pacman -Syu
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
./CSE373-Adaptive-City-Routing
```

---

## Build Instructions: Windows

### 1. Prerequisites
- **Visual Studio 2022**: Install the "Desktop development with C++" workload.
- **CMake**: Download and install from [cmake.org](https://cmake.org/download/).
- **SDL2**:
  - Download the `SDL2-devel-2.x.x-VC.zip` from the [SDL GitHub releases](https://github.com/libsdl-org/SDL/releases).
  - Extract it to a known location (e.g., `C:/Libraries/SDL2`).

### 2. Build via Command Prompt (Developer PowerShell/CMD)
```powershell
mkdir build
cd build
# Tell CMake where to find SDL2
cmake .. -DSDL2_DIR="C:/path/to/SDL2/cmake"
cmake --build . --config Release
```

### 3. Build via Visual Studio UI
1. Open Visual Studio.
2. Select "Open a local folder" and choose the project root.
3. Visual Studio will automatically detect `CMakeLists.txt`.
4. Right-click `CMakeLists.txt` -> "Build".
5. Press the "Start" (Green Arrow) button to run.

---

## Build Instructions: WebAssembly (Wasm)

Requires the [Emscripten SDK (emsdk)](https://emscripten.org/docs/getting_started/downloads.html).

```bash
mkdir build_wasm
cd build_wasm
emcmake cmake ..
make
cp CSE373-Adaptive-City-Routing.* ../web/
```
Then, serve the `web/` directory using any local server (e.g., `python -m http.server`).

---

## Project Structure
- `src/`: Core C++ logic and GUI implementation.
- `assets/`: Graph data (`city_map.txt`) and project PDF.
- `web/`: WebAssembly entry point and hosting files.
- `docs/`: Technical documentation and algorithm details.

## License
Educational project for the CSE373 course.
