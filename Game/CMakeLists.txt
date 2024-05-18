# Build files for the Game 
project(Game)

set(_SHARED_SOURCES  
	Common/GraphicsSystem.cpp
	Common/PhysicsSystem.cpp
	Common/CreateScenes.cpp
) 

set(CLIENT_SOURCES 
	Client/Main.cpp 
	Client/Game.cpp 	
	Client/ClientEventSystem.cpp
)

set(SERVER_SOURCES
	Server/Main.cpp
	Server/Game.cpp
	Server/ServerEventSystem.cpp
)

add_executable(Server ${SERVER_SOURCES} ${_SHARED_SOURCES})
add_executable(Client ${CLIENT_SOURCES} ${_SHARED_SOURCES})

target_include_directories(Server PRIVATE Common)
target_include_directories(Client PRIVATE Common)