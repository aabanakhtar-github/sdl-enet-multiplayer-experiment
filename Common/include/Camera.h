#ifndef CAMERA_TRANSFORM_H
#define CAMERA_TRANSFORM_H

#include "MathFuncs.h"

// util class for transforming cartesian coordinates to screen coordinates
class Camera {
public:
  Camera() = default;

  // get the world position of the camera
  [[nodiscard]] Vector2 getPosition() { return offset_; }

  // transform world to screen coordinates
  [[nodiscard]] Vector2 transform(Vector2 position) const {
    position.x = origin_.x + position.x - offset_.x;
    position.y = origin_.y - position.y + offset_.y;
    return position;
  }

  // transform screen to world coordinates
  [[nodiscard]] Vector2 inverseTransform(Vector2 position) const {
    position.x = position.x - origin_.x + offset_.x;
    position.y = origin_.y - position.y - offset_.y;
    return position;
  }

  // set position
  void setOffset(const Vector2 &position) { offset_ = position; }

private:
  // 900 x 600
  Vector2 origin_{450.0f, 300.0f};
  Vector2 offset_{0.0f, 0.0f};
};

#endif // CAMERA_TRANSFORM_H
