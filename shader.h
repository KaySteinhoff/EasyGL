#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "linmath.h"

typedef struct
{
	GLuint handle;
	GLuint id;

	int success;
	char infoLog[512];
}PartialShader;

class IEventListener
{
	
};

class Shader : public IEventListener
{
protected:
	float *verticies;
	int num;

	void LoadShaderValues(GLenum usage)
	{
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num, verticies, usage);
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
		glEnableVertexAttribArray(1);
	}

public:
	GLuint id;

	GLuint vbo;
	GLuint vao;

	PartialShader vertexShader;
	PartialShader fragmentShader;

	mat4x4 transform;
	vec3 position;
	vec3 rotation;
	vec3 scale;

	int success;
	char infoLog[512];

	void (*ShaderUpdate)(GLint, mat4x4);
	
	virtual void Show() = 0;
	
	const char* LoadSource(const char* path)
	{
		char* source;
		long size;

		FILE* fptr = fopen(path, "r");

		if(!fptr)
		{
			printf("Couldn't open file because it is used by another process!\n");
			return NULL;
		}

		fseek(fptr, 0, SEEK_END);
		size = ftell(fptr);
		fseek(fptr, 0, SEEK_SET);

		//Read
		source = (char*)malloc(size+1);
		fread(source, size, 1, fptr);
		fclose(fptr);
		source[size] = 0;

		return source;
	}
	
	void LoadPartialShader(GLenum shaderType, PartialShader* shader, const char* shaderSource)
	{
		shader->id = glCreateShader(shaderType);
		glShaderSource(shader->id, 1, &shaderSource, NULL);
		glCompileShader(shader->id);

		//Reading infolog and success
		glGetShaderiv(shader->id, GL_COMPILE_STATUS, &shader->success);
		glGetShaderInfoLog(shader->id, 512, NULL, shader->infoLog);

		//Print possible shader errors
		printf("%s", shader->infoLog);
	}
	
	Shader(const char* vertexShader, const char* fragmentShader)
	{
		if(!strcmp(vertexShader, "") || !strcmp(fragmentShader, ""))
			return;
	
		//Vertex shader
		LoadPartialShader(GL_VERTEX_SHADER, &this->vertexShader, LoadSource(vertexShader));

		//Fragment shader
		LoadPartialShader(GL_FRAGMENT_SHADER, &this->fragmentShader, LoadSource(fragmentShader));

		//Attaching them to the final Shader
		this->id = glCreateProgram();
		glAttachShader(this->id, this->vertexShader.id);
		glAttachShader(this->id, this->fragmentShader.id);
		glLinkProgram(this->id);

		//Reading infolog and success
		glGetProgramiv(this->id, GL_LINK_STATUS, &success);
		glGetProgramInfoLog(this->id, 512, NULL, infoLog);

		printf("%s\n", infoLog);
	}
	
};
