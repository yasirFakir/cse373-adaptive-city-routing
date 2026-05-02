# Dijkstra Algorithm - Adaptive Multi-Constraint City Routing System
### CSE 373: Design and Analysis of Algorithms Project

An interactive, high-performance routing engine designed to visualize and solve complex pathfinding problems in urban environments. This system utilizes a modified **Dijkstra's Algorithm** to find optimal paths based on multiple constraints like distance, time, and cost, while providing real-time graph manipulation capabilities.

---

## 🚀 Key Features

### 🛠️ Dynamic Graph Manipulation
- **Real-time Editing:** Add nodes with double-click, move them via drag-and-drop, and create edges by dragging between nodes.
- **Node & Edge Management:** Select any element and press the **Delete** key to remove it. Update edge weights (distance, time, cost) and transport types via a double-click context menu.
- **Auto-Layout:** Instantly organize nodes using a circular layout for better visualization of complex networks.

### 🛣️ Multi-Constraint Pathfinding
- **Optimization Modes:** 
  - **Shortest Distance:** Minimizes the physical length of the route.
  - **Fastest Time:** Calculates the quickest path based on speed limits and traffic characteristics.
  - **Cheapest Route:** Optimizes for travel cost (tolls, fuel, or fare).
  - **Custom Weighted:** User-defined weights for distance, time, and cost.
- **Intelligent Filtering:** Dynamically block specific nodes (e.g., "Under Construction") or entire transport modes (e.g., "Avoid Metro") and see the path update instantly.

### 📊 Advanced Visualization
- **Path Highlighting:** Clear visual feedback of the calculated route with total metrics (Dist/Time/Cost).
- **Relaxation Table:** A detailed step-by-step breakdown of the algorithm's execution, showing node discovery and distance updates.
- **Responsive UI:** Dark/Light mode support with a clean, modern interface powered by Dear ImGui.

---

## 💻 Tech Stack
- **Language:** C++17
- **GUI Framework:** [Dear ImGui](https://github.com/ocornut/imgui)
- **Graphics & Windowing:** SDL2 & OpenGL 3.0
- **Build System:** CMake
- **Web Support:** Emscripten (WebAssembly)
- **Utilities:** Zenity (Native file dialogs on Linux)

---

## 📂 File Structure
```text
.
├── assets/                    # Map data, fonts, and documentation
│   ├── city_map.txt           # Standard metropolitan map
│   ├── circular_map.txt       # Example of symmetric layout
│   └── Roboto-Regular.ttf     # Project font
├── src/                       # Core C++ Logic
│   ├── main.cpp               # UI Orchestration & Event Loop
│   ├── graph.cpp / .h         # Graph Data Structure & File I/O
│   ├── algorithms.cpp         # Dijkstra's Algorithm Implementation
│   └── default_map.h          # Fallback map data
├── web/                       # Deployment assets for WebAssembly
└── CMakeLists.txt             # Cross-platform build configuration
```

---

## ⚙️ Map File Format
You can load custom maps using simple `.txt` files. Each line defines a directed edge:
`[Source_ID] [Destination_ID] [Distance] [Time] [Cost] [Transport_Type]`

**Example:**
```text
1 2 10.5 5 2.0 road
2 3 15.0 10 5.0 metro
```
*Note: Node labels (A, B, C...) are automatically mapped from their integer IDs (1=A, 2=B, etc.).*

---

## 🕹️ How to Use

Follow these steps to interact with the routing system and visualize Dijkstra's algorithm:

- **Select Node/Edge:** Left-click on any element to select it.
- **Add a New Node:** Double-click on any empty space on the map.
- **Move Nodes:** Left-click and drag a node to reposition it.
- **Create an Edge:** Right-click on a node and drag the cursor to another node to connect them.
- **Edit Edge Weights:** Double-click on an existing edge to open the editor and update its **Distance**, **Time**, or **Cost**.
- **Delete Elements:** Select a node or edge and press the **Delete** key.
- **Find Best Path:** 
  1. Set the **Start** and **End** node labels in the sidebar.
  2. Click the **COMPUTE** button to calculate the optimal route.
- **Filter & Constraints:** 
  - Toggle specific **Nodes** in the filter list to mark them as "Avoid" (e.g., closed roads).
  - Toggle **Transport Modes** (Road, Metro, Bus) to exclude them from the calculation.
- **Get Help:** Click the **HELP** button in the top-right corner of the interface at any time.

---

## 🛠️ Installation & Build

### Prerequisites (Linux)
Ensure you have the following installed:
- `cmake`
- `build-essential`
- `libsdl2-dev`
- `libgl1-mesa-dev`
- `zenity` (for file saving/loading)

### Prerequisites (Windows)
1. **Visual Studio 2022:** Install with the "Desktop development with C++" workload.
2. **CMake:** Download and install from [cmake.org](https://cmake.org/download/).
3. **SDL2 Development Library:**
   - Download the `SDL2-devel-2.x.x-VC.zip` from [GitHub Releases](https://github.com/libsdl-org/SDL/releases).
   - Extract it to a known location (e.g., `C:/SDL2`).

### Build Steps (Native - Linux)
1. **Clone the repository:**
   ```bash
   git clone https://github.com/yasirFakir/dijkstra-algorithm.git
   cd dijkstra-algorithm
   ```
2. **Compile:**
   ```bash
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ```
3. **Run:**
   ```bash
   ./dijkstra-algorithm
   ```

### Build Steps (Native - Windows)
1. **Open a terminal (PowerShell or CMD).**
2. **Clone the repository:**
   ```bash
   git clone https://github.com/yasirFakir/dijkstra-algorithm.git
   cd dijkstra-algorithm
   ```
3. **Configure and Build:**
   ```bash
   mkdir build
   cd build
   cmake .. -DSDL2_DIR="C:/path/to/SDL2/cmake"
   cmake --build . --config Release
   ```
   *Note: Replace `C:/path/to/SDL2/cmake` with the actual path to the `cmake` folder inside your extracted SDL2 directory.*
4. **Run:**
   - Copy `SDL2.dll` from the `lib/x64` folder of your SDL2 directory to the `Release` folder in your build directory.
   - Run `Release/dijkstra-algorithm.exe`.

### Build Steps (WebAssembly via Emscripten)
1. **Setup Emscripten environment.**
2. **Compile:**
   ```bash
   mkdir build-web && cd build-web
   emcmake cmake .. -DEMSCRIPTEN=ON
   make
   ```
3. **Serve:** Use a local web server (e.g., `python3 -m http.server`) to view `index.html`.

---

## 📄 Documentation
Comprehensive project details, including complexity analysis and architectural diagrams, can be found in:
- `CSE373_Semester Project.pdf` (Project Report)
- `docs/gemini.md` (Developer Notes)

---

## 🧠 Behind the Algorithm
The system uses a priority-queue based Dijkstra implementation ($O(E \log V)$).
When a path is computed:
1. It checks the **Avoidance List** to skip disabled nodes/edges.
2. It calculates the weight for each edge based on the selected **RouteMode**.
3. It stores each **Relaxation Step** to allow the UI to render the algorithmic trace.

---

## 📜 License & Acknowledgments
Created for the **CSE 373** course. Special thanks to the Dear ImGui and SDL2 communities.

**Contributors:** Yasir
