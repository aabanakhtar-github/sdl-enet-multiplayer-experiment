cmake_minimum_required(VERSION 3.26)

project(Common)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory(lib/SDL)
add_subdirectory(lib/SDL_image)
add_subdirectory(lib/enet)

set(SOURCES
	src/Window.cpp
	src/Texture.cpp
	src/ECS.cpp
	src/Util.cpp
	src/EventHandler.cpp
	src/Netcode/Packet.cpp
	src/Netcode/Client.cpp
	src/Netcode/Server.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES}) 

target_include_directories(${PROJECT_NAME} PUBLIC 
	include
	lib/SDL_image/include 
	lib/SDL/include
	lib/enet/include
)

if (MSVC)
	target_link_libraries(${PROJECT_NAME} SDL2::SDL2main SDL2::SDL2 SDL2_image enet ws2_32.lib winmm.lib)
elseif(MINGW)
	target_link_libraries(${PROJECT_NAME} mingw32 SDL2::SDL2main SDL2::SDL2 SDL2_image enet)
else()
  target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2_image enet)
endif()
