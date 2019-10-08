cmake_minimum_required(VERSION 3.02)

file(GLOB srcs *.cpp)

add_executable(Syinx ${srcs})

target_link_libraries(Syinx event pthread jsoncpp)