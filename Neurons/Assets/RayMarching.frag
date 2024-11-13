#version 460

// Uniforms
uniform float uTime;              // Time in seconds
uniform vec2 uResolution;         // Screen resolution
uniform vec3 uCameraPosition;     // Camera position in world space

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

vec3 getLighting(vec3 p, vec3 lightPos) {
    vec3 normal = getNormal(p);
    vec3 lightDir = normalize(lightPos - p);
    float diff = max(dot(normal, lightDir), 0.0);
    return vec3(1.0, 0.8, 0.6) * diff; // Color and intensity of the light
}

// Main fragment shader
out vec4 FragColor;

void main() {
    // Normalized pixel coordinates (centered and scaled for aspect ratio)
    vec2 uv = (gl_FragCoord.xy - uResolution.xy * 0.5) / uResolution.y;

    // Use the uniform camera position and set up the ray direction
    vec3 ro = uCameraPosition;       // Camera position from uniform
    vec3 rd = normalize(vec3(uv, -1.0)); // Camera direction (along -z, can be adjusted for other directions)

    // Ray march to find the intersection distance
    float dist = rayMarch(ro, rd);
    
    // Calculate color based on distance
    if (dist < 100.0) {
        vec3 p = ro + rd * dist;          // Point on the surface
        vec3 color = getLighting(p, vec3(3.0 * sin(uTime), 3.0, 3.0 * cos(uTime))); // Moving light position
        FragColor = vec4(color, 1.0);      // Render the color with alpha 1.0
    } else {
        FragColor = vec4(0.0);             // Background color (black)
    }
}
