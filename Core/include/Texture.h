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

	int GetWidth() const { return  m_Width; }
	int GetHeight() const { return m_Height; }
	bool GetValid() const { return m_Valid; }
public: 
	SDL_Surface* Surface; 
	SDL_Texture* Texture;

private:
	int m_Width;
	int m_Height;
	SDL_Renderer* m_Renderer;
	bool m_Valid;
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

		if (!valid) 
		{
			return valid;
		}

		m_Textures.insert({ key, t });
		return valid;
	}
private:
	std::unordered_map<std::string, TextureData> m_Textures;
};

#endif