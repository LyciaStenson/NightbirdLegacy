#pragma once

#include <future>

struct TextureData
{
	unsigned char* data;
	int width;
	int height;
};

struct TextureLoadComponent
{
	//TextureData textureData;
	std::future<TextureData> future;
};