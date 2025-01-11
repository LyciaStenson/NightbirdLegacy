#version 460 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

in vec2 baseColorTexCoord;
in vec2 metallicRoughnessTexCoord;

uniform vec4 baseColor;
uniform sampler2D baseColorTexture;
uniform bool hasBaseColorTexture;

uniform vec3 lightPos;

uniform vec3 lightColor;

uniform vec3 viewPos;

void main()
{
	float ambientStrength = 0.2f;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * lightColor;

	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 lighting = (ambient + diffuse + specular);

	vec4 finalBaseColor = hasBaseColorTexture ? texture(baseColorTexture, baseColorTexCoord) : baseColor;

	FragColor = vec4(lighting, 1.0f) * finalBaseColor;
}
