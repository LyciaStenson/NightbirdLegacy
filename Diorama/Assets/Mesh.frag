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

//uniform vec3 lightPos;

//uniform vec3 lightColor;

uniform vec3 viewPos;

struct DirectionalLight
{
	vec3 direction;
	float intensity;
};

uniform DirectionalLight directionalLight;

void main()
{
	vec3 ambient = vec3(1.0f, 1.0f, 1.0f) * 0.01f;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-directionalLight.direction);
	//vec3 lightDir = normalize(-vec3(0.0f, 1.0f, 0.0f));
	//vec3 lightDir = normalize(lightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);// * lightColor;
	
	//float specularStrength = 0.5f;
	//vec3 viewDir = normalize(viewPos - FragPos);
	//vec3 reflectDir = reflect(-lightDir, norm);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	//vec3 specular = specularStrength * spec * lightColor;

	//vec3 lighting = (ambient + diffuse + specular);
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

	vec4 normalAsColor = vec4(0.9f, 0.1f, 0.1f, 1.0f);
	if (hasNormalTexture)
	{
		normalAsColor = texture(normalTexture, normalTexCoord);
	}
	
	FragColor = vec4(lighting, 1.0f) * finalBaseColor;
	//FragColor = finalBaseColor;
	//FragColor = vec4(ambientOcclusion, ambientOcclusion, ambientOcclusion, 1.0f);
	//FragColor = vec4(finalMetallic, finalMetallic, finalMetallic, 1.0f);
	//FragColor = vec4(finalRoughness, finalRoughness, finalRoughness, 1.0f);
	//FragColor = normalAsColor;
}
