#pragma once

#include <future>

#include <TextureLoadComponent.h>

struct CubemapLoadComponent
{
	//std::vector<TextureData> textureData;
	std::vector<std::shared_future<TextureData>> futures;

	std::vector<bool> loadedChecks = { false, false, false, false, false, false };
	//unsigned int loadedCount = 0;
};