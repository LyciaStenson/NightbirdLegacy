#pragma once

enum class TextureType
{
	BaseColor,
	Normal,
	MetallicRoughness,
	Occlusion
};

struct Texture
{
	unsigned int id;
	TextureType type;
};