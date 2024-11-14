#version 460

// Uniforms
uniform float uTime;              // Time in seconds
uniform vec2 uResolution;         // Screen resolution
uniform vec3 uCameraPosition;     // Camera position in world space
uniform vec3 uCameraForward;      // Camera forward direction (normalized)
uniform vec3 uCameraRight;        // Camera right direction (normalized)
uniform vec3 uCameraUp;           // Camera up direction (normalized)

// Function to create a sphere's signed distance field (SDF)
float sphereSDF(vec3 p, float radius) {
    return length(p) - radius;
}

// Function to calculate the distance to the closest object (sphere in this case)
float sceneSDF(vec3 p) {
    return sphereSDF(p, 1.0); // Sphere of radius 1.0 at the origin
}

// Ray marching function to determine the distance along the ray
float rayMarch(vec3 ro, vec3 rd) {
    float dist = 0.0;
    const int MAX_STEPS = 100;
    const float MAX_DIST = 100.0;
    const float SURF_DIST = 0.001;

    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 p = ro + rd * dist;
        float d = sceneSDF(p);
        if (d < SURF_DIST) return dist;
        if (dist > MAX_DIST) break;
        dist += d;
    }

    return MAX_DIST;
}

// Function for simple lighting based on normal direction
vec3 getNormal(vec3 p) {
    const vec2 e = vec2(0.001, 0.0);
    return normalize(vec3(
        sceneSDF(p + e.xyy) - sceneSDF(p - e.xyy),
        sceneSDF(p + e.yxy) - sceneSDF(p - e.yxy),
        sceneSDF(p + e.yyx) - sceneSDF(p - e.yyx)
    ));
}

// Main fragment shader
out vec4 FragColor;

void main() {
    // Normalize pixel coordinates based on OpenGL screen space (origin at bottom-left)
    vec2 uv = (gl_FragCoord.xy) / uResolution.xy;
    uv = uv * 2.0 - 1.0;  // Mapping to range [-1, 1]
    uv.x *= uResolution.x / uResolution.y;  // Correct aspect ratio

    // Camera ray origin and direction
    vec3 ro = uCameraPosition;  // Camera position
    vec3 rd = normalize(
        uv.x * uCameraRight +  // Right direction (camera's horizontal orientation)
        uv.y * uCameraUp +     // Up direction (camera's vertical orientation)
        uCameraForward         // Forward direction (viewing direction)
    );

    // Ray march to find the intersection distance
    float dist = rayMarch(ro, rd);
    
    // Calculate color based on distance
    if (dist < 100.0) {
        FragColor = vec4(1.0);  // Render white color (surface color)
    } else {
        FragColor = vec4(0.0);  // Render black (background color)
    }
}