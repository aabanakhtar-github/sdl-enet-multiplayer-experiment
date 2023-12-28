project(CoreLib)

set(SOURCES
	src/Window.cpp
	src/Texture.cpp
)
add_subdirectory(vendor/SDL)
add_library(STAMINAZE_${PROJECT_NAME} ${SOURCES} "src/Texture.cpp" "include/ECS.h") 
target_include_directories(STAMINAZE_${PROJECT_NAME} PUBLIC 
	include
	vendor/SDL/include
)

if(MINGW) 
	target_link_libraries(STAMINAZE_${PROJECT_NAME} mingw32)
endif()

target_link_libraries(STAMINAZE_${PROJECT_NAME} SDL2::SDL2main SDL2::SDL2)