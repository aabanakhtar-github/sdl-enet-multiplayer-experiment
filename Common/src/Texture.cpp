#include "Texture.h"
#include <format>
#include "Util.h"
#include "Window.h"
#include "AppState.h"

TextureData::TextureData()
	: m_Height(0), m_Width(0), m_Renderer(nullptr), Surface(nullptr), Texture(nullptr), m_Valid(false)
{}

TextureData::TextureData(const TextureData& other)
	:  TextureData()
{
	if (!other.m_Valid)
	{
		return; 
	}

	m_Renderer = other.m_Renderer; 	
	Surface = SDL_CreateRGBSurfaceFrom(other.Surface->pixels, other.Surface->w, other.Surface->h,
		other.Surface->format->BitsPerPixel, other.Surface->pitch, other.Surface->format->Rmask,
			other.Surface->format->Gmask, other.Surface->format->Bmask, other.Surface->format->Amask);

	if (Surface == nullptr)
	{
		m_Valid = false;
		return;
	}

	Texture = SDL_CreateTextureFromSurface(m_Renderer, Surface);
	if (Texture == nullptr)
	{
		m_Valid = false; 
		return;
	}

	m_Width = Surface->w;
	m_Height = Surface->h;
	m_Valid = true;
}

TextureData::TextureData(TextureData&& other) noexcept
	: TextureData()
{
	swap(*this, other);
}

TextureData& TextureData::operator = (TextureData&& other) noexcept
{
	swap(*this, other);
	return *this;
}

TextureData& TextureData::operator = (TextureData other)
{
	swap(*this, other);	
	return *this;
}

bool TextureData::Load(Window& window, const std::string& filepath)
{
	SDL_FreeSurface(Surface);
	SDL_DestroyTexture(Texture);
	
	m_Valid = false;
	m_Renderer = window.m_SDLRenderer;
	Surface = SDL_LoadBMP(filepath.c_str());	
	
	if (Surface == nullptr)
	{
		m_Valid = false;
		return false;
	}
	
	SDL_Surface* window_surface = SDL_GetWindowSurface(window.m_SDLWindow);
	// set the iamge to use the right screen format for rendering performance
	SDL_Surface* optimized = SDL_ConvertSurface(Surface, window_surface->format, 0);
	if (optimized == nullptr)
	{
		m_Valid = false;
		return false;
	}
	
	SDL_FreeSurface(Surface);
	Surface = optimized;
	Texture = SDL_CreateTextureFromSurface(window.m_SDLRenderer, Surface);
	if (Texture == nullptr)
	{
		m_Valid = false;
		return false;
	}
	
	m_Width = Surface->w;
	m_Height = Surface->h;	
	m_Valid = true;	

	return true;
}

bool TextureData::Refresh() 
{
	SDL_DestroyTexture(Texture);
	Texture = SDL_CreateTextureFromSurface(m_Renderer, Surface);

	if (Texture == nullptr)
	{
		m_Valid = false;
		return false;
	}
	
	m_Valid = true;
	return true;
}

TextureData::~TextureData()
{
	if (Surface != nullptr)	
		SDL_FreeSurface(Surface);
	Surface = nullptr;

	if (Texture != nullptr)
		SDL_DestroyTexture(Texture);
	Texture = nullptr;
}
