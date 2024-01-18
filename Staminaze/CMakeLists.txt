# Build files for the Game 
project(StaminazeExecutable)
set(SOURCES 
	Main.cpp
	Game.cpp 
	GraphicsSystem.cpp
	CollisionSystem.cpp
) 

add_executable( STAMINAZE_${PROJECT_NAME} ${SOURCES}  "Game.cpp" "CollisionSystem.h") 
