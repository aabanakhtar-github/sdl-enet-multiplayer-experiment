// entry point
#include "All.h"
#include "Window.h"
#include "Util.h"
#include "Texture.h"
#include "ECS.h"

class c : ECS::Component {
public:
	int x;
};

using namespace ECS;

auto main(int argc, char* argv[]) -> int 

{
	Scene myscene;
	myscene.RegisterComponent<c>();
	auto e = myscene.CreateEntity();
	myscene.AddComponent<c>(e);
	auto e2 = myscene.CreateEntity();
	myscene.AddComponent<c>(e2);
	auto e3 = myscene.CreateEntity();

	auto view = SceneView<c>(myscene).Get();
	for (auto i : view)
	{
		std::cout << i << " ";
	}
	std::this_thread::sleep_for(std::chrono::seconds(5));
	return 0;
}
