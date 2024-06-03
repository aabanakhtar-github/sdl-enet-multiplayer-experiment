cmake_minimum_required(VERSION 3.26)
project(CoreLib)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
# Building Enet cross platform requires source build i guess
add_subdirectory(lib/enet-1.3.18)

set(SOURCES
	src/Window.cpp
	src/Texture.cpp
	src/ECS.cpp
	src/EventHandler.cpp
	src/Netcode/Packet.cpp
	src/Netcode/Client.cpp
	src/Netcode/Server.cpp
)

add_library(${PROJECT_NAME} ${SOURCES}) 

target_include_directories(${PROJECT_NAME} PUBLIC 
	include
	${SDL2_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIRS}
	lib/enet-1.3.18/include
)

if (MSVC)
	target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} enet ws2_32.lib winmm.lib)
elseif(MINGW)
	target_link_libraries(${PROJECT_NAME} mingw32 ${SDL2_LIBRARIES} enet) 
else()
	target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} enet)
endif()
