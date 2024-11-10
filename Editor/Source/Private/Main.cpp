#include <Engine.h>
#include <EditorRenderTarget.h>

int main()
{
	int WIDTH = 1920;
	int HEIGHT = 1080;

	EditorRenderTarget* renderTarget = new EditorRenderTarget();
	Engine engine = Engine(WIDTH, HEIGHT, "Editor", renderTarget);

	engine.Init();

	engine.InitSystems();
	engine.MainLoop();
	engine.Terminate();
	delete renderTarget;
	return 0;
}