#include <memory>
#include "Util.h"
#include "ECS.h"
#include "Camera.h"
#include "Window.h"

class Camera;

class GraphicsSystem : public ECS::ISystem {
public:
	GraphicsSystem();

	// Render Loop
	void init(ECS::Scene &scene) override;
	void update(ECS::Scene &scene, float delta) override;

public:
    Camera camera;

private:
    Window game_window_;
    static constexpr bool draw_debug_rects_ = true;
};