cmake_minimum_required(VERSION 3.20)
project(engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

include(FetchContent)
FetchContent_Declare(
    easy_profiler
    GIT_REPOSITORY https://github.com/yse/easy_profiler.git
    GIT_TAG        v2.1.0
)
set(EASY_PROFILER_NO_GUI ON CACHE BOOL "" FORCE)
set(EASY_PROFILER_NO_SAMPLES ON CACHE BOOL "" FORCE)

# easy_profiler's own CMakeLists.txt declares an ancient cmake_minimum_required
# that modern CMake refuses outright. This tells CMake "treat anything down to
# 3.5 as compatible" so the subproject's old build script still configures.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

FetchContent_MakeAvailable(easy_profiler)

add_executable(engine
    src/profile_easier.cpp
    third_party/glad/src/glad.c
)

target_include_directories(engine PRIVATE
    third_party/glad/include
)

target_link_libraries(engine PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    easy_profiler
)
