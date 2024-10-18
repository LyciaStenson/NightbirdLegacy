#pragma once

#include <Engine.h>

#include <GameRenderTarget.h>

#include <iostream>

class Game
{
public:
	Game();
	~Game();

private:
	Engine* m_Engine;
};