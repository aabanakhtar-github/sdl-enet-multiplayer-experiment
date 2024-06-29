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
    swap(a.surface, b.surface);
    swap(a.texture, b.texture);
    swap(a.width_, b.width_);
    swap(a.height_, b.height_);
    swap(a.renderer_, b.renderer_);
    swap(a.valid_, b.valid_);
  }

  [[nodiscard]] int getWidth() const { return width_; }
  [[nodiscard]] int getHeight() const { return height_; }
  [[nodiscard]] bool getValid() const { return valid_; }

public:
  SDL_Surface *surface;
  SDL_Texture *texture;

private:
  int width_;
  int height_;
  SDL_Renderer *renderer_;
  bool valid_;
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
