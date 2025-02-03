#version 460 core

in vec3 FragPos;

out vec4 FragColor;

uniform vec3 color = vec3(0.0f, 0.0f, 1.0f);

void main()
{
	float dist = length(FragPos.xz);
	if (dist > 50.0f)
		discard;
	
	FragColor = vec4(color, 1.0f);
}