#version 420 core     
#extension GL_EXT_texture_array : enable
//layout (binding = 0) uniform sampler2D tex_color;
layout (binding = 1) uniform sampler2D tex_norm;
layout (binding = 2) uniform sampler2D tex_glow;
layout (binding = 3) uniform sampler2DArray tex_animation;
                                                          
in vec4 vVaryingColor;
in vec2 vVaryingTextureCoord;
in vec4 vVaryingLightDir;
in vec4 vVaryingNormal;
in vec4 vVaryingPosViewSpace;
out vec4 fragOut;

uniform mat4 normalMatrix;
uniform int currentFrame;
uniform int totalFrames;

//http://pouet.net/topic.php?which=6266
//calculates tangent space matrix from normal, vector in plane and texture coordinates
//needs normalized normal and viewDirection!
mat3 computeTangentFrame(vec3 normal, vec3 viewDirection, vec2 texCoord)
{
    vec3 dpx = dFdx(viewDirection);
    vec3 dpy = dFdy(viewDirection);
    vec2 dtx = dFdx(texCoord);
    vec2 dty = dFdy(texCoord);
    
    vec3 tangent = normalize(dpx * dty.t - dpy * dtx.t);
	vec3 binormal = normalize(-dpx * dty.s + dpy * dtx.s);
   
    return mat3(tangent, binormal, normal);
}
                                                          
void main(void)                               
{
	vec3 n = normalize(vVaryingNormal.xyz);
    vec3 norm = normalize(texture(tex_norm, vVaryingTextureCoord).xyz * 2.0 - 1.0);
	
	mat3 TBN = computeTangentFrame(n, -normalize(vVaryingPosViewSpace.xyz), vVaryingTextureCoord);
	
	vec4 normalNorm = vec4(normalize(TBN * norm),0);

	float diff = max(0.0, dot(normalNorm, normalize(vVaryingLightDir)));
	vec4 vReflection = normalize(reflect(-normalize(vVaryingLightDir), normalNorm));
	vec4 viewDir = -normalize(vec4(vVaryingPosViewSpace.xyz,0));

	fragOut = vec4(diff);
	fragOut *= texture2DArray(tex_animation, vec3(vVaryingTextureCoord.st, currentFrame));
	//fragOut *= vec4(texture(tex_color,vVaryingTextureCoord).rgb,1);
	fragOut += vec4(texture(tex_glow,vVaryingTextureCoord).rgb*.2,1);

	if(diff > 0)
	{
		float spec = pow(max(0, dot(viewDir, vReflection)),192.0);
		fragOut+=  vec4(spec);
	}
}                                                         