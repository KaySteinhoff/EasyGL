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

typedef struct
{
	GLuint id;
	GLuint texture;

	mat4x4 transform;
	float zoom;
	float x, y;

	GLuint vbo;
	GLuint vao;

	int success;
	char infoLog[512];
	PartialShader vertexShader;
	PartialShader fragmentShader;
	float *verticies;
	int num;

	void (*ShaderUpdate)(GLint, mat4x4);
}Shader;

typedef struct
{
	GLFWwindow* handle;
	int width;
	int height;
	char* name;
	vec3 backgroundColor;

	void (*RenderFunction)();
	int numShaders;
	Shader* shader;

	mat4x4 projection, view;

	float *verticies;
}Window;

void LoadTexture(Shader* shader, const char* path)
{
	glGenTextures(1, &shader->texture);
	glBindTexture(GL_TEXTURE_2D, shader->texture);
	//set texture wrapping/filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//load and generate texture
	int width, height, channels;
	unsigned char *data = stbi_load(path, &width, &height, &channels, 0);

	if(data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}else
	{
		printf("Failed to load texture!");
		return;
	}

	mat4x4 t;
	mat4x4_identity(t);
	mat4x4_identity(shader->transform);
	mat4x4_scale_aniso(shader->transform, t, width/(float)height, 1.0, 1.0);

	shader->x = shader->transform[0][0];
	shader->y = shader->transform[1][1];

	stbi_image_free(data);
}

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
	source = malloc(size+1);
	fread(source, size, 1, fptr);
	fclose(fptr);
	source[size] = 0;

	return source;
}

void LoadPartialShader(GLenum shaderType, PartialShader* shader, const char* path)
{
	//Load shader source
	const char* shaderSource = LoadSource(path);

	shader->id = glCreateShader(shaderType);
	glShaderSource(shader->id, 1, &shaderSource, NULL);
	glCompileShader(shader->id);

	//Reading infolog and success
	glGetShaderiv(shader->id, GL_COMPILE_STATUS, &shader->success);
	glGetShaderInfoLog(shader->id, 512, NULL, shader->infoLog);

	//Print possible shader errors
	printf("%s", shader->infoLog);
}

void LoadShader(Shader* shader, const char* vertexShader, const char* fragmentShader)
{
	//Vertex shader
	LoadPartialShader(GL_VERTEX_SHADER, &shader->vertexShader, vertexShader);

	//Fragment shader
	LoadPartialShader(GL_FRAGMENT_SHADER, &shader->fragmentShader, fragmentShader);

	//Attaching them to the final Shader
	shader->id = glCreateProgram();
	glAttachShader(shader->id, shader->vertexShader.id);
	glAttachShader(shader->id, shader->fragmentShader.id);
	glLinkProgram(shader->id);

	//Reading infolog and success
	glGetProgramiv(shader->id, GL_LINK_STATUS, &shader->success);
	glGetProgramInfoLog(shader->id, 512, NULL, shader->infoLog);

	printf("%s\n", shader->infoLog);
}
