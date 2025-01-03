#pragma once

#include <future>

#include <TextureLoadComponent.h>

struct CubemapLoadComponent
{
	std::vector<std::shared_future<TextureData>> futures;

	std::vector<bool> loadedChecks = { false, false, false, false, false, false };
};