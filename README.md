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

## 🎮 User Guide (Interaction)
### Graph Controls
- **Move Nodes:** Click and hold the **Left Mouse Button** on a node to drag it to a new position.
- **Create Edges:** Click and hold the **Right Mouse Button** on a node, then drag the connection to another node to create a new road/metro/bus line.
- **Select Items:** Single **Left Click** on a node or edge to select it for deletion.
- **Delete Items:** Press the **Delete** or **Backspace** key to remove the currently selected node or edge.
- **Edit Edge Properties:** **Double Click** on any edge to open a popup where you can modify its distance, time, cost, and transport type.
- **Add New Nodes:** Use the **ADD NODE** button in the sidebar to spawn a new node on the canvas.

### Pathfinding
1. Enter the **Start** and **End** node labels (e.g., A, B, C) in the input fields.
2. Click **COMPUTE** to visualize the optimal paths.
3. Use the **NODE FILTERS** and **TRANSPORT FILTERS** checklists to dynamically block specific routes.

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
