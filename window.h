#include "shader.h"

class Window
{
	inline static bool mLeftDown = false;
	inline static bool mRightDown = false;
	inline static int _mods = 0;

	inline static bool Lold = false;
	inline static bool Rold = false;
	
	inline static int _key = 0;
	inline static int charMods = 0;
	inline static bool keyDown = false;
	inline static bool kold = false;

	static void MouseButtonDown(GLFWwindow* window, int button, int action, int mods)
	{
		_mods = mods;
	
		if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			mLeftDown = true;
		else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			mLeftDown = false;
			
		if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
			mRightDown = true;
		else if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
			mRightDown = false;
	}
	
	static void InputKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		charMods = mods;
		_key = key;
		
		if(action == GLFW_PRESS)
			keyDown = true;
		else
			keyDown = false;
		
	}

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
	
	void (*Resize)();
	
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
		
		glfwSetMouseButtonCallback(handle, MouseButtonDown);
		glfwSetKeyCallback(handle, InputKeyDown);
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
			
			int width, height;
			glfwGetWindowSize(handle, &width, &height);
			
			if(width != this->width || height != this->height)
			{
				this->width = width;
				this->height = height;
				
				if(Resize != 0)
					Resize();
			}
			
			glViewport(0, 0, width, height);

			glDisable(GL_DEPTH_TEST);
			glEnable( GL_BLEND );
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			if(mLeftDown && mLeftDown != Lold)
			{
				if(MouseLeftButtonDown != 0)
					MouseLeftButtonDown(_mods);
				Lold = mLeftDown;
			}
			else if(!mLeftDown && mLeftDown != Lold)
			{
				if(MouseLeftButtonUp != 0)
					MouseLeftButtonUp(_mods);
				Lold = mLeftDown;
			}
			
			if(mRightDown && mRightDown != Rold)
			{
				if(MouseRightButtonDown != 0)
					MouseRightButtonDown(_mods);
				Rold = mRightDown;
			}
			else if(!mRightDown && mRightDown != Rold)
			{
				if(MouseRightButtonUp != 0)
					MouseRightButtonUp(_mods);
				Rold = mRightDown;
			}

			if(keyDown && keyDown != kold)
			{
				if(KeyDown != 0)
					KeyDown(_key, charMods);
				kold = keyDown;
			}else if(!keyDown && keyDown != kold)
			{
				if(KeyUp != 0)
					KeyUp(_key, charMods);
				kold = keyDown;
			}

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
