# Dijkstra Algorithm - Adaptive City Routing System
### CSE 373: Design and Analysis of Algorithms Project

---

## 1. Algorithmic Foundation
This system utilizes **Dijkstra’s Algorithm** to solve multi-constraint routing problems within a dynamic urban graph environment.

### **The Multi-Weight Model**
Edges represent city connections with three primary metrics:
*   **Distance ($d$):** Physical length between nodes.
*   **Time ($t$):** Estimated travel time based on transport type.
*   **Cost ($c$):** Financial cost of traversal.

The engine supports three optimization modes: `Shortest Distance`, `Fastest Time`, and `Cheapest Route`.

---

## 2. Dynamic Constraint System
The project features a real-time filtering system that allows users to customize pathfinding:
*   **Node Exclusion:** A scrollable checklist allows users to disable specific nodes. Excluded nodes are marked with a **Red X** on the graph and are skipped by the algorithm.
*   **Transport Type Filtering:** Users can toggle specific transport modes (Road, Metro, Bus).
    *   **Road:** Visualized in **Blue**.
    *   **Metro:** Visualized in **Purple**.
    *   **Bus:** Visualized in **Orange**.
*   **Real-time Interaction:** Toggling any filter instantly updates the graph visualization and recomputes active paths.

---

## 3. Visualization & Interface
The UI is built with **Dear ImGui** and optimized for both native and **WebAssembly** environments.

### **Interactive Graph Canvas**
*   **Start/End Indicators:** The Start node is highlighted in **Green**, and the End node in **Cyan**.
*   **Path Highlighting:** The optimal route is rendered with a bold **Green** line.
*   **Edge Creation:** Users can add nodes and drag connections. A color-coded "wire" (matching the selected creation type) follows the mouse during edge creation.
*   **Theme Support:** A minimalist **Dark/Light Mode** toggle ensures high contrast and readability across all environments.

### **WebAssembly Capabilities**
*   **Local File Upload:** Users can upload `.txt` map files directly from their PC to the live browser version using a custom JavaScript/C++ bridge.
*   **Asset Loading:** Quick access to pre-defined map structures (City, Circular, Grid).

---

## 4. Performance & Analytics
The system provides deep insights into the algorithm's execution:
*   **Results Summary:** Displays the total distance, time, cost, and precise **execution time (ms)** using high-resolution timers.
*   **Enhanced Trace Table:** A standard representation of the **Relaxation Theorem**.
    *   Shows step-by-step distance updates.
    *   Displays **Predecessors** for each node (e.g., `50(B)`).
    *   Highlights the **currently visited node** in Green to illustrate the Priority Queue extraction process.

---

> **Final Note:** This project demonstrates the practical application of graph theory and Dijkstra's algorithm, providing a robust, interactive tool for urban routing analysis.
