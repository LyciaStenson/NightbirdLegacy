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

struct Box {
	vec3 position;
	vec3 size;
};

Sphere spheres[2] = Sphere[](
	Sphere(vec3(-3.0, 0.0, 0.0), 1.0),
	Sphere(vec3(3.0, 0.0, 0.0), 1.0)
);

Box boxes[1] = Box[](
	Box(vec3(0.0, 5.0, 0.0), vec3(1.0))
);

float sdfSphere(vec3 p, Sphere sphere) {
	return length(p - sphere.position) - sphere.radius;
}

float sdfBox(vec3 p, Box box) {
	vec3 q = abs(p) - box.size;
	return length(max(q, vec3(0.0))) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sdfPlane(vec3 p, vec3 normal, float d) {
	return dot(p, normal) + d;
}

float sceneSDF(vec3 p) {
	// Return the minimum distance to the scene objects
	float minDist = sdfSphere(p, spheres[0]);
	for (int i = 0; i < spheres.length(); i++) {
		minDist = min(minDist, sdfSphere(p, spheres[i]));
	}
	for (int i = 0; i < boxes.length(); i++) {
		minDist = min(minDist, sdfBox(p, boxes[i]));
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
		color = vec3(0.3, 0.6, 1.0) * diff; // Blue-tinted diffuse color
	}
	
	FragColor = vec4(color, 1.0);
}
