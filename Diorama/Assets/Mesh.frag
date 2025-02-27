#version 460 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 ViewPos;

in vec3 DirectionalLightDir;
in vec4 FragPosLightSpace;

#define MAX_POINT_LIGHTS 16
in vec3 PointLightPositions[MAX_POINT_LIGHTS];

in vec2 baseColorTexCoord;
in vec2 metallicRoughnessTexCoord;
in vec2 normalTexCoord;

uniform vec4 baseColorFactor;
uniform sampler2D baseColorTexture;
uniform bool hasBaseColorTexture;

uniform float metallicFactor = 1.0f;
uniform float roughnessFactor = 1.0f;
uniform sampler2D metallicRoughnessTexture;
uniform bool hasMetallicRoughnessTexture;

uniform sampler2D normalTexture;
uniform bool hasNormalTexture;

struct DirectionalLight
{
	float intensity;
	float ambient;
	vec3 color;
};

struct PointLight
{
	float intensity;
	vec3 color;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

uniform DirectionalLight directionalLight;
uniform sampler2DShadow directionalLightShadowMap;

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

uniform bool useShadowMapping;

float CalculateDirectionalShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5f + 0.5f;
	if (projCoords.z > 1.0f)
		return 0.0f;
	
	float bias = max(0.01 * (1.0f - dot(normal, lightDir)), 0.001f);
	
	float shadow = 0.0f;
	vec2 texelSize = 1.0f / textureSize(directionalLightShadowMap, 0);
	int samples = 3;
	for (int x = -samples / 2; x <= samples / 2; ++x)
	{
		for (int y = -samples / 2; y <= samples / 2; ++y)
		{
			vec2 offset = vec2(x, y) * texelSize;
			shadow += texture(directionalLightShadowMap, vec3(projCoords.xy + offset, projCoords.z - bias));
		}
	}
	return 1.0f - shadow / (samples * samples);
}

void main()
{
	vec4 finalBaseColor = hasBaseColorTexture ? texture(baseColorTexture, baseColorTexCoord) : baseColorFactor;
	if (finalBaseColor.a < 0.1)
	{
		discard;
	}

	vec3 ambient = vec3(1.0f, 1.0f, 1.0f) * directionalLight.ambient;
	
	vec3 normal = hasNormalTexture ? normalize((texture(normalTexture, normalTexCoord).rgb * 2.0f) - 1.0f) : normalize(Normal);
	
	vec3 directionalLightDir = normalize(-DirectionalLightDir);
	
	vec3 directionalDiffuse = directionalLight.color * max(dot(normal, directionalLightDir), 0.0f) * directionalLight.intensity;
	
	//float specularStrength = 0.5f;
	//vec3 viewDir = normalize(ViewPos - FragPos);
	//vec3 reflectDir = reflect(-directionalLightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	//vec3 specular = specularStrength * spec * lightColor;

	vec3 pointDiffuse = vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < pointLightCount; i++)
	{
		vec3 pointLightDir = PointLightPositions[i] - FragPos;
		float dist = length(pointLightDir);
		pointLightDir = normalize(pointLightDir);
		float attenuation = 1.0f / (pointLights[i].constantAttenuation + pointLights[i].linearAttenuation * dist + pointLights[i].quadraticAttenuation * dist * dist);
		pointDiffuse += pointLights[i].color * max(dot(normal, pointLightDir), 0.0f) * pointLights[i].intensity * attenuation;
	}
	
	float finalMetallic = metallicFactor;
	float finalRoughness = roughnessFactor;
	float ambientOcclusion = 1.0f;

	if (hasMetallicRoughnessTexture)
	{
		vec4 metallicRoughnessData = texture(metallicRoughnessTexture, metallicRoughnessTexCoord);
		ambientOcclusion = metallicRoughnessData.r;
		finalMetallic = metallicRoughnessData.b;
		finalRoughness = metallicRoughnessData.g;
	}
	
	if (useShadowMapping)
	{
		float directionalShadow = CalculateDirectionalShadow(FragPosLightSpace, normal, directionalLightDir);
		directionalDiffuse *= (1.0f - directionalShadow);
	}
	
	vec3 diffuse = (directionalDiffuse + pointDiffuse);
	
	vec3 lighting = ambient + diffuse;
	
	FragColor = vec4(lighting, 1.0f) * finalBaseColor;

	//FragColor = vec4(normal, 1.0f);
	//FragColor = finalBaseColor;
	//FragColor = vec4(ambientOcclusion, ambientOcclusion, ambientOcclusion, 1.0f);
	//FragColor = vec4(finalMetallic, finalMetallic, finalMetallic, 1.0f);
	//FragColor = vec4(finalRoughness, finalRoughness, finalRoughness, 1.0f);
}
