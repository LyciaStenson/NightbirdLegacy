#pragma once

#include <vector>

#include <MeshPrimitive.h>

#include <Shader.h>

struct MeshComponent
{
	std::vector<MeshPrimitive> primitives;
};