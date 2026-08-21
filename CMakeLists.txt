cmake_minimum_required(VERSION 3.16)

project(realtime_mot_cpp)

find_package(OpenCV REQUIRED)

add_executable(realtime_mot 
    src/main.cpp
    src/core/VideoSource
    )

target_include_directories(person_reid_system PRIVATE include)

target_link_libraries(person_reid_system ${OpenCV_LIBS})