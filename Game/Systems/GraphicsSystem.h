#include <memory>
#include "Util.h"
#include "ECS.h"

class GraphicsSystem : ECS::ISystem
{
	MAKE_SINGLETON(GraphicsSystem)
public:
	GraphicsSystem();

	void InitMedia();

	// Render Loop
	virtual void Init(ECS::Scene& scene);
	virtual void Update(ECS::Scene& scene, float delta);
public:
	bool DrawDebugRects;
public:
	std::unique_ptr<class Window> GameWindow;
};