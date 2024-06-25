#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "ECS.h"
#include "Camera.h"
#include "Window.h"

class Camera;
class TextureManager;

class GraphicsSystem : public ECS::ISystem {
public:
	GraphicsSystem();

	void init(ECS::Scene &scene) override;
	void update(ECS::Scene &scene, float delta) override;
public:
    Camera camera;
private:
    void drawNormalEntities(ECS::Scene& scene, TextureManager& manager, const std::vector<ECS::EntityID>& IDs);
    void drawPhysicsEntities(ECS::Scene& scene, TextureManager& manager, const std::vector<ECS::EntityID>& IDs);

    // Texture groups are basically collections of textures in a "Group" 
    // so you don't "need" 3 texture components to make a singular platform
    void drawGroupPhysicsEntities(ECS::Scene& scene, TextureManager& manager, const std::vector<ECS::EntityID>& IDs);
private:
    Window game_window_;
    static constexpr bool draw_debug_rects_ = true;
};

#endif // GRAPHICS_SYSTEM_H
