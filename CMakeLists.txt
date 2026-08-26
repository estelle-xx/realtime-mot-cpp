cmake_minimum_required(VERSION 3.16)

project(realtime_mot_cpp)

find_package(OpenCV REQUIRED)

add_executable(realtime_mot 
    src/main.cpp
    src/core/VideoSource.cpp
    src/detection/Detector.cpp
    )

target_include_directories(realtime_mot PRIVATE include)

target_link_libraries(realtime_mot ${OpenCV_LIBS})