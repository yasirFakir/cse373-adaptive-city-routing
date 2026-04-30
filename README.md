# Dijkstra Algorithm - Adaptive City Routing System
### CSE 373: Design and Analysis of Algorithms Project

An interactive, multi-constraint routing system that utilizes Dijkstra's algorithm to find optimal paths based on distance, time, and cost in a dynamic urban environment.

## 🚀 Features
- **Multi-Constraint Optimization:** Switch between Shortest Distance, Fastest Time, and Cheapest Route.
- **Dynamic Filtering:** Toggle visibility and accessibility of specific nodes and transport modes (Road, Metro, Bus).
- **Interactive Graph:** Add/delete nodes, drag to move nodes, and create edges in real-time.
- **Cross-Platform:** Runs natively on Linux and via WebAssembly in modern browsers.
- **Map Persistence:** Upload custom map files from your PC or download your created maps.

## 📂 Project Structure
```text
.
├── assets/                    # Map data and project documentation
│   ├── circular_map.txt
│   ├── city_map.txt           # Default map
│   ├── grid_map.txt
│   └── test.txt
├── src/                       # C++ Source Code
│   ├── main.cpp               # GUI and App Logic (ImGui + SDL2)
│   ├── graph.cpp/h            # Graph data structure implementation
│   └── algorithms.cpp         # Dijkstra's Algorithm implementation
├── web/                       # Web-specific files
│   └── index.html             # Emscripten template
├── CMakeLists.txt             # Build configuration
└── README.md                  # Project documentation
```

## 🛠️ Build & Run

### Native (Linux)
**Dependencies:** `SDL2`, `OpenGL`, `Zenity` (for file dialogs).
```bash
mkdir build && cd build
cmake ..
make
./dijkstra-algorithm
```

### Web (Emscripten)
```bash
emcmake cmake .. -DEMSCRIPTEN=ON
make
```

## 🧠 Algorithmic Implementation
The core engine implements a modified Dijkstra's algorithm using a `std::priority_queue` for efficiency ($O(E \log V)$). It supports:
- **Avoidance Sets:** Skip nodes or transport types during the relaxation step.
- **Trace Visualization:** A step-by-step relaxation table showing how distances are updated across the graph.

## 👥 Contributors
- **Yasir** - Lead Developer & Algorithm Design
