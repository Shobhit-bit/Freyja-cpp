cmake_minimum_required(VERSION 3.20)
project(meopti LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- glad: vendored, not a system package ---
# Generate it once from https://gen.glad.sh/ (gl -> Core -> 4.5 -> C/C++)
# and extract into third_party/glad/ so this layout exists:
#   third_party/glad/include/glad/glad.h
#   third_party/glad/include/KHR/khrplatform.h
#   third_party/glad/src/glad.c
add_library(glad STATIC third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)

add_executable(meopti src/meopti.cpp)

# --- OpenGL / windowing ---
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# --- Math ---
find_package(glm REQUIRED)

# --- Model loading ---
find_package(assimp REQUIRED)

# --- Mesh optimization (not commonly packaged, so fetch it) ---
include(FetchContent)
FetchContent_Declare(
    meshoptimizer
    GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
    GIT_TAG v0.22
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(meshoptimizer)

target_link_libraries(meopti PRIVATE
    OpenGL::GL
    glfw
    glad
    glm::glm
    assimp::assimp
    meshoptimizer
)
