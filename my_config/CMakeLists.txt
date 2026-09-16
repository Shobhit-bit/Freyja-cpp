cmake_minimum_required(VERSION 3.20)
project(engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

# easy_profiler's own CMakeLists.txt declares an ancient cmake_minimum_required
# that modern CMake refuses outright. This tells CMake "treat anything down to
# 3.5 as compatible" so the subproject's old build script still configures.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

file(GLOB ETC2COMP_SOURCES
    third_party/etc2comp/EtcLib/Etc/*.cpp
    third_party/etc2comp/EtcLib/EtcCodec/*.cpp
    third_party/etc2comp/EtcTool/EtcFile.cpp
    third_party/etc2comp/EtcTool/EtcFileHeader.cpp
)

add_executable(engine
    src/etc.cpp
    third_party/glad/src/glad.c
    ${ETC2COMP_SOURCES}
)
target_include_directories(engine PRIVATE
    third_party/glad/include
    third_party
    third_party/etc2comp/EtcLib/Etc
    third_party/etc2comp/EtcLib/EtcCodec
    third_party/etc2comp/EtcTool
)

target_link_libraries(engine PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    assimp::assimp
)
