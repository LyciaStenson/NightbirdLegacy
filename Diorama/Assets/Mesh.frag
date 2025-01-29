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
uniform sampler2D directionalLightShadowMap;

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

uniform bool useShadowMapping;

float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5f + 0.5f;
	if (projCoords.z > 1.0f)
		return 0.0f;
	
	float closestDepth = texture(directionalLightShadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	
	float bias = max(0.0005 * (1.0f - dot(normal, lightDir)), 0.0005f);
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
	return shadow;
}

void main()
{
	vec3 ambient = vec3(1.0f, 1.0f, 1.0f) * directionalLight.ambient;
	
	vec3 normal;

	if (hasNormalTexture)
	{
		normal = texture(normalTexture, normalTexCoord).rgb;
		normal = normalize((normal * 2.0f) - 1.0f);
	}
	else
	{
		normal = normalize(Normal);
	}
	
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
	
	vec4 finalBaseColor = hasBaseColorTexture ? texture(baseColorTexture, baseColorTexCoord) : baseColorFactor;

	float finalMetallic = 0.0f;
	//float finalMetallic = metallicFactor;
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
		float directionalLightShadow = CalculateShadow(FragPosLightSpace, normal, directionalLightDir);
		directionalDiffuse *= (1.0f - directionalLightShadow);
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
