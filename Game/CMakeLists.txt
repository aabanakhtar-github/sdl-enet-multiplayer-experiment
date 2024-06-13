
cmake_minimum_required(VERSION 3.26)

# Build files for the Game
project(Game)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(SHARED_SOURCES
	Common/GraphicsSystem.cpp
	Common/CreateScenes.cpp
	Common/AnimationSystem.cpp
)

set(CLIENT_SOURCES 
	Client/Main.cpp 
	Client/Game.cpp 	
	Client/ClientEventSystem.cpp
	Client/CameraFollowerSystem.cpp
)

set(SERVER_SOURCES
	Server/PhysicsSystem.cpp
	Server/Main.cpp
	Server/Game.cpp
	Server/ServerEventSystem.cpp
)

add_executable(Server ${SERVER_SOURCES} ${SHARED_SOURCES})
add_executable(Client ${CLIENT_SOURCES} ${SHARED_SOURCES})

target_include_directories(Server PRIVATE Common)
target_include_directories(Client PRIVATE Common)