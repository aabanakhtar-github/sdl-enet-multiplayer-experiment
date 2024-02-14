#include <memory>
#include "Util.h"
#include "ECS.h"

class GraphicsSystem
{
	MAKE_SINGLETON(GraphicsSystem)
public:
	GraphicsSystem();

	void InitMedia();

	// Render Loop
	void Update(ECS::Scene& scene, float delta = 0.f, bool draw_debug_rects = false);
public:
	std::unique_ptr<class Window> GameWindow;
};