#ifndef TEXTURE_H
#define TEXTURE_H

#include "SDL.h"
#include "Util.h"
#include <cstdint>
#include <string>
#include <unordered_map>

class Window;

// TODO: member cleanup
class TextureData {
public:
  TextureData();
  TextureData(const TextureData &);
  TextureData(TextureData &&) noexcept;
  TextureData &operator=(TextureData);
  TextureData &operator=(TextureData &&) noexcept;
  ~TextureData();

  bool load(Window &window, const std::string &filepath);
  // TODO: never tested
  bool refresh();
  // cool feature
  friend void swap(TextureData &a, TextureData &b) {
    using std::swap;
    swap(a.Surface, b.Surface);
    swap(a.Texture, b.Texture);
    swap(a.m_Width, b.m_Width);
    swap(a.m_Height, b.m_Height);
    swap(a.m_Renderer, b.m_Renderer);
    swap(a.m_Valid, b.m_Valid);
  }

  [[nodiscard]] int getWidth() const { return m_Width; }
  [[nodiscard]] int getHeight() const { return m_Height; }
  [[nodiscard]] bool getValid() const { return m_Valid; }

public:
  SDL_Surface *Surface;
  SDL_Texture *Texture;

private:
  int m_Width;
  int m_Height;
  SDL_Renderer *m_Renderer;
  bool m_Valid;
};

class TextureManager {
  MAKE_SINGLETON(TextureManager)

public:
  TextureManager() : textures_() {}

  [[nodiscard]] TextureData &getTexture(const std::string &key);
  bool addTexture(Window &window, const std::string &filepath,
                  const std::string &key);

private:
  std::unordered_map<std::string, TextureData> textures_;
};

#endif
