#include "Window.h"
#include "Texture.h"
#include "Util.h"
#include <cassert>

Window::Window() : renderer_(nullptr), window_(nullptr) {}

bool Window::init(const std::string &name, int w, int h) {
  assert((w > 0 && h > 0) &&
         "The window should be greater than 0 in any dimension");
  window_ = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);

  if (window_ == nullptr) {
    GlobalAppState::get().setAppState(AppState::AS_FAIL,
                                      "Couldn't create window! SDL_ERROR: " +
                                          std::string(SDL_GetError()));
    return false;
  }

  renderer_ = SDL_CreateRenderer(
      window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer_ == nullptr) {
    GlobalAppState::get().setAppState(AppState::AS_FAIL,
                                      "Couldn't create renderer! SDL_ERROR: " +
                                          std::string(SDL_GetError()));
    return false;
  }

  return true;
}

Window::~Window() {
  if (window_ != nullptr) {
    SDL_DestroyWindow(window_);
  }

  if (renderer_ != nullptr) {
    SDL_DestroyRenderer(renderer_);
  }

  window_ = nullptr;
  renderer_ = nullptr;
}

void Window::renderTexture(TextureData &tex, Rect *src, FRect *dst,
                           SDL_RendererFlip flip) {
  if (tex.Texture != nullptr) {
    SDL_RenderCopyExF(renderer_, tex.Texture, src, dst, 0.0f, nullptr, flip);
  }
}

void Window::drawRect(FRect &rect) {
  std::uint8_t r, g, b, a;
  SDL_GetRenderDrawColor(renderer_, &r, &g, &b, &a);
  SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 0);
  SDL_RenderDrawRectF(renderer_, &rect);
  SDL_SetRenderDrawColor(renderer_, r, g, b, a);
}
