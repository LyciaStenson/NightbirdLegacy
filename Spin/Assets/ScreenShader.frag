#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
	vec3 col = texture(screenTexture, TexCoords).rgb;
	FragColor = vec4(col, 1.0f);

	float gamma = 2.2f;
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0f/gamma));
}