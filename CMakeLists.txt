cmake_minimum_required(VERSION 3.22)

project(nodeapackage VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Test)

qt_standard_project_setup(REQUIRES 6.10)

add_subdirectory(NodeEditor)
add_subdirectory(examples/Demo)
add_subdirectory(examples/time-display)
add_subdirectory(examples/led-matrix)
add_subdirectory(examples/key-press)
add_subdirectory(examples/electric-demo)
add_subdirectory(examples/script-demo)
# add_subdirectory(examples/random-value-display)
add_subdirectory(tests)
