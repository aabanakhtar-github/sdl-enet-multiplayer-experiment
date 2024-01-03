#include "Texture.h"

#include <format>

#include "Util.h"
#include "Window.h"

TextureData::TextureData()
	: Surface(nullptr), Texture(nullptr), m_Width(0), m_Height(0)
{
	
}

TextureData::TextureData(const TextureData& other)
	: m_Renderer(other.m_Renderer)
{
	Surface = SDL_DuplicateSurface(other.Surface);
	if (Surface == nullptr)
	{
	}

	Texture = SDL_CreateTextureFromSurface(m_Renderer, Surface);
	if (Texture == nullptr)
	{
		
	}

	//intentional not in init list
	m_Width = other.m_Width;
	m_Height = other.m_Height;
}

TextureData::TextureData(TextureData&& other) noexcept
	: m_Renderer(other.m_Renderer), Surface(other.Surface), Texture(other.Texture), m_Width(other.m_Width), m_Height(other.m_Height)
{
	other.m_Renderer = nullptr;
	other.Surface = nullptr;
	other.Texture = nullptr; 
	other.m_Width = 0;
	other.m_Height = 0; 
}

TextureData& TextureData::operator = (TextureData&& other) noexcept
{
	m_Renderer = other.m_Renderer;
	Surface = other.Surface;
	Texture = other.Texture;
	m_Width = other.m_Width;
	m_Height = other.m_Height;
	other.m_Renderer = nullptr;
	other.Surface = nullptr;
	other.Texture = nullptr; 
	other.m_Width = 0;
	other.m_Height = 0; 
	return *this;
}

// ADD DELETE TO EXISTING COPY 
TextureData& TextureData::operator = (const TextureData& other)
{	
	Surface = SDL_DuplicateSurface(other.Surface);
	CUSTOM_ASSERT(Surface != nullptr, std::format("Could not create surface! SDL Error: {}", SDL_GetError()).c_str());
	Texture = SDL_CreateTextureFromSurface(m_Renderer, Surface);
	CUSTOM_ASSERT(Texture != nullptr, std::format("Could not create texture! SDL Error: {}", SDL_GetError()).c_str());
	//intentional not in init list
	m_Width = other.m_Width;
	m_Height = other.m_Height;
	return *this;
}

bool TextureData::Load(Window& window, const std::string& filepath)
{
	m_Renderer = window.m_SDLRenderer;

	Surface = SDL_LoadBMP(filepath.c_str());
	CUSTOM_ASSERT(Surface != nullptr, std::format("Could not load surface! SDL Error: {}", SDL_GetError()).c_str());
	SDL_Surface* window_surface = SDL_GetWindowSurface(window.m_SDLWindow);
	// set the iamge to use the right screen format for rendering performance
	SDL_Surface* optimized = SDL_ConvertSurface(Surface, window_surface->format, 0);
	SDL_FreeSurface(Surface);
	Surface = optimized;

	Texture = SDL_CreateTextureFromSurface(window.m_SDLRenderer, Surface);
	CUSTOM_ASSERT(Texture != nullptr,  std::format("Could not load Texture! SDL Error: {}", SDL_GetError()).c_str());
	m_Width = Surface->w;
	m_Height = Surface->h;
	
	return true;
}

TextureData::~TextureData()
{
	if (Surface != nullptr)
	{
		SDL_FreeSurface(Surface);
	}
	if (Texture != nullptr)
	{
		SDL_DestroyTexture(Texture);
	}
}


void TextureManager::AddTexture(Window& window, const std::string& key, const std::string& filepath)
{
	CUSTOM_ASSERT(!m_Textures.contains(key), std::format("Cannot insert an already existing texture {} into Manager!", key).c_str());
	TextureData tmp;
	tmp.Load(window, filepath);
	m_Textures.insert(std::make_pair(key, tmp));
}

