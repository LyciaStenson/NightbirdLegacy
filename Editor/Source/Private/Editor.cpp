#include <Editor.h>

Editor::Editor()
{
	engine = new Engine();
	engine->Init();
	engine->Tick();
	engine->Terminate();
}

Editor::~Editor()
{
	delete engine;
}