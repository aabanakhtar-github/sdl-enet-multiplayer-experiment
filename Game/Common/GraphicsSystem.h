#include <memory>
#include "Util.h"
#include "ECS.h"

class GraphicsSystem : public ECS::ISystem
{
public:
	GraphicsSystem();

	// Render Loop
	virtual void init(ECS::Scene &scene);
	virtual void update(ECS::Scene &scene, float delta);
public:
	bool DrawDebugRects;
	std::unique_ptr<class Window> GameWindow;
private:
	void InitMedia();
};