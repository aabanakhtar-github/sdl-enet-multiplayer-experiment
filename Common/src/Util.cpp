#include "Util.h"
#include "SDL.h"
#include "SDL_image.h"
#include "enet/enet.h"

void GlobalAppState::setAppState(const AppState state, const std::string& error) {
    app_state_ = state; 
    if (error.size() > 0) { 
        error_.push_back(error);
    }
}

void initLibraries() { 
    if (enet_initialize() != 0) { 

    }
    
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0 && IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG)) { 

    }
}

void shutdownLibraries() { 
    IMG_Quit(); 
    SDL_Quit();
    enet_deinitialize();
}