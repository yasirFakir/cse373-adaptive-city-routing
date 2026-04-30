cmake_minimum_required(VERSION 3.14)
project(CSE373-Adaptive-City-Routing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/graph.cpp
    src/algorithms.cpp
)

# ImGui setup
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.90.4
)
FetchContent_MakeAvailable(imgui)

# Add ImGui sources (Core + SDL2 + OpenGL3)
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

include_directories(src ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

if(EMSCRIPTEN)
    # WebAssembly specific flags
    add_executable(CSE373-Adaptive-City-Routing ${SOURCES} ${IMGUI_SOURCES})
    set_target_properties(CSE373-Adaptive-City-Routing PROPERTIES SUFFIX ".js")
    
    # Emscripten requires these flags for both compiling and linking
    set(EMSCRIPTEN_FLAGS
        "-s USE_SDL=2"
        "-s USE_WEBGL2=1"
        "-s FULL_ES3=1"
        "-s WASM=1"
        "-s ALLOW_MEMORY_GROWTH=1"
        "-s NO_EXIT_RUNTIME=1"
    )

    target_compile_options(CSE373-Adaptive-City-Routing PRIVATE ${EMSCRIPTEN_FLAGS})
    target_link_options(CSE373-Adaptive-City-Routing PRIVATE 
        ${EMSCRIPTEN_FLAGS}
        "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
    )
    
    # Ensure headers are found
    target_include_directories(CSE373-Adaptive-City-Routing PRIVATE 
        src 
        ${imgui_SOURCE_DIR} 
        ${imgui_SOURCE_DIR}/backends
    )
else()
    # Native build (requires SDL2 and OpenGL installed on system)
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
    add_executable(CSE373-Adaptive-City-Routing ${SOURCES} ${IMGUI_SOURCES})
    target_link_libraries(CSE373-Adaptive-City-Routing PRIVATE SDL2::SDL2 OpenGL::GL)
endif()
