cmake_minimum_required(VERSION 3.02)

include_directories(
./module 
./Syinx 
./Sylog 
)

file(GLOB srcs 
./main.cpp 
./module/*.cpp 
./Syinx/*.cpp 
./Sylog/*.cpp 
./Sylog/*.cc 
)

LINK_DIRECTORIES(/usr/local/lib/)

add_executable(gameserver ${srcs})


TARGET_LINK_LIBRARIES (
gameserver 
-lpthread 
-levent
-lprotobuf
-lmysqlclient
)