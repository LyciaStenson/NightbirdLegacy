#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthTexture;

void main()
{
	float depth = texture(depthTexture, TexCoords).r;
	FragColor = vec4(vec3(depth), 1.0f);
}