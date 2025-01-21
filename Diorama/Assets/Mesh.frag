#version 460 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

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

uniform vec3 viewPos;

struct DirectionalLight
{
	vec3 direction;
	float intensity;
	float ambient;
	vec3 color;
};

struct PointLight
{
	vec3 position;
	float intensity;
	vec3 color;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

uniform DirectionalLight directionalLight;

#define MAX_POINT_LIGHTS 16
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

void main()
{
	vec3 ambient = vec3(1.0f, 1.0f, 1.0f) * directionalLight.ambient;
	
	vec3 norm = normalize(Normal);
	vec3 directionalLightDir = normalize(-directionalLight.direction);
	
	//float directionalDiffuse = max(dot(norm, directionalLightDir), 0.0f) * directionalLight.intensity;
	vec3 directionalDiffuse = directionalLight.color * max(dot(norm, directionalLightDir), 0.0f) * directionalLight.intensity;
	
	//float specularStrength = 0.5f;
	//vec3 viewDir = normalize(viewPos - FragPos);
	//vec3 reflectDir = reflect(-directionalLightDir, norm);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	//vec3 specular = specularStrength * spec * lightColor;

	vec3 pointDiffuse = vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < pointLightCount; i++)
	{
		vec3 pointLightDir = pointLights[i].position - FragPos;
		float dist = length(pointLightDir);
		pointLightDir = normalize(pointLightDir);
		//float attenuation = 1.0f / ((dist * dist) + 0.001f);
		float attenuation = 1.0f / (pointLights[i].constantAttenuation + pointLights[i].linearAttenuation * dist + pointLights[i].quadraticAttenuation * dist * dist);
		pointDiffuse += pointLights[i].color * max(dot(norm, pointLightDir), 0.0f) * pointLights[i].intensity * attenuation;
	}

	vec3 diffuse = (directionalDiffuse + pointDiffuse);

	vec3 lighting = ambient + diffuse;
	
	vec4 finalBaseColor = hasBaseColorTexture ? texture(baseColorTexture, baseColorTexCoord) : baseColorFactor;

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

	//vec4 normalAsColor = vec4(0.9f, 0.1f, 0.1f, 1.0f);
	//if (hasNormalTexture)
	//{
		//normalAsColor = texture(normalTexture, normalTexCoord);
	//}
	
	FragColor = vec4(lighting, 1.0f) * finalBaseColor;
	//FragColor = finalBaseColor;
	//FragColor = vec4(ambientOcclusion, ambientOcclusion, ambientOcclusion, 1.0f);
	//FragColor = vec4(finalMetallic, finalMetallic, finalMetallic, 1.0f);
	//FragColor = vec4(finalRoughness, finalRoughness, finalRoughness, 1.0f);
	//FragColor = normalAsColor;
}
