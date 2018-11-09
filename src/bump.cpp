/*
 * Copyright � 2012-2013 Graham Sellers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


#include <sb6.h>
#include <sb6ktx.h>
#include <vmath.h>
#include <object.h>
#include <assert.h> 
#include <iostream>
#include <TGALoader.h>
using namespace NS_TGALOADER;
#include <shader.h>
#include <Windows.h>
#define STACKS 18
#define SLICES 36
#define PI 3.14159
bool gluInvertMatrix(const float m[16], float invOut[16]);
class singlepoint_app : public sb6::application
{
    void init()
    {
        static const char title[] = "Bump Mapping";

        sb6::application::init();

        memcpy(info.title, title, sizeof(title));
		
    }

    virtual void startup()
    {

		viewMatrix	= vmath::translate(0.0f,0.0f,-3.0f);
		normalViewMatrix = vmath::translate(0.0f,0.0f,-3.0f);
		modelMatrix = vmath::mat4::identity();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		GLuint  vs, fs;
        
		
		char buffer[1024];


		vs = sb6::shader::load("bump.vs.glsl", GL_VERTEX_SHADER);
		fs = sb6::shader::load("bump.fs.glsl", GL_FRAGMENT_SHADER);
		
        glGetShaderInfoLog(vs, 1024, NULL, buffer);
		OutputDebugStringA(buffer);

        render_prog = glCreateProgram();
        glAttachShader(render_prog, vs);
        glAttachShader(render_prog, fs);
        glLinkProgram(render_prog);

        glDeleteShader(vs);
        glDeleteShader(fs);
		int loglen;
        glGetProgramInfoLog(render_prog, 1024,&loglen, buffer);
		OutputDebugStringA(buffer);

		GLint status;
		glGetProgramiv(render_prog, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
			exit(1);

		uniforms.model = glGetUniformLocation(render_prog, "model");
		uniforms.view = glGetUniformLocation(render_prog, "view");
		uniforms.projection = glGetUniformLocation(render_prog, "projection");
		uniforms.lightSource = glGetUniformLocation(render_prog, "lightSource");
		uniforms.normalMatrix = glGetUniformLocation(render_prog, "normalMatrix");
		


		struct vertex
		{
			// Position
			float x;
			float y;
			float z;
			float w;

			// Normal
			float xi;
			float yi;
			float zi;
			float wi;

			// texture coordinates
			float s;
			float t;

			// Color
			float r;
			float g;
			float b;
			float a;
		};


		glGenVertexArrays(1, &vaoSphere);
        glBindVertexArray(vaoSphere);
		
		vmath::vec4 normalVec;
		vertex *circle = new vertex[(STACKS+1) * (SLICES+1)]();
		int numVertices = 0;
		for (int lat = -90; lat <= 90; lat += 180/STACKS)
		{
			for (int lon = 0; lon <= 360; lon += 360/SLICES)
			{
				
				
				float radius = cos((float)lat * PI/180.0f);
				circle[numVertices].y = sin((float)lat * PI/180.0f);
				circle[numVertices].x = cos((float)lon * PI/180.0f)*radius;
				circle[numVertices].z = -sin((float)lon * PI/180.0f)*radius;
				circle[numVertices].w = 1;
				normalVec = vmath::vec4(circle[numVertices].x,circle[numVertices].y,circle[numVertices].z,0.0f);
				vmath::normalize(normalVec);
				
				circle[numVertices].xi = normalVec[0];
				circle[numVertices].yi = normalVec[1];
				circle[numVertices].zi = normalVec[2];
				circle[numVertices].wi = 0;
				circle[numVertices].s = (float)lon/360.0f;
				circle[numVertices].t = (float)(lat+90)/180.0f;
				circle[numVertices].r = circle[numVertices].z;
				circle[numVertices].g = circle[numVertices].x;
				circle[numVertices].b = circle[numVertices].y;
				circle[numVertices].a = 1;
				numVertices++;

			}
		}
		GLuint vbo;
		// Allocate and initialize a buffer object
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, ((STACKS+1) * (SLICES+1)) * sizeof(vertex), circle, GL_STATIC_DRAW);

		// Set up two vertex attributes - first positions
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
							  sizeof(vertex), (void *)offsetof(vertex, x));
		glEnableVertexAttribArray(0);

		// Now colors
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
							  sizeof(vertex), (void *)offsetof(vertex, r));
		glEnableVertexAttribArray(1);

		// Now textures
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
							  sizeof(vertex), (void *)offsetof(vertex, s));
		glEnableVertexAttribArray(2);
		
				// Now textures
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE,
							  sizeof(vertex), (void *)offsetof(vertex, xi));
		glEnableVertexAttribArray(3);




		GLuint *sphereIndices = new GLuint[STACKS * SLICES * 6]();
		GLuint triIndex = 0;
		for (GLuint stack = 0; stack < STACKS; ++stack)
		{
			for (GLuint slice = 0; slice < SLICES; ++slice)
			{
				// Bottom tri of the quad
				sphereIndices[triIndex++] = (stack * (SLICES+1)) + slice; //1
				sphereIndices[triIndex++] = (stack * (SLICES+1)) + ((slice+1)%(SLICES+1));//2
				sphereIndices[triIndex++] = ((stack+1) * (SLICES+1)) + ((slice+1)%(SLICES+1));//3
 
				// Top tri of the quad
				sphereIndices[triIndex++] = (stack * (SLICES+1)) + slice; //1
				sphereIndices[triIndex++] = ((stack+1) * (SLICES+1)) + ((slice+1)%(SLICES+1));//3
				sphereIndices[triIndex++] = ((stack+1) * (SLICES+1)) + (slice%(SLICES+1));//4

			}
		}
 
			
				// The type used for names in OpenGL is GLuint

	// Create and bind a BO for index data
		glGenBuffers(1, &iSphereBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iSphereBuffer);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, STACKS * SLICES * 6 *sizeof(GLuint), sphereIndices, GL_STATIC_DRAW);











		delete [] sphereIndices;
		delete [] circle;

		glGenVertexArrays(1, &vaoCube);
        glBindVertexArray(vaoCube);
		static const vertex vertices[] = 
		{
			{ 1, 1, 1, 1,  1, 1, 1, 1 }, // 0
			{-1, 1, 1, 1,  0, 1, 1, 1 }, // 1
			{-1,-1, 1, 1,  0, 0, 1, 1 }, // 2
			{ 1,-1, 1, 1,  1, 0, 1, 1 }, // 3
			{ 1,-1,-1, 1,  1, 0, 0, 1 }, // 4
			{-1,-1,-1, 1,  0, 0, 0, 1 }, // 5
			{-1, 1,-1, 1,  0, 1, 0, 1 }, // 6
			{ 1, 1,-1, 1,  1, 1, 0, 1 }, // 7
		};

		GLubyte indices[36] = 
		{
			0, 1, 2, 2, 3, 0,                 // Front face
			7, 4, 5, 5, 6, 7,                // Back face
			6, 5, 2, 2, 1, 6,                // Left face
			7, 0, 3, 3, 4, 7,                 // Right face
			7, 6, 1, 1, 0, 7,                 // Top face
			3, 2, 5, 5, 4, 3                 // Bottom face
		};

				// The type used for names in OpenGL is GLuint

		// Allocate and initialize a buffer object
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Set up two vertex attributes - first positions
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
							  sizeof(vertex), (void *)offsetof(vertex, x));
		glEnableVertexAttribArray(0);

		// Now colors
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
							  sizeof(vertex), (void *)offsetof(vertex, r));
		glEnableVertexAttribArray(1);

		// Create and bind a BO for index data
		glGenBuffers(1, &iCubeBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iCubeBuffer);
		// copy data into the buffer object
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


		rotX = 0;
		rotY = 0;
		oldMouseX = 0;
		oldMouseY = 0;

		moveLeft=false;
		moveRight=false;
		moveUp=false;
		moveDown=false;
		objectView = false;
		lookUp = lookDown = lookLeft = lookRight = false;

		glActiveTexture(GL_TEXTURE0);
		// Generate a name for the texture
		glGenTextures(1, &textureColor);
		// Load texture from file
		glBindTexture(GL_TEXTURE_2D, textureColor);
		LoadTGATexture("stone.tga", GL_LINEAR, 
					   GL_LINEAR, GL_CLAMP_TO_EDGE);

		glActiveTexture(GL_TEXTURE0+1);
		// Generate a name for the texture
		glGenTextures(1, &textureBump);
		// Load texture from file
		glBindTexture(GL_TEXTURE_2D, textureBump);
		LoadTGATexture("bump.tga", GL_LINEAR, 
					   GL_LINEAR, GL_CLAMP_TO_EDGE);
	


//		GLuint sampler;
//		glGenSamplers(1, &sampler);
//		glBindSampler(0,sampler);
//		glSamplerParameteri(sampler , GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//glSamplerParameteri(sampler , GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);*/
    }

	void checkInput()
	{
		vmath::mat4 temp;
		float x,y,z,xRot,yRot,zRot;
		x=y=z=xRot=yRot=zRot=0;

		if(moveLeft)
			x-=.1;
		if(moveRight)
			x+=.1;
		if(moveUp)
			z-=.1;
		if(moveDown)
			z+=.1;
		modelMatrix = modelMatrix * vmath::translate(x, y, z);

		if(lookDown)
			modelMatrix = modelMatrix * vmath::rotate(1.0f,1.0f,0.0f,0.0f);
		if(lookUp)
			modelMatrix = modelMatrix * vmath::rotate(-1.0f,1.0f,0.0f,0.0f);
		if(lookLeft)
			modelMatrix = modelMatrix * vmath::rotate(1.0f,0.0f,1.0f,0.0f);
		if(lookRight)
			modelMatrix = modelMatrix * vmath::rotate(-1.0f,0.0f,1.0f,0.0f);

		if(objectView)
		{
				gluInvertMatrix(modelMatrix,temp);
				viewMatrix = vmath::translate(0.0f,0.0f,-2.0f)*temp;
		}
		else
		{
			viewMatrix = normalViewMatrix;
		}
		
		

		
	}

    virtual void render(double currentTime)
    {
		if(resize)
		{
			resize = false;
			glViewport(0,0,w,h);
		}
		checkInput();

		rotY+=1;
		
        static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;

		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

        glUseProgram(render_prog);

		glBindVertexArray(vaoSphere);
		
		glUniform4f(uniforms.lightSource, 0.0f, 10.0f*cos(rotY * PI/180.0f),10.0f*sin(rotY * PI/180.0f),1.0f);
		//glUniform4f(uniforms.lightSource, 10.0f,0.0f,0.0f,1.0f);
					glUniformMatrix4fv(uniforms.projection, 1, GL_FALSE, vmath::perspective(45.0f,(float)w/(float)h,0.1f,20.0f));
		glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(uniforms.model, 1, GL_FALSE, vmath::rotate(rotY,0.0f,1.0f,0.0f));
		glUniformMatrix4fv(uniforms.normalMatrix, 1, GL_FALSE, vmath::rotate(rotY,0.0f,1.0f,0.0f));
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iSphereBuffer);
			//glDrawArrays(GL_POINTS,0,(STACKS+1) * STACK);

			glDrawElements(GL_TRIANGLES, SLICES * STACKS * 6, GL_UNSIGNED_INT, (void*)0);


    }

    virtual void shutdown()
    {
        glDeleteVertexArrays(1, &vaoCube);
		 glDeleteVertexArrays(1, &vaoSphere);
        glDeleteProgram(render_prog);
    }
	virtual void onResize(int w, int h)
    {
        info.windowWidth = w;
        info.windowHeight = h;
		this->w = w;
		this->h = h;
		resize = true;
		
		
    }

	virtual void onMouseMove(int mouseX, int mouseY)
	{
	//	rotY -= (oldMouseX - mouseX)*0.01; //as mouse moves right, rotate right, which is CW (-)
	//	rotX += (oldMouseY - mouseY)*0.01; //as mouse moves down, rotate up, which is CCW (+)
	//
	}

	virtual void onKey(int key, int action)
    {
		//http://www.glfw.org/docs/3.0/group__keys.html
		if(action==1)
		switch (key)
		{
			
			case 86:
				objectView = true;
				break;
			case 283:
				lookUp=true;
				break;
			case 284:
				lookDown=true;
				break;
			case 285:
				lookLeft=true;
				break;
			case 286:
				lookRight=true;
				break;
			case 87:
				moveUp=true;
				break;
			case 83:
				moveDown=true;
				break;
			case 65:
				moveLeft=true;
				break;
			case 68:
				moveRight=true;
				break;

		}
		else if(action==0)
		switch (key)
		{
			case 86:
				objectView = false;
				break;
			case 283:
				lookUp=false;
				break;
			case 284:
				lookDown=false;
				break;
			case 285:
				lookLeft=false;
				break;
			case 286:
				lookRight=false;
				break;
			case 87:
				moveUp=false;
				break;
			case 83:
				moveDown=false;
				break;
			case 65:
				moveLeft=false;
				break;
			case 68:
				moveRight=false;
				break;

		}

    }

private:
	struct
    {
        GLint       model;
		GLint       view;
		GLint       projection;
		GLint       normalMatrix;
		GLint		lightSource;
    } uniforms;
    GLuint          render_prog;
    GLuint          vaoCube;
	GLuint          vaoSphere;
	GLuint			iCubeBuffer;
	GLuint			iSphereBuffer;
	vmath::mat4		modelMatrix;
	vmath::mat4		viewMatrix;
	vmath::mat4		normalViewMatrix;
	sb6::object     object;
	int h,w;
	GLuint textureColor;
	GLuint textureBump;
	float rotX, rotY;
	int oldMouseX, oldMouseY;
	bool moveLeft,moveRight,moveUp,moveDown,lookLeft,lookRight,lookUp,lookDown,resize,objectView;
};
bool gluInvertMatrix(const float m[16], float invOut[16])
{
    float inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}
DECLARE_MAIN(singlepoint_app)
