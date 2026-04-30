# ## Adaptive Multi-Constraint City Routing System
### CSE 373: Design and Analysis of Algorithms Project

---

## ## 1. Algorithmic Foundation
This project implements **Dijkstra’s Algorithm** to solve complex multi-constraint routing problems within a urban graph environment[cite: 1].

### **The Multi-Weight Model**
Each connection in the city map is represented as a directed edge with a weight vector $W$:
*   **$d$**: Physical distance (meters).
*   **$t$**: Travel time (seconds), varying by transport mode.
*   **$c$**: Financial cost (currency).

The core engine uses a **Min-Priority Queue** to explore nodes. To support "Custom Weighted Routes," the algorithm calculates a composite scalar weight $W_{final}$ based on user-defined coefficients $\alpha, \beta,$ and $\gamma$.

---

## ## 2. Interactive Input Panel
*The GUI homepage will feature these primary controls to satisfy the Functional Requirements:*

*   **Location Selection**: Input for **Source** and **Destination** nodes.
*   **Optimization Strategy**: Selection between:
    *   `Shortest Distance` (Minimizes $d$).
    *   `Fastest Route` (Minimizes $t$).
    *   `Cheapest Route` (Minimizes $c$).
    *   `Balanced/Custom` (Uses weight sliders for $\alpha, \beta, \gamma$).
*   **Dynamic Constraints**: Toggle switches to exclude specific nodes or transport types (e.g., "Avoid Metro").
*   **Execution Trigger**: A "Compute Route" button that initiates the Dijkstra logic and performance timer.

---

## ## 3. Algorithm Step-by-Step Walkthrough
*To provide an educational "Showcase" experience, the UI will visualize the internal state of the algorithm during execution.*

### **Step 1: Initialization**
*   Set all node distances to $\infty$, except for the Source node ($0$).
*   Populate the Priority Queue with the Source node.
*   Display the initial **Distance Table** in the GUI sidebar.

### **Step 2: Iterative Relaxation**
*   **Highlight Current Node**: The node currently being "relaxed" will glow in the UI.
*   **Edge Evaluation**: Visualize the calculation $Distance(u) + weight(u, v) < Distance(v)$.
*   **Constraint Check**: If a path violates a user constraint, the UI will mark that edge as "Filtered" and skip it.

### **Step 3: Path Reconstruction**
*   Once the Destination node is extracted from the Priority Queue, the algorithm stops.
*   The UI will "backtrack" through the Parent Map to highlight the optimal path in bold colors.

---

## ## 4. Performance Analytics
*After the walkthrough, a summary dashboard will appear:*

| Metric | Purpose |
| :--- | :--- |
| **Total Path Cost** | Sum of distance, time, and cost in the chosen path. |
| **Execution Speed** | Measured in microseconds ($\mu s$) using `<chrono>`. |
| **Nodes Visited** | Total number of nodes processed by the Priority Queue. |

---

> **Developer Note**: This specification guides the implementation of `main.cpp` using **Dear ImGui** and **WebAssembly** for hosting on GitHub Pages.
