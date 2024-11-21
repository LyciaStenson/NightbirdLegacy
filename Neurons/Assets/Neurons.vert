#version 330 core

layout(location = 0) in vec2 aPosition;

out vec2 vUv;

uniform vec2 uResolution;

void main() {
	// Pass the normalized screen coordinates to the fragment shader
	vUv = (aPosition + 1.0) * 0.5; // Transform from [-1, 1] to [0, 1]

	// Set the position of the vertex in clip space
	gl_Position = vec4(aPosition, 0.0, 1.0); // Set the position for OpenGL
}
