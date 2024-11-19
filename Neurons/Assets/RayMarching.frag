#version 450

uniform float uTime;              // Time in seconds
uniform vec2 uResolution;         // Screen resolution
uniform vec3 uCameraPosition;     // Camera position in world space
uniform vec3 uCameraForward;      // Camera forward direction (normalized)
uniform vec3 uCameraRight;        // Camera right direction (normalized)
uniform vec3 uCameraUp;           // Camera up direction (normalized)

out vec4 FragColor;

// Function to define the scene's geometry as signed distance fields (SDF)
float sdfSphere(vec3 p, vec3 center, float radius) {
    return length(p - center) - radius;
}

float sdfPlane(vec3 p, vec3 normal, float d) {
    return dot(p, normal) + d;
}

// Scene SDF
float sceneSDF(vec3 p) {
    // Sphere at the origin with radius 1.0
    float sphere = sdfSphere(p, vec3(0.0, 0.0, 0.0), 1.0);
    // Infinite plane below the sphere
    float plane = sdfPlane(p, vec3(0.0, 1.0, 0.0), 0.5);

    // Return the minimum distance to the scene objects
    return min(sphere, plane);
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
    vec3 rayDir = normalize(uCameraForward + uv.x * uCameraRight + uv.y * uCameraUp);

    // Ray origin
    vec3 rayOrigin = uCameraPosition;

    // Perform ray marching
    float maxDist = 50.0;
    float epsilon = 0.001;
    int maxSteps = 256;
    float dist = rayMarch(rayOrigin, rayDir, maxSteps, maxDist, epsilon);

    // Determine color
    vec3 color = vec3(0.0); // Default to black
    if (dist > 0.0) {
        vec3 hitPoint = rayOrigin + rayDir * dist;
        vec3 normal = calcNormal(hitPoint);

        // Simple lighting
        vec3 lightDir = normalize(vec3(-0.5, 1.0, -0.5));
        float diff = max(dot(normal, lightDir), 0.0);
        color = vec3(0.3, 0.6, 1.0) * diff; // Blue-tinted diffuse color
    }

    FragColor = vec4(color, 1.0);
}
