#ifndef TEXTURE_H
#define TEXTURE_H

#include <string> 
#include <cstdint> 
#include <unordered_map>

#include "SDL.h" 

class Window;

class TextureData
{
public:
	explicit TextureData(); 
	TextureData(const TextureData&);
	TextureData(TextureData&&) noexcept;
	TextureData& operator = (const TextureData&);
	TextureData& operator = (TextureData&&) noexcept;
	~TextureData();

	bool Load(Window& window, const std::string& filepath);
	// call after making modifications to the surface, reloading the texture basically	
	bool Refresh() {}

	std::uint16_t getWidth() { return m_Width; }
	std::uint16_t getHeight() { return m_Height; }
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
public:
	static TextureManager& Get() 
	{
		static TextureManager instance; 
		return instance;
	}
	
	TextureData& operator [] (const std::string& name)
	{
		return m_Textures.at(name);
	}
	
	void AddTexture(Window& window, const std::string& key, const std::string& filepath);
private:
	TextureManager() {}
	std::unordered_map<std::string, TextureData> m_Textures;
};

#endif
