cmake_minimum_required(VERSION 3.20)
project(engine LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- glad: vendored ---
add_library(glad STATIC third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

# --- stb (header-only, fetched) ---
include(FetchContent)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(stb)

add_executable(engine src/load_tex_ref.cpp src/cube_tex.cpp)
target_include_directories(engine PRIVATE ${stb_SOURCE_DIR})
target_link_libraries(engine PRIVATE OpenGL::GL glfw glad glm::glm assimp::assimp ${CMAKE_DL_LIBS})
