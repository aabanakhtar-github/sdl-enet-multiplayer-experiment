#include "Texture.h"
#include <format>
#include "Util.h"
#include "Window.h"
#include "SDL_image.h"

TextureData::TextureData()
	: m_Height(0), 
	  m_Width(0), 
	  m_Renderer(nullptr), 
	  Surface(nullptr), 
	  Texture(nullptr), 
	  m_Valid(false)
{}

TextureData::TextureData(const TextureData& other)
	:  TextureData() {
	if (!other.m_Valid) {
		return; 
	}

	m_Renderer = other.m_Renderer; 	
	Surface = SDL_CreateRGBSurfaceFrom(other.Surface->pixels, other.Surface->w, other.Surface->h,
		other.Surface->format->BitsPerPixel, other.Surface->pitch, other.Surface->format->Rmask,
			other.Surface->format->Gmask, other.Surface->format->Bmask, other.Surface->format->Amask);

	if (Surface == nullptr) {
		m_Valid = false;
		return;
	}

	Texture = SDL_CreateTextureFromSurface(m_Renderer, Surface);
	if (Texture == nullptr) {
		m_Valid = false; 
		return;
	}

	m_Width = Surface->w;
	m_Height = Surface->h;
	m_Valid = true;
}

TextureData::TextureData(TextureData&& other) noexcept
	: TextureData() {
	swap(*this, other);
}

TextureData& TextureData::operator = (TextureData&& other) noexcept {
	swap(*this, other);
	return *this;
}

TextureData& TextureData::operator = (TextureData other) {
	swap(*this, other);	
	return *this;
}

bool TextureData::load(Window& window, const std::string& filepath) {
	if (Surface != nullptr) {
        SDL_FreeSurface(Surface);
    }

    if (Texture != nullptr) {
        SDL_DestroyTexture(Texture);
    }

	m_Valid = false;
	m_Renderer = window.renderer_;
	Surface = IMG_Load(filepath.c_str());	
	
	if (Surface == nullptr) {
		m_Valid = false;
		return false;
	}
	
	SDL_Surface* window_surface = SDL_GetWindowSurface(window.window_);
	// set the image to use the right screen format for rendering performance
	SDL_Surface* optimized = SDL_ConvertSurface(Surface, window_surface->format, 0);

	if (optimized == nullptr) {
		m_Valid = false;
		return false;
	}
	
	SDL_FreeSurface(Surface);
	Surface = optimized;
    // set color key
    SDL_SetColorKey(Surface, SDL_TRUE, SDL_MapRGB(Surface->format, 0, 255, 255));

	Texture = SDL_CreateTextureFromSurface(window.renderer_, Surface);

	if (Texture == nullptr)	{
		m_Valid = false;
		return false;
	}
	
	m_Width = Surface->w;
	m_Height = Surface->h;	
	m_Valid = true;
	return true;
}

// bro does this even work
bool TextureData::refresh() {
	if (Texture == nullptr || Surface == nullptr) { 
		return false; 
	}

	SDL_DestroyTexture(Texture);
	Texture = SDL_CreateTextureFromSurface(m_Renderer, Surface);

	if (Texture == nullptr) {
		m_Valid = false;
		return false;
	}
	
	m_Valid = true;
	return true;
}

TextureData::~TextureData() {
	if (Surface != nullptr) {
		SDL_FreeSurface(Surface);
	}	
	
	if (Texture != nullptr) {	
		SDL_DestroyTexture(Texture);
	}

	Surface = nullptr;
	Texture = nullptr;
}

TextureData &TextureManager::getTexture(const std::string &key) {
    return textures_[key];
}

bool TextureManager::addTexture(Window &window, const std::string &filepath, const std::string &key) {
    TextureData t;
    if(!t.load(window, filepath)) {
        return false;
    }

    textures_.insert({ key, std::move(t) });
    return true;
}
