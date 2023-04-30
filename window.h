#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"

void CreateWindow(Window* window)
{
	stbi_set_flip_vertically_on_load(1);

	printf("started\n");
	if(!glfwInit())
		exit(-1);
	window->handle = glfwCreateWindow(window->width, window->height, window->name, NULL, NULL);

	printf("Created window\n");
	if(!window->handle)
	{
		glfwTerminate();
		exit(-1);
	}
	printf("Checked window\nCreate context\n");

	glfwMakeContextCurrent(window->handle);
	printf("Created context\n");

	GLenum err = glewInit();
	printf("%d: %s\n", err, glewGetErrorString(err));

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window->shader = malloc(window->numShaders*sizeof(Shader));
	
	/*glDisable(GL_DEPTH_TEST);
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);*/
}

void StartWindow(Window* window)
{
	for(int i = 0; i < window->numShaders; ++i)
	{
		glGenBuffers(1, &window->shader[i].vbo);
		glBindBuffer(GL_ARRAY_BUFFER, window->shader[i].vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*window->shader[i].num, window->shader[i].verticies, GL_STATIC_DRAW);
		glGenVertexArrays(1, &window->shader[i].vao);
		glBindVertexArray(window->shader[i].vao);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
		glEnableVertexAttribArray(1);

		window->shader[i].ShaderUpdate(window->shader[i].id, window->shader[i].transform);
	}

	mat4x4 tmp;
	while(!glfwWindowShouldClose(window->handle))
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearColor(window->backgroundColor[0], window->backgroundColor[0], window->backgroundColor[0], 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, window->width, window->height);

		glDisable(GL_DEPTH_TEST);
		glEnable( GL_BLEND );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		window->RenderFunction();
		
		glfwSwapBuffers(window->handle);
		glfwPollEvents();
	}
}

void Destroy(Window* window)
{

	for(int i = 0; i < window->numShaders; ++i)
	{
		glDeleteShader(window->shader[i].vertexShader.id);
		glDeleteShader(window->shader[i].fragmentShader.id);
	}
	glfwDestroyWindow(window->handle);
	glfwTerminate();
	window->handle = NULL;
}
