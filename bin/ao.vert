#version 430 core
// https://github.com/openglsuperbible/sb6code/blob/master/bin/media/shaders/ssao/ssao.vs.glsl
// Vertices array is the 4 indices used to draw the screen when utilizing AO
void main(void)
{
    const vec4 vertices[] = vec4[]( vec4(-1.0, -1.0, 0.5, 1.0),
                                    vec4( 1.0, -1.0, 0.5, 1.0),
                                    vec4(-1.0,  1.0, 0.5, 1.0),
                                    vec4( 1.0,  1.0, 0.5, 1.0) );

    gl_Position = vertices[gl_VertexID];
}