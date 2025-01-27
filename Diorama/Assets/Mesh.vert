#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aTangent;
layout (location = 3) in vec2 aBaseColorTexCoord;
layout (location = 4) in vec2 aMetallicRoughnessTexCoord;
layout (location = 5) in vec2 aNormalTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

uniform bool hasNormalTexture;
uniform int tangentHandedness;

uniform sampler2D directionalLightShadowMap;
uniform vec3 directionalLightDir;
uniform mat4 lightSpaceMat;

#define MAX_POINT_LIGHTS 16
uniform vec3 pointLightPositions[MAX_POINT_LIGHTS];
uniform int pointLightCount;

out vec3 FragPos;
out vec3 Normal;

out vec3 ViewPos;

out vec3 DirectionalLightDir;
out vec4 FragPosLightSpace;

out vec3 PointLightPositions[MAX_POINT_LIGHTS];

out vec2 baseColorTexCoord;
out vec2 metallicRoughnessTexCoord;
out vec2 normalTexCoord;

void main()
{
	vec3 fragPos = vec3(model * vec4(aPos, 1.0f));

	mat3 normalMat = transpose(inverse(mat3(model)));
	vec3 normal = normalize(normalMat * aNormal);
	vec3 tangent = normalize(normalMat * aTangent.xyz);
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = cross(normal, tangent) * aTangent.w;

	mat3 tbn = transpose(mat3(tangent, bitangent, normal));

	FragPos = hasNormalTexture ? tbn * fragPos : fragPos;

	ViewPos = hasNormalTexture ? tbn * viewPos : viewPos;

	DirectionalLightDir = hasNormalTexture ? tbn * directionalLightDir : directionalLightDir;

	if (hasNormalTexture)
	{
		for (int i = 0; i < pointLightCount && i < MAX_POINT_LIGHTS; i++)
		{
			PointLightPositions[i] = tbn * pointLightPositions[i];
		}
	}
	else
	{
		PointLightPositions = pointLightPositions;
	}
	
	Normal = mat3(transpose(inverse(model))) * aNormal;

	baseColorTexCoord = vec2(aBaseColorTexCoord.x, aBaseColorTexCoord.y);
	metallicRoughnessTexCoord = vec2(aMetallicRoughnessTexCoord.x, aMetallicRoughnessTexCoord.y);
	normalTexCoord = vec2(aNormalTexCoord.x, aNormalTexCoord.y);
	
	FragPosLightSpace = lightSpaceMat * vec4(fragPos, 1.0f);

	gl_Position = projection * view * vec4(fragPos, 1.0f);
}
