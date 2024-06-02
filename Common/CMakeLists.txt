project(CoreLib)

add_subdirectory(vendor/SDL)
set(SDL2IMAGE_SAMPLES OFF)
add_subdirectory(vendor/SDL_image)
add_subdirectory(vendor/enet)


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
	vendor/SDL/include
	vendor/enet/include
	vendor/SDL_image/include
)

if (MSVC)
	target_link_libraries(${PROJECT_NAME} SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image enet ws2_32.lib winmm.lib)
elseif(MINGW)
	target_link_libraries(${PROJECT_NAME} mingw32 SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image enet)
else()
	target_link_libraries(${PROJECT_NAME} SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image enet)
endif()
