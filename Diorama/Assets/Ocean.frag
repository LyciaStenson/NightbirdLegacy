#version 460 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec4 color;

struct DirectionalLight
{
	vec3 direction;
	float intensity;
	float ambient;
	vec3 color;
};

uniform DirectionalLight directionalLight;

void main()
{
	float dist = length(FragPos.xz);
	if (dist > 25.0f)
		discard;
	
	vec3 normal = normalize(Normal);
	
	vec3 directionalDiffuse = directionalLight.color * max(dot(normal, -directionalLight.direction), 0.0f) * directionalLight.intensity;
	
	vec3 ambient = directionalLight.ambient * vec3(1.0f, 1.0f, 1.0f);

	//vec4 fragColor = (ambient + directionalDiffuse) * color;
	//FragColor = (ambient + directionalDiffuse) * color;
	FragColor = color;

	//FragColor = vec4(fragColor, 1.0f);
	//FragColor = vec4(Normal, 1.0f);
}