#include "shader.h"

class Window
{

	

public:
	GLFWwindow* handle;
	int width;
	int height;
	const char* name;
	vec3 backgroundColor;

	void (*RenderFunction)();
	
	void (*MouseLeftButtonDown)(int mods);
	void (*MouseLeftButtonUp)(int mods);
	
	void (*MouseRightButtonDown)(int mods);
	void (*MouseRightButtonUp)(int mods);
	
	void (*KeyDown)(int key, int mods);
	void (*KeyUp)(int key, int mods);
	
	int numShaders;
	Shader **shader;

	mat4x4 projection, view;

	Window(const char* name, int width, int height, int numShaders)
	{
		this->name = name;
		this->width = width;
		this->height = height;
		this->numShaders = numShaders;
	
		stbi_set_flip_vertically_on_load(1);

		printf("started\n");
		if(!glfwInit())
			exit(-1);
		this->handle = glfwCreateWindow(width, height, name, NULL, NULL);

		printf("Created window\n");
		if(!this->handle)
		{
			glfwTerminate();
			exit(-1);
		}
		printf("Checked window\nCreate context\n");

		glfwMakeContextCurrent(this->handle);
		printf("Created context\n");

		GLenum err = glewInit();
		printf("%d: %s\n", err, glewGetErrorString(err));

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		this->shader = (Shader**)malloc(numShaders*sizeof(Shader*));
	}
	
	void Show()
	{
		for(int i = 0; i < numShaders; ++i)
		{
			glGenBuffers(1, &shader[i]->vbo);
			glBindBuffer(GL_ARRAY_BUFFER, shader[i]->vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*shader[i]->num, shader[i]->verticies, GL_STATIC_DRAW);
			glGenVertexArrays(1, &shader[i]->vao);
			glBindVertexArray(shader[i]->vao);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
			glEnableVertexAttribArray(1);

			shader[i]->ShaderUpdate(shader[i]->id, shader[i]->transform);
		}

		mat4x4 tmp;
		while(!glfwWindowShouldClose(handle))
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glClearColor(backgroundColor[0], backgroundColor[0], backgroundColor[0], 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, width, height);

			glDisable(GL_DEPTH_TEST);
			glEnable( GL_BLEND );
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			this->RenderFunction();

			glfwSwapBuffers(handle);
			glfwPollEvents();
		}
	}
	
	void Destroy()
	{
		for(int i = 0; i < numShaders; ++i)
		{
			glDeleteShader(shader[i]->vertexShader.id);
			glDeleteShader(shader[i]->fragmentShader.id);
		}
		glfwDestroyWindow(handle);
		glfwTerminate();
		handle = NULL;
	}
	
};
