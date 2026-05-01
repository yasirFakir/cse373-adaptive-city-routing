cmake_minimum_required(VERSION 3.14)
project(dijkstra-algorithm)

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
    add_executable(dijkstra-algorithm ${SOURCES} ${IMGUI_SOURCES})
    set_target_properties(dijkstra-algorithm PROPERTIES SUFFIX ".js")
    
    # Flags needed for BOTH compiling (for headers) and linking
    target_compile_options(dijkstra-algorithm PRIVATE "-sUSE_SDL=2" -DIMGUI_IMPL_OPENGL_ES3)
    
    # Flags strictly for the Linker
    target_link_options(dijkstra-algorithm PRIVATE 
        "-sUSE_SDL=2"
        "-sUSE_WEBGL2=1"
        "-sFULL_ES3=1"
        "-sWASM=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sNO_EXIT_RUNTIME=1"
        "--preload-file assets@/assets"
    )
    
    # Ensure headers are found
    target_include_directories(dijkstra-algorithm PRIVATE 
        src 
        ${imgui_SOURCE_DIR} 
        ${imgui_SOURCE_DIR}/backends
    )
else()
    # Native build (requires SDL2 and OpenGL installed on system)
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
    add_executable(dijkstra-algorithm ${SOURCES} ${IMGUI_SOURCES})
    target_link_libraries(dijkstra-algorithm PRIVATE SDL2::SDL2 OpenGL::GL)

    # Copy assets to build directory for local execution
    add_custom_command(TARGET dijkstra-algorithm POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:dijkstra-algorithm>/assets)
endif()
