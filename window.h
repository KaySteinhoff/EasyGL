#pragma once

class MouseArgs : public EventArgs
{
public:
	float x, y;
	MouseArgs(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
};

class ModArgs : public EventArgs
{
public:
	int mods;
	ModArgs(int mods)
	{
		this->mods = mods;
	}
};

class KeyArgs : public EventArgs
{
public:
	int key, mods;
	KeyArgs(int key, int mods)
	{
		this->key = key;
		this->mods = mods;
	}
};

class Window
{
private:
	inline static bool mLeftDown = false;
	inline static bool mRightDown = false;
	inline static int _mods = 0;

	inline static bool Lold = false;
	inline static bool Rold = false;
	
	inline static int _key = 0;
	inline static int charMods = 0;
	inline static bool keyDown = false;
	inline static bool kold = false;

protected:
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

	int cx, cy;

public:
	GLFWwindow* handle;
	int width;
	int height;
	const char* name;
	vec3 backgroundColor;

	void (*RenderFunction)();
	
	EventHandler MouseMove;
		
	EventHandler MouseLeftButtonDown;
	EventHandler MouseLeftButtonUp;
	
	EventHandler MouseRightButtonDown;
	EventHandler MouseRightButtonUp;
	
	EventHandler KeyDown;
	EventHandler KeyUp;
	
	EventHandler Resize;
	
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
			shader[i]->ShaderUpdate(shader[i]->id, shader[i]->transform);
		
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
				
				Resize(nullptr);
			}
			
			glViewport(0, 0, width, height);

			glDisable(GL_DEPTH_TEST);
			glEnable( GL_BLEND );
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			if(mLeftDown && mLeftDown != Lold)
			{				
				MouseLeftButtonDown(new ModArgs(_mods));
				Lold = mLeftDown;
			}
			else if(!mLeftDown && mLeftDown != Lold)
			{				
				MouseLeftButtonUp(new ModArgs(_mods));
				Lold = mLeftDown;
			}
			
			if(mRightDown && mRightDown != Rold)
			{				
				MouseRightButtonDown(new ModArgs(_mods));
				Rold = mRightDown;
			}
			else if(!mRightDown && mRightDown != Rold)
			{							
				MouseRightButtonUp(new ModArgs(_mods));
				Rold = mRightDown;
			}

			if(keyDown && keyDown != kold)
			{			
				KeyDown(new KeyArgs(_key, _mods));
				kold = keyDown;
			}else if(!keyDown && keyDown != kold)
			{
				KeyArgs keys = KeyArgs(_key, _mods);
				
				KeyUp(new KeyArgs(_key, _mods));
				kold = keyDown;
			}

			double xpos, ypos;
			glfwGetCursorPos(handle, &xpos, &ypos);
	
			if(cx != xpos || cy != ypos)
			{
				MouseArgs mouse = MouseArgs(xpos-cx, ypos-cy);
				
				MouseMove(new MouseArgs(xpos-cx, ypos-cy));
				Mouse::X = xpos;
				Mouse::Y = height-ypos;
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
