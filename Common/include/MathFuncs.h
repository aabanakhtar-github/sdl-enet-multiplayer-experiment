#ifndef MATH_FUNCS_H
#define MATH_FUNCS_H

#include "Window.h"
#include <random>
#include <type_traits>

struct Vector2 {
  float x = 0.0, y = 0.0;

  constexpr auto operator<=>(const Vector2 &other) const = default;

  constexpr friend Vector2 operator+(const Vector2 &a, const Vector2 &other) {
    return {a.x + other.x, a.y + other.y};
  }

  constexpr Vector2 &operator+=(const Vector2 &other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  constexpr friend Vector2 operator-(const Vector2 &a, const Vector2 &b) {
    return {a.x - b.x, a.y - b.y};
  }

  constexpr Vector2 &operator-=(const Vector2 &b) {
    x -= b.x;
    y -= b.y;
    return *this;
  }

  constexpr friend Vector2 operator*(const Vector2 &v, const float scalar) {
    return {v.x * scalar, v.y * scalar};
  }

  constexpr friend bool operator==(const Vector2 &a, const Vector2 &b) {
    return a.x == b.x && b.y == a.y;
  }
};

inline bool isColliding(const FRect &a, const FRect &b) {
  if (a.x < b.x + b.w // is the left side before the right side of b?
      && a.x + a.w > b.x && a.y < b.y + b.h &&
      a.y + a.h > b.y // is the right side after the left side of b?
  ) {
    return true;
  }

  return false;
}

inline bool isColliding(const Rect &a, const Rect &b) {
  if (a.x < b.x + b.w // is the left side before the right side of b?
      && a.x + a.w > b.x && a.y < b.y + b.h &&
      a.y + a.h > b.y // is the right side after the left side of b?
  ) {
    return true;
  }

  return false;
}

inline bool operator==(const Rect &a, const Rect &b) {
  return a.x == b.x && a.w == b.w && a.y == b.y && a.h == b.h;
}

inline bool operator==(const FRect &a, const FRect &b) {
  return a.x == b.x && a.w == b.w && a.y == b.y && a.h == b.h;
}

inline int random(int min, int max) {
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution distribution(min, max);
  return distribution(engine);
}

#endif // MATH_FUNCS_H
