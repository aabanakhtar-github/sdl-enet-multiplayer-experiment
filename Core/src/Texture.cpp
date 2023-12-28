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

TextureData& TextureData::operator = (const TextureData& other)
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
	return *this;
}

bool TextureData::Load(Window& window, const std::string& filepath)
{
	m_Renderer = window.m_SDLRenderer;

	Surface = SDL_LoadBMP(filepath.c_str());
	if (Surface == nullptr)
	{
		PRINT_SDL_ERROR_MESSAGE(std::format("Could not load Texture: {}", filepath));
		return false;
	}

	SDL_Surface* window_surface = SDL_GetWindowSurface(window.m_SDLWindow);
	// set the iamge to use the right screen format for rendering performance
	SDL_Surface* optimized = SDL_ConvertSurface(Surface, window_surface->format, 0);
	SDL_FreeSurface(Surface);
	Surface = optimized;

	Texture = SDL_CreateTextureFromSurface(window.m_SDLRenderer, Surface);
	if (Texture == nullptr)
	{
		PRINT_SDL_ERROR_MESSAGE(std::format("could not create Texture from Surface: {} !", filepath));
		return false;
	}
	
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
	if (m_Textures.contains(key))
	{
		PRINT_SDL_ERROR_MESSAGE(std::format("CANNOT ADD TEXTURE THAT ALREADY EXISTS TO TEXTURE MANAGER! Key: {} ", key));
		return;
	}
	TextureData tmp;
	tmp.Load(window, filepath);
	m_Textures.insert(std::make_pair(key, tmp));
}

