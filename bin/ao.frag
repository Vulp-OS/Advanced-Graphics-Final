#version 430 core

// Samplers for pre-rendered color, normal and depth
layout (binding = 0) uniform sampler2D sColor;
layout (binding = 1) uniform sampler2D sNormalDepth;

// Final output
layout (location = 0) out vec4 color;

// Various uniforms controling SSAO effect
uniform float ssao_radius;
uniform uint point_count;

// Uniform block containing up to 256 random directions (x,y,z,0)
// and 256 more completely random vectors
layout (binding = 0, std140) uniform SAMPLE_POINTS
{
    vec4 pos[256];
    vec4 random_vectors[256];
} points;

void main(void)
{
    vec2 P = gl_FragCoord.xy / textureSize(sNormalDepth, 0); // Get texture position from gl_FragCoord
    vec4 ND = textureLod(sNormalDepth, P, 0); // ND = normal and depth
    
    vec3 N = ND.xyz; // Extract normal
    float my_depth = ND.w; // Extract depth

    // Local temporary variables
    int i;
    int j;
    int n;

    float occ = 0.0;
    float total = 0.0;

    // Select random vector using noise1 to generate a random number
	n = int(floor(((noise1(gl_FragCoord.x * gl_FragCoord.y) + 1.0) / 2.0) * 255.0));
    vec4 v = points.random_vectors[n]; // Pull one of the random vectors

    float r = (v.r + 3.0) * 0.1; // r is our 'radius randomizer'

    // For each random point (or direction)...
    for (i = 0; i < point_count; i++)
    {
        vec3 dir = points.pos[i].xyz; // Get direction
        if (dot(N, dir) < 0.0) // Put it into the correct hemisphere
            dir = -dir;
        float f = 0.0; // f is the distance we've stepped in this direction
        float z = my_depth; // z is the interpolated depth

        // We're going to take 4 steps - we could make this
        // configurable
        total += 4.0;

        for (j = 0; j < 4; j++)
        {
            
            f += r; // Step in the right direction
            z -= dir.z * f; // Step _towards_ viewer reduces z
            // Read depth from current fragment
            float their_depth = textureLod(sNormalDepth, (P + dir.xy * f * ssao_radius), 0).w;
            // Calculate a weighting (d) for this fragment's contribution to occlusion
            float d = abs(their_depth - my_depth);
            d *= d;
            if ((z - their_depth) > 0.0) // If we're obscured, accumulate occlusion
                occ += 4.0 / (1.0 + d);
        }
    }
    float ao_amount = float(1.0 - occ / total); // Calculate occlusion amount
    vec4 object_color =  textureLod(sColor, P, 0); // Get object color from color texture
	color = object_color * ao_amount; // Mix in ambient color scaled by SSAO level
}