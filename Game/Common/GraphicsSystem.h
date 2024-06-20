#include <memory>
#include "Util.h"
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
    void drawGroupPhysicsEntities(ECS::Scene& scene, TextureManager& manager, const std::vector<ECS::EntityID>& IDs);
private:
    Window game_window_;
    static constexpr bool draw_debug_rects_ = true;
};