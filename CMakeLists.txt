cmake_minimum_required(VERSION 3.26) 
project(Game)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True) 

if (CMAKE_BUILD_TYPE STREQUAL "Debug") 
	add_compile_definitions("__DEBUG__")
endif() 

add_subdirectory(Game) 
add_subdirectory(Core) 

target_link_libraries(Game CoreLib)