
cmake_minimum_required(VERSION 3.21)

project(GradeTrack VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

qt_standard_project_setup()

qt_add_executable(GradeTrack
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
)

target_link_libraries(GradeTrack PRIVATE Qt6::Widgets)

target_include_directories(GradeTrack PRIVATE src)

enable_testing()
add_executable(GradeTrackTests tests/main.cpp)
target_link_libraries(GradeTrackTests PRIVATE Qt6::Core)
target_include_directories(GradeTrackTests PRIVATE src)
add_test(NAME GradeTrackTests COMMAND GradeTrackTests)
