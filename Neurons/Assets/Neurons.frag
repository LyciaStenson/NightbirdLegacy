#version 460

uniform float uTime;			// Time in seconds
uniform vec2 uResolution;		// Screen resolution
uniform vec3 uCameraPosition;	// Camera position in world space
uniform vec3 uCameraForward;	// Camera forward direction (normalized)
uniform vec3 uCameraRight;		// Camera right direction (normalized)
uniform vec3 uCameraUp;			// Camera up direction (normalized)

uniform float uNeuronPositions[99];
uniform int uNeuronPositionsSize;

uniform int uConnections[99];
uniform int uConnectionsSize;

out vec4 FragColor;

float sdSphere(vec3 p, vec3 position, float radius) {
	return length(p - position) - radius;
}

float sdCapsule( vec3 p, vec3 a, vec3 b, float r ) {
	vec3 pa = p - a, ba = b - a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	return length( pa - ba*h ) - r;
}

float smoothMin(float a, float b, float k) {
	float h = max(k - abs(a - b), 0.0) / k;
	return min(a, b) - h * h * h * k * (1.0 / 6.0);
}

float sceneSDF(vec3 p) {
	vec3 neuronPos = vec3(uNeuronPositions[0], uNeuronPositions[1], uNeuronPositions[2]);
	float minDist = sdSphere(p, neuronPos, 1.0);
	for (int i = 3; i < uNeuronPositionsSize; i += 3) {
		neuronPos = vec3(uNeuronPositions[i], uNeuronPositions[i+1], uNeuronPositions[i+2]);
		minDist = smoothMin(minDist, sdSphere(p, neuronPos, 1.0), 0.7);
	}
	for (int i = 0; i <= uConnectionsSize - 2; i += 2) {
		int topIndex = uConnections[i] * 3;
		int bottomIndex = uConnections[i+1] * 3;
		vec3 top = vec3(uNeuronPositions[topIndex], uNeuronPositions[topIndex+1], uNeuronPositions[topIndex+2]);
		vec3 bottom = vec3(uNeuronPositions[bottomIndex], uNeuronPositions[bottomIndex+1], uNeuronPositions[bottomIndex+2]);
		minDist = smoothMin(minDist, sdCapsule(p, top, bottom, 0.1), 0.7);
	}
	return minDist;
}

// Simple gradient-based normal calculation for shading
vec3 calcNormal(vec3 p) {
	float epsilon = 0.001;
	vec3 dx = vec3(epsilon, 0.0, 0.0);
	vec3 dy = vec3(0.0, epsilon, 0.0);
	vec3 dz = vec3(0.0, 0.0, epsilon);

	float nx = sceneSDF(p + dx) - sceneSDF(p - dx);
	float ny = sceneSDF(p + dy) - sceneSDF(p - dy);
	float nz = sceneSDF(p + dz) - sceneSDF(p - dz);

	return normalize(vec3(nx, ny, nz));
}

// Ray marching function
float rayMarch(vec3 ro, vec3 rd, int maxSteps, float maxDist, float epsilon) {
	float dist = 0.0;
	for (int i = 0; i < maxSteps; i++) {
		vec3 p = ro + rd * dist;
		float d = sceneSDF(p);
		if (d < epsilon) {
			return dist;
		}
		if (dist > maxDist) {
			return -1.0; // Indicates no hit
		}
		dist += d;
	}
	return -1.0; // Indicates no hit
}

void main() {
	// Compute ray direction
	vec2 uv = (gl_FragCoord.xy / uResolution - 0.5) * 2.0;
	uv.x *= uResolution.x / uResolution.y; // Adjust for aspect ratio
	vec3 rayDir = normalize(uCameraForward + uv.x * 0.6 * uCameraRight + uv.y * 0.6 * uCameraUp);

	// Perform ray marching
	float maxDist = 1000.0;
	float epsilon = 0.001;
	int maxSteps = 512;
	float dist = rayMarch(uCameraPosition, rayDir, maxSteps, maxDist, epsilon);

	// Determine color
	vec3 color = vec3(0.025, 0.0, 0.01); // Background color
	if (dist > 0.0) {
		vec3 hitPoint = uCameraPosition + rayDir * dist;
		vec3 normal = calcNormal(hitPoint);

		// Simple lighting
		vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
		float diffuse = max(dot(normal, lightDir), 0.2);
		color = normalize(normalize(hitPoint) * 0.5 + 0.5) * diffuse;
	}

	FragColor = vec4(color, 1.0);
}
