cmake_minimum_required(VERSION 3.26) 
project(Staminaze)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True) 

add_subdirectory(Staminaze) 
add_subdirectory(Core) 

target_link_libraries(STAMINAZE_StaminazeExecutable STAMINAZE_CoreLib)