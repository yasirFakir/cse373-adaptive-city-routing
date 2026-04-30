cmake_minimum_required(VERSION 3.10)
project(CSE373-Adaptive-City-Routing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/graph.cpp
    src/algorithms.cpp
)

# Headers
include_directories(src)

# Executable
add_executable(CityRouting ${SOURCES})

# WebAssembly (conditional if using Emscripten)
if(EMSCRIPTEN)
    set_target_properties(CityRouting PROPERTIES SUFFIX ".js")
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    # Add Emscripten specific flags here
endif()
