#version 430 core

// https://github.com/openglsuperbible/sb6code/blob/master/bin/media/shaders/ssao/ssao.fs.glsl

// Samplers for pre-rendered color, normal and depth
layout (binding = 0) uniform sampler2D sColor;
layout (binding = 1) uniform sampler2D sNormalDepth;

// Final output
layout (location = 0) out vec4 color;

// Various uniforms controling SSAO effect
uniform float ssao_radius;
uniform float NumSteps;
uniform uint point_count;

// Uniform block containing up to 256 random directions (x,y,z,0)
// and 256 more completely random vectors
// This interface block is defined with std140 to explicitly state the layout of the block.
//	STD140 prevents unused uniforms from being removed
layout (binding = 0, std140) uniform KERNEL_POINTS {
    vec4 pos[256];
    vec4 random_vectors[256];
} points;

void main(void) {
    vec2 TexPosition = gl_FragCoord.xy / textureSize(sNormalDepth, 0); // Get texture position from gl_FragCoord
    vec4 NormalAndDepth = textureLod(sNormalDepth, TexPosition, 0); // ND = normal and depth
    
    vec3 Normal = NormalAndDepth.xyz; // Extract normal
    float CurrentFragDepth = NormalAndDepth.w; // Extract depth

    float occ = 0.0;	// Occlusion Factor
    float TotalSteps = 0.0;

    // Select random vector using noise1 to generate a random number
	int RandomIndex = int(floor(((noise1(gl_FragCoord.x * gl_FragCoord.y) + 1.0) / 2.0) * 255.0));
    vec4 RandomVector = points.random_vectors[RandomIndex]; // Pull one of the random vectors

    float RandomRadius = (RandomVector.r + 3.0) * 0.1; // 'radius randomizer'

    // For each random point (or direction)...
    for (int i = 0; i < point_count; i++)
    {
        vec3 dir = points.pos[i].xyz; // Get direction
        if (dot(Normal, dir) < 0.0) // Put it into the correct hemisphere
            dir = -dir;
        float Distance = 0.0; // The distance we've stepped in this direction
        float InterpolatedDepth = CurrentFragDepth;

        // We're going to take (NumSteps) steps
        TotalSteps += NumSteps;

        for (int j = 0; j < int(NumSteps); j++)
        {
            
            Distance += RandomRadius; // Step in the right direction
            InterpolatedDepth -= dir.z * Distance; // Step _towards_ viewer reduces z
            // Read depth from current fragment
            float SampleDepth = textureLod(sNormalDepth, (TexPosition + dir.xy * Distance * ssao_radius), 0).w;
            // Calculate a weighting for this fragment's contribution to occlusion
            float OcclusionWeight = abs(SampleDepth - CurrentFragDepth);
            OcclusionWeight *= OcclusionWeight;
            if ((InterpolatedDepth - SampleDepth) > 0.0) // If we're obscured, accumulate occlusion
                occ += NumSteps / (1.0 + OcclusionWeight);
        }
    }
    float ao_amount = float(1.0 - occ / TotalSteps); // Calculate occlusion amount
    vec4 object_color =  textureLod(sColor, TexPosition, 0); // Get object color from color texture
	color = object_color * ao_amount; // Mix in ambient color scaled by SSAO level
}