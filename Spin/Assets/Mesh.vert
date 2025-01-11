#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aBaseColorTexCoord;
layout (location = 3) in vec2 aMetallicRoughnessTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;

out vec2 baseColorTexCoord;
out vec2 metallicRoughnessTexCoord;

void main()
{
	Normal = mat3(transpose(inverse(model))) * aNormal;
	FragPos = vec3(model * vec4(aPos, 1.0f));

	baseColorTexCoord = vec2(aBaseColorTexCoord.x, 1.0 - aBaseColorTexCoord.y);
	metallicRoughnessTexCoord = vec2(aMetallicRoughnessTexCoord.x, 1.0 - aMetallicRoughnessTexCoord.y);

	gl_Position = projection * view * vec4(FragPos, 1.0f);
}
