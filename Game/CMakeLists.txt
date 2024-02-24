# Build files for the Game 
project(Game)

set(SOURCES 
	Main.cpp
	Game.cpp 
	Systems/GraphicsSystem.cpp
	Systems/PhysicsSystem.cpp
	Systems/PlayerInputSystem.cpp
) 

add_executable( ${PROJECT_NAME} ${SOURCES} ) 
target_include_directories(Game PRIVATE Systems)
