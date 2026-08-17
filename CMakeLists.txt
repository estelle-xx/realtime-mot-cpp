cmake_minimum_required(VERSION 3.16)

project(person_reid_system)

find_package(OpenCV REQUIRED)

add_executable(person_reid_system src/main.cpp)

target_link_libraries(person_reid_system ${OpenCV_LIBS})