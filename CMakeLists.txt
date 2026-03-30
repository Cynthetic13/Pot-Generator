cmake_minimum_required(VERSION 4.1)
project(Pot_Generator)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(WIN32)

endif(WIN32)

if(UNIX)
    find_package(SDL3 REQUIRED)
endif(UNIX)

add_executable(Pot_Generator main.cpp)

target_link_libraries(Pot_Generator PRIVATE SDL3::SDL3)