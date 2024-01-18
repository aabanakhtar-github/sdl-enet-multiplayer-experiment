#include <chrono>

#include "All.h" 

class GraphicsSystem
{
	MAKE_SINGLETON(GraphicsSystem)
public:
	GraphicsSystem();
	// Render Loop
	void Update(ECS::Scene& scene, float delta = 0.f, bool draw_debug_rects = false);
private:
	std::unique_ptr<Window> m_Window;
	friend class Game;
};