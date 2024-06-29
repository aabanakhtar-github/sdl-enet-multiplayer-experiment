#include "Texture.h"
#include "SDL_image.h"
#include "Util.h"
#include "Window.h"
#include <format>

TextureData::TextureData()
    : height_(0), width_(0), renderer_(nullptr), surface(nullptr),
      texture(nullptr), valid_(false) {}

TextureData::TextureData(const TextureData &other) : TextureData() {
  if (!other.valid_) {
    return;
  }

  renderer_ = other.renderer_;
  surface = SDL_CreateRGBSurfaceFrom(
      other.surface->pixels, other.surface->w, other.surface->h,
      other.surface->format->BitsPerPixel, other.surface->pitch,
      other.surface->format->Rmask, other.surface->format->Gmask,
      other.surface->format->Bmask, other.surface->format->Amask);

  if (surface == nullptr) {
    valid_ = false;
    return;
  }

  texture = SDL_CreateTextureFromSurface(renderer_, surface);
  if (texture == nullptr) {
    valid_ = false;
    return;
  }

  width_ = surface->w;
  height_ = surface->h;
  valid_ = true;
}

TextureData::TextureData(TextureData &&other) noexcept : TextureData() {
  swap(*this, other);
}

TextureData &TextureData::operator=(TextureData &&other) noexcept {
  swap(*this, other);
  return *this;
}

TextureData &TextureData::operator=(TextureData other) {
  swap(*this, other);
  return *this;
}

bool TextureData::load(Window &window, const std::string &filepath) {
  if (surface != nullptr) {
    SDL_FreeSurface(surface);
  }

  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
  }

  valid_ = false;
  renderer_ = window.renderer_;
  surface = IMG_Load(filepath.c_str());

  if (surface == nullptr) {
    valid_ = false;
    return false;
  }

  SDL_Surface *window_surface = SDL_GetWindowSurface(window.window_);
  // set the image to use the right screen format for rendering performance
  SDL_Surface *optimized =
      SDL_ConvertSurface(surface, window_surface->format, 0);

  if (optimized == nullptr) {
    valid_ = false;
    return false;
  }

  SDL_FreeSurface(surface);
  surface = optimized;
  // set color key
  SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 255, 255));

  texture = SDL_CreateTextureFromSurface(window.renderer_, surface);

  if (texture == nullptr) {
    valid_ = false;
    return false;
  }

  width_ = surface->w;
  height_ = surface->h;
  valid_ = true;
  return true;
}

// bro does this even work
bool TextureData::refresh() {
  if (texture == nullptr || surface == nullptr) {
    return false;
  }

  SDL_DestroyTexture(texture);
  texture = SDL_CreateTextureFromSurface(renderer_, surface);

  if (texture == nullptr) {
    valid_ = false;
    return false;
  }

  valid_ = true;
  return true;
}

TextureData::~TextureData() {
  if (surface != nullptr) {
    SDL_FreeSurface(surface);
  }

  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
  }

  surface = nullptr;
  texture = nullptr;
}

TextureData &TextureManager::getTexture(const std::string &key) {
  return textures_[key];
}

bool TextureManager::addTexture(Window &window, const std::string &filepath,
                                const std::string &key) {
  TextureData t;
  if (!t.load(window, filepath)) {
    return false;
  }

  textures_.insert({key, std::move(t)});
  return true;
}
