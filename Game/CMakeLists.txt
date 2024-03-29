# Build files for the Game 
project(Game)

set(_SHARED_SOURCES  
	Systems/GraphicsSystem.cpp
	Systems/PhysicsSystem.cpp
) 

set(CLIENT_SOURCES 
	Client/Main.cpp 
	Client/Game.cpp 	
	Systems/ClientEventSystem.cpp
)

set(SERVER_SOURCES
	Server/Main.cpp
	Server/Game.cpp
)

add_executable(Server ${SERVER_SOURCES} ${_SHARED_SOURCES})
add_executable(Client ${CLIENT_SOURCES} ${_SHARED_SOURCES})

target_include_directories(Server PRIVATE Systems)
target_include_directories(Client PRIVATE Systems)