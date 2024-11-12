#pragma once

#include <future>

#include <TextureLoadComponent.h>

struct CubemapLoadComponent
{
	//std::vector<TextureData> textureData;
	std::vector<std::shared_future<TextureData>> futures;
};