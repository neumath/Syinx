cmake_minimum_required(VERSION 3.02)

include_directories(./module ./Timer ./Syinx ./Sylog)

file(GLOB srcs ./main.cpp ./module/*.cpp ./Timer/*.cpp ./Syinx/*.cpp ./Sylog/*.cpp)

add_executable(Runtest ${srcs})

target_link_libraries(Runtest pthread event mysqlclient)
