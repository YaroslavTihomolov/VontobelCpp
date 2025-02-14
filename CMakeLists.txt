cmake_minimum_required(VERSION 3.16)
project(Vontobel)

set(CMAKE_CXX_STANDARD 20)

add_executable(Vontobel main.cpp Matrix.cpp Vontobel.cpp Combination.cpp example.cpp)

target_include_directories(Vontobel PUBLIC C:/WinNTL-11_5_1/include)
target_link_libraries(Vontobel PUBLIC C:/WinNTL-11_5_1/libntl_lib.a)