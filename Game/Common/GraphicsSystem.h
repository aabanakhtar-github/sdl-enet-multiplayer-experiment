#include <memory>
#include "Util.h"
#include "ECS.h"
#include "Window.h"

class GraphicsSystem : public ECS::ISystem {
public:
	GraphicsSystem();

	// Render Loop
	void init(ECS::Scene &scene) override;
	void update(ECS::Scene &scene, float delta) override;
public:
    Window game_window_;
    static constexpr bool draw_debug_rects_ = true;
};