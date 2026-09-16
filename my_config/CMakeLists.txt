cmake_minimum_required(VERSION 3.20)
project(taskflow_demo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(taskflow_demo src/taskflow.cpp)

target_include_directories(taskflow_demo PRIVATE
    third_party/taskflow
)

find_package(Threads REQUIRED)
target_link_libraries(taskflow_demo PRIVATE Threads::Threads)
