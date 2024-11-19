#version 450

uniform float uTime;              // Time in seconds
uniform vec2 uResolution;         // Screen resolution
uniform vec3 uCameraPosition;     // Camera position in world space
uniform vec3 uCameraForward;      // Camera forward direction (normalized)
uniform vec3 uCameraRight;        // Camera right direction (normalized)
uniform vec3 uCameraUp;           // Camera up direction (normalized)

out vec4 FragColor;

struct Sphere {
	vec3 position;
	float radius;
};

struct Capsule {
	vec3 top;
	vec3 bottom;
	float radius;
};

Sphere spheres[2] = Sphere[](
	Sphere(vec3(3.0, 0.0, 0.0), 1.0),
	Sphere(vec3(-3.0, 0.0, 0.0), 1.0)
);

Capsule capsules[1] = Capsule[](
	Capsule(vec3(3.0, 0.0, 0.0), vec3(-3.0, 0.0, 0.0), 0.3)
);

float sdSphere(vec3 p, Sphere sphere) {
	return length(p - sphere.position) - sphere.radius;
}

float sdCapsule( vec3 p, Capsule capsule ) {
	vec3 pa = p - capsule.top;
	vec3 ba = capsule.bottom - capsule.top;
	float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
	return length(pa - ba * h) - capsule.radius;
}

float smoothMin(float a, float b, float k) {
	float h = max(k - abs(a - b), 0.0) / k;
	return min(a, b) - h * h * h * k * (1.0 / 6.0);
}

float sceneSDF(vec3 p) {
	float minDist = sdSphere(p, spheres[0]);
	for (int i = 1; i < spheres.length(); i++) {
		minDist = smoothMin(minDist, sdSphere(p, spheres[i]), 1.0);
	}
	for (int i = 0; i < capsules.length(); i++) {
		minDist = smoothMin(minDist, sdCapsule(p, capsules[i]), 1.0);
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
	vec3 color = vec3(0.0); // Default to black
	if (dist > 0.0) {
		vec3 hitPoint = uCameraPosition + rayDir * dist;
		vec3 normal = calcNormal(hitPoint);
		
		// Simple lighting
		vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
		float diff = max(dot(normal, lightDir), 0.0);
		color = vec3(0.6, 0.2, 0.5) * diff;
	}
	
	FragColor = vec4(color, 1.0);
}
