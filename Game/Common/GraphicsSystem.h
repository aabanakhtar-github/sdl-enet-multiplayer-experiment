#include <memory>
#include "Util.h"
#include "ECS.h"

class GraphicsSystem : public ECS::ISystem
{
public:
	GraphicsSystem();

	// Render Loop
	virtual void Init(ECS::Scene& scene);
	virtual void Update(ECS::Scene& scene, float delta);
public:
	bool DrawDebugRects;
	std::unique_ptr<class Window> GameWindow;
private:
	void InitMedia();
};