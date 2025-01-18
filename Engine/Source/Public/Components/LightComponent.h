#pragma once

struct LightComponent
{
	float intensity;
};

struct DirectionalLightComponent
{
	float ambient;
};

struct PointLightComponent
{
	// This value will be used for optimisation
	float range;
};