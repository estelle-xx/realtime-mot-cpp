cmake_minimum_required(VERSION 3.16)

project(person_reid_system)

find_package(OpenCV REQUIRED)

add_executable(person_reid_system 
    src/main.cpp
    src/core/VideoSource
    )

target_include_directories(person_reid_system PRIVATE include)

target_link_libraries(person_reid_system ${OpenCV_LIBS})