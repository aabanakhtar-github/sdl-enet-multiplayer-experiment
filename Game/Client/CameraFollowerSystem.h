#ifndef CAMERA_FOLLOWER_SYSTEM_H
#define CAMERA_FOLLOWER_SYSTEM_H

#include "ECS.h"

// System to follow the player based on their position
class CameraFollowerSystem : public ECS::ISystem {
public:
    CameraFollowerSystem() = default;

    void init(ECS::Scene& scene) override;
    void update(ECS::Scene& scene, float delta) override;
};

#endif // CAMERA_FOLLOWER_SYSTEM_H