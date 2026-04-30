# Adaptive City Routing Project

An efficient C++ based routing engine designed to find optimal paths in a city graph based on different modes (e.g., shortest distance, fastest time). This project utilizes Dijkstra's algorithm and is designed for cross-platform compatibility, including a WebAssembly build for web-based interaction.

## Project Structure

- `src/`: Core C++ source files implementing the graph data structures and routing algorithms.
  - `graph.h`: Graph class definitions and node/edge structures.
  - `graph.cpp`: Implementation of graph construction from input data.
  - `algorithms.cpp`: Dijkstra's algorithm implementation with multi-mode logic.
  - `main.cpp`: Entry point for CLI and ImGui-based user interface.
- `assets/`: Static resources including the city map data and project specifications.
- `web/`: Build artifacts for WebAssembly, allowing the project to run in a browser.
- `docs/`: Technical documentation and algorithm walkthroughs.
- `CMakeLists.txt`: Build configuration for C++ compilers and Emscripten.

## Features

- **Multi-Mode Routing**: Find paths based on distance or time.
- **Efficient Graph Processing**: Optimized Dijkstra implementation for performance.
- **Cross-Platform**: Compiles to native binaries and WebAssembly.
- **Interactive UI**: Integrated with Dear ImGui for a visual experience (optional/web).

## Getting Started

### Prerequisites

- CMake (3.10 or higher)
- C++17 compliant compiler (GCC, Clang, or MSVC)
- [Optional] Emscripten SDK for WebAssembly builds

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

### Running the Application

```bash
./CityRouting
```

## License

This project is developed for educational purposes as part of the CSE373 course.
