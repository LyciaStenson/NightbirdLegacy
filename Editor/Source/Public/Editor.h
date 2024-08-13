#pragma once

#include <Engine.h>

#include <GameRenderTarget.h>
#include <EditorRenderTarget.h>

#include <iostream>

class Editor
{
public:
	Editor();
	~Editor();

private:
	Engine* engine;
};