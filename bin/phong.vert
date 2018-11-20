#version 420 core                             
layout (location = 0) in vec4 positionIn;	   
layout (location = 2) in vec2 vVaryingTextureCoordIn; 
layout (location = 3) in vec4 normalIn; 
														   
														   
uniform vec4 lightSource;                     
uniform mat4 model;                           
uniform mat4 view;                            
uniform mat4 projection;                      
uniform mat4 normalMatrix;                      
														   
out vec2 vVaryingTextureCoord;    				       
out vec4 vVaryingLightDir;    				       
out vec4 vVaryingNormal;    				       
out vec4 vVaryingPosViewSpace;    				       
                                                          
void main(void)                               
{                                             
		vVaryingTextureCoord = vVaryingTextureCoordIn;			   
		vVaryingLightDir = (view * lightSource)-(view * model * positionIn);				   
		vVaryingNormal = normalMatrix * normalIn;					
		vVaryingPosViewSpace =  view * model * positionIn;        
		gl_Position = projection * view * model * positionIn;			   
}                                             