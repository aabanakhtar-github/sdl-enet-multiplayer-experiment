#ifndef TEXTURE_H
#define TEXTURE_H

#include <string> 
#include <cstdint> 
#include <unordered_map>

#include "SDL.h" 
#include "Util.h"

class Window;

class TextureData
{
public:
	TextureData();
	TextureData(const TextureData&);
	TextureData(TextureData&&) noexcept;
	TextureData& operator = (TextureData);
	TextureData& operator = (TextureData&&) noexcept;
	~TextureData();
	
	bool Load(Window& window, const std::string& filepath);
	// call after making modifications to the surface, reloading the texture basically	
	bool Refresh() {}
	// cool feature!
	friend void swap(TextureData& a, TextureData& b)
	{
		using std::swap;
		swap(a.Surface, b.Surface);
		swap(a.Texture, b.Texture);
		swap(a.m_Width, b.m_Width);
		swap(a.m_Height, b.m_Height);
		swap(a.m_Renderer, b.m_Renderer);
	}

	std::uint16_t GetWidth() { return m_Width; }
	std::uint16_t GetHeight() { return m_Height; }
public: 
	SDL_Surface* Surface; 
	SDL_Texture* Texture;

private:
	std::uint16_t m_Width;
	std::uint16_t m_Height;
	SDL_Renderer* m_Renderer;
};

class TextureManager
{
	MAKE_SINGLETON(TextureManager)

public:
	TextureManager() = default; 

	TextureData& GetTexture(const std::string& key)
	{
		return m_Textures.at(key);
	}

	bool AddTexture(Window& window, const std::string& filepath, const std::string& key)
	{
		TextureData t;
		bool valid = t.Load(window, filepath);
		m_Textures.insert({ key, t });
		return valid;
	}
private:
	std::unordered_map<std::string, TextureData> m_Textures;
};

#endif