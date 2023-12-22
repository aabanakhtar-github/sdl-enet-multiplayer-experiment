project(CoreLib)

set(SOURCES grr.cpp)
add_subdirectory(vendor/SDL)
add_library(STAMINAZE_${PROJECT_NAME} ${SOURCES}) 
target_include_directories(STAMINAZE_${PROJECT_NAME} PUBLIC 
	include
	vendor/SDL/include
)
target_link_libraries(STAMINAZE_${PROJECT_NAME} SDL3::SDL3-shared)