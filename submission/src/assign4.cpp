/*
 * Copyright © 2012-2013 Graham Sellers
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
#include "TGALoader.h"
#include "OBJLoader.h"
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
        static const char title[] = "Assignment 3";

        sb6::application::init();

        memcpy(info.title, title, sizeof(title));
		
    }

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

	void createVBOfromMesh(NS_OBJLOADER::MESH* Mesh, GLuint *vao, long *numTriangles)
	{
		(*numTriangles) = 0;
		for (int i = 0; i < Mesh->m_Faces.size(); i++)
		{
			if (Mesh->m_Faces.at(i).m_Vertices.size() == 3)
			{
				(*numTriangles)++;
			}
			else if (Mesh->m_Faces.at(i).m_Vertices.size() == 4)
			{
				(*numTriangles)++;
				(*numTriangles)++;
			}

		}


		vertex *data = new vertex[(*numTriangles) * 3]();

		glGenVertexArrays(1, vao);
		glBindVertexArray(*vao);

		(*numTriangles) = 0;
		for (int i = 0; i < Mesh->m_Faces.size(); i++)
		{
			if (Mesh->m_Faces.at(i).m_Vertices.size() == 3)
			{
				cout << "triangle: " << (*numTriangles) << endl;
				(*numTriangles)++;

				for (int j = 0; j < 3; j++)
				{
					data[((*numTriangles) - 1) * 3 + j].x = Mesh->m_Positions.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiPositionID).x;
					data[((*numTriangles) - 1) * 3 + j].y = Mesh->m_Positions.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiPositionID).y;
					data[((*numTriangles) - 1) * 3 + j].z = Mesh->m_Positions.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiPositionID).z;
					data[((*numTriangles) - 1) * 3 + j].w = 1;
					data[((*numTriangles) - 1) * 3 + j].r = 1;
					data[((*numTriangles) - 1) * 3 + j].g = Mesh->m_Normals.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiNormalID).y;
					data[((*numTriangles) - 1) * 3 + j].b = 1;
					data[((*numTriangles) - 1) * 3 + j].a = 1;
					data[((*numTriangles) - 1) * 3 + j].s = Mesh->m_TexCoords.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiTexCoordID).m_fTexCoordU;
					data[((*numTriangles) - 1) * 3 + j].t = Mesh->m_TexCoords.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiTexCoordID).m_fTexCoordV;
					data[((*numTriangles) - 1) * 3 + j].xi = Mesh->m_Normals.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiNormalID).x;
					data[((*numTriangles) - 1) * 3 + j].yi = Mesh->m_Normals.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiNormalID).y;
					data[((*numTriangles) - 1) * 3 + j].zi = Mesh->m_Normals.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiNormalID).z;
				}
			}
			else if (Mesh->m_Faces.at(i).m_Vertices.size() == 4)
			{
				cout << "triangle: " << (*numTriangles) << endl;
				(*numTriangles)++;
				for (int j = 0; j < 3; j++) //first triangle = 0,1,2
				{
					data[((*numTriangles) - 1) * 3 + j].x = Mesh->m_Positions.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiPositionID).x;
					data[((*numTriangles) - 1) * 3 + j].y = Mesh->m_Positions.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiPositionID).y;
					data[((*numTriangles) - 1) * 3 + j].z = Mesh->m_Positions.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiPositionID).z;
					data[((*numTriangles) - 1) * 3 + j].w = 1;
					data[((*numTriangles) - 1) * 3 + j].r = 0;
					data[((*numTriangles) - 1) * 3 + j].g = Mesh->m_Normals.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiNormalID).y;
					data[((*numTriangles) - 1) * 3 + j].b = 0;
					data[((*numTriangles) - 1) * 3 + j].a = 0;
					data[((*numTriangles) - 1) * 3 + j].s = Mesh->m_TexCoords.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiTexCoordID).m_fTexCoordU;
					data[((*numTriangles) - 1) * 3 + j].t = Mesh->m_TexCoords.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiTexCoordID).m_fTexCoordV;
					data[((*numTriangles) - 1) * 3 + j].xi = Mesh->m_Normals.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiNormalID).x;
					data[((*numTriangles) - 1) * 3 + j].yi = Mesh->m_Normals.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiNormalID).y;
					data[((*numTriangles) - 1) * 3 + j].zi = Mesh->m_Normals.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiNormalID).z;
				}
				cout << "triangle: " << (*numTriangles) << endl;
				(*numTriangles)++;
				int k = 0;//0,1,2
				for (int j = 0; j < 4; j++) //second triangle = 0,2,3
				{
					if (j == 1)
						j = 2;
					data[((*numTriangles) - 1) * 3 + k].x = Mesh->m_Positions.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiPositionID).x;
					data[((*numTriangles) - 1) * 3 + k].y = Mesh->m_Positions.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiPositionID).y;
					data[((*numTriangles) - 1) * 3 + k].z = Mesh->m_Positions.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiPositionID).z;
					data[((*numTriangles) - 1) * 3 + k].w = 1;
					data[((*numTriangles) - 1) * 3 + k].r = 0;
					data[((*numTriangles) - 1) * 3 + k].g = Mesh->m_Normals.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiNormalID).y;
					data[((*numTriangles) - 1) * 3 + k].b = 0;
					data[((*numTriangles) - 1) * 3 + k].a = 1;
					data[((*numTriangles) - 1) * 3 + k].s = Mesh->m_TexCoords.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiTexCoordID).m_fTexCoordU;
					data[((*numTriangles) - 1) * 3 + k].t = Mesh->m_TexCoords.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiTexCoordID).m_fTexCoordV;
					data[((*numTriangles) - 1) * 3 + k].xi = Mesh->m_Normals.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiNormalID).x;
					data[((*numTriangles) - 1) * 3 + k].yi = Mesh->m_Normals.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiNormalID).y;
					data[((*numTriangles) - 1) * 3 + k].zi = Mesh->m_Normals.at(Mesh->m_Faces.at(i).m_Vertices.at(j).m_uiNormalID).z;
					k++;
				}

			}

		}
		GLuint vbo;
		// Allocate and initialize a buffer object
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, *numTriangles * sizeof(vertex) * 3, data, GL_STATIC_DRAW);

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

		delete[] data;
		glBindVertexArray(0);

	}

    virtual void startup()
    {

		viewMatrix	= vmath::translate(0.0f,0.0f,-3.0f);
		normalViewMatrix = vmath::translate(0.0f,0.0f,-3.0f);
		modelMatrix = vmath::mat4::identity();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		GLuint  vs, fs;

		vs = sb6::shader::load("shader.vert", GL_VERTEX_SHADER);
		fs = sb6::shader::load("shader.frag", GL_FRAGMENT_SHADER);
		
		char buffer[1024];
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
				circle[numVertices].z = sin((float)lon * PI/180.0f)*radius;
				circle[numVertices].w = 1;
				normalVec = vmath::vec4(circle[numVertices].x,circle[numVertices].y,circle[numVertices].z,0.0f);
				//vmath::normalize(normalVec);
				
				circle[numVertices].xi = normalVec[0];
				circle[numVertices].yi = normalVec[1];
				circle[numVertices].zi = normalVec[2];
				circle[numVertices].wi = normalVec[3];
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


		NS_OBJLOADER::MESH Mesh;
		map<string, NS_OBJLOADER::MATERIAL> Materials;
		const char *szFile = "horse.obj";
		LoadOBJFile(szFile, Mesh, Materials);
		createVBOfromMesh(&Mesh, &vaoHorse, &horseTriangles);

		glGenTextures(2, textureColor);
		glGenTextures(2, textureNormal);
		glGenTextures(2, textureSpec);


		//////////////////////////////////////
		//				HORSE				//
		//////////////////////////////////////

		// Load Actual Texture
		glActiveTexture(GL_TEXTURE0 + 0);
		
		// Load texture from file
		glBindTexture(GL_TEXTURE_2D, textureColor[0]);

		LoadTGATexture("Horse.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

		// Load Bump Map
		glActiveTexture(GL_TEXTURE0 + 1);

		// Load texture from file
		glBindTexture(GL_TEXTURE_2D, textureNormal[0]);
		LoadTGATexture("Horse_NP.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

		// Load Fake Glow Map
		glActiveTexture(GL_TEXTURE0 + 2);

		// Load texture from file
		glBindTexture(GL_TEXTURE_2D, textureSpec[0]);
		LoadTGATexture("Horse_spec.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

		//////////////////////////////////////
		//				SPHERE				//
		//////////////////////////////////////

		// Load Actual Texture
		glActiveTexture(GL_TEXTURE0 + 0);
			
		// Load texture from file
		glBindTexture(GL_TEXTURE_2D, textureColor[1]);
		LoadTGATexture("albedo.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

		// Load Bump Map
		glActiveTexture(GL_TEXTURE0 + 1);
		
		// Load texture from file
		glBindTexture(GL_TEXTURE_2D, textureNormal[1]);
		LoadTGATexture("normal.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

		// Load Glow Map
		glActiveTexture(GL_TEXTURE0 + 2);

		// Load texture from file
		glBindTexture(GL_TEXTURE_2D, textureSpec[1]);
		LoadTGATexture("glowBlack.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

		globalPerspective = vmath::perspective(45.0f, (float)w / (float)h, 0.1f, 20.0f);
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
				viewMatrix = vmath::translate(0.0f,0.0f,-0.7f)*vmath::rotate(35.0f, 1.0f, 0.0f, 0.0f)*temp;
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

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, textureColor[1]);

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, textureNormal[1]);

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, textureSpec[1]);

		glBindVertexArray(vaoSphere);

		for (int i = -10; i < 10; i++) {
			for (int j = -10; j <= 0; j++) {
				glUniform4f(uniforms.lightSource, 0.0f, 3.0f/**cos(rotY * PI / 180.0f)*/, 10.0f/**sin(rotY * PI / 180.0f)*/, 1.0f);

				glUniformMatrix4fv(uniforms.projection, 1, GL_FALSE, globalPerspective);
				glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, viewMatrix);
				glUniformMatrix4fv(uniforms.model, 1, GL_FALSE, (vmath::scale(0.5f) * vmath::translate(0.0f, -1.0f, 0.0f) * vmath::rotate(180.0f, 1.0f, 0.0f, 0.0f) * vmath::rotate(15.0f, 0.0f, 0.0f, 1.0f) * vmath::rotate(rotY, 0.0f, 1.0f, 0.0f)));
				glUniformMatrix4fv(uniforms.normalMatrix, 1, GL_FALSE, vmath::rotate(180.0f, 1.0f, 0.0f, 0.0f) * vmath::rotate(15.0f, 0.0f, 0.0f, 1.0f) * vmath::rotate(rotY, 0.0f, 1.0f, 0.0f));

				glBindVertexArray(vaoSphere);
				glDrawElements(GL_TRIANGLES, SLICES * STACKS * 6, GL_UNSIGNED_INT, (void*)0);
			}
		}

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, textureColor[0]);

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, textureNormal[0]);

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, textureSpec[0]);

		glBindVertexArray(vaoHorse);
		
		glUniform4f(uniforms.lightSource, 0.0f, 3.0f/**cos(rotY * PI/180.0f)*/,10.0f/**sin(rotY * PI/180.0f)*/,1.0f);
		
		glUniformMatrix4fv(uniforms.projection, 1, GL_FALSE, globalPerspective);
		glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(uniforms.model, 1, GL_FALSE, (modelMatrix * vmath::rotate(180.0f, 0.0f, 1.0f, 0.0f) * vmath::scale(0.001f)));
		glUniformMatrix4fv(uniforms.normalMatrix, 1, GL_FALSE, vmath::rotate(180.0f,0.0f,1.0f,0.0f));


		//glDrawElements(GL_TRIANGLES, SLICES * STACKS * 6, GL_UNSIGNED_INT, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, horseTriangles * 3);


    }

    virtual void shutdown()
    {
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
	GLuint          vaoSphere;
	GLuint			iCubeBuffer;
	GLuint			iSphereBuffer;

	GLuint          vaoHorse;
	long			horseTriangles;

	vmath::mat4		modelMatrix;
	vmath::mat4		viewMatrix;
	vmath::mat4		normalViewMatrix;
	vmath::mat4		globalPerspective;
	sb6::object     object;
	int h,w;

	GLuint textureColor[2];
	GLuint textureNormal[2];
	GLuint textureGlow[2];
	GLuint textureSpec[2];

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
