cmake_minimum_required(VERSION 3.20)
project(engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(engine
    src/main.cpp
    third_party/glad/src/glad.c
)

target_include_directories(engine PRIVATE
    third_party/glad/include
)

target_link_libraries(engine PRIVATE
    OpenGL::GL
    glfw
)
