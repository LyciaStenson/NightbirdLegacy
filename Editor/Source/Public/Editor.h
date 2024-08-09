#pragma once

#include <Engine.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

class Editor
{
public:
	Editor();
	~Editor();

private:
	Engine* engine;
};