cmake_minimum_required(VERSION 3.26) 

project(Game)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True) 


add_subdirectory(Common) 
add_subdirectory(Game) 

target_link_libraries(Server Common)
target_link_libraries(Client Common)
