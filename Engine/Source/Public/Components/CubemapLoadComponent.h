#pragma once

#include <future>

#include <TextureLoadComponent.h>

struct CubemapLoadComponent
{
	std::vector<TextureData> textureData;
	//std::future<TextureData> future;
};