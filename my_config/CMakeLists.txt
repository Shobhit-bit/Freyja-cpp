cmake_minimum_required(VERSION 3.20)
project(engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(engine
    src/GuiIm.cpp
    third_party/glad/src/glad.c
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_demo.cpp
)

target_include_directories(engine PRIVATE
    third_party/glad/include
    third_party/imgui
)

target_link_libraries(engine PRIVATE
    OpenGL::GL
    glfw
    glm::glm
)
