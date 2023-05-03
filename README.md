# EasyGL an OpenGL framework for C/C++

## About

EasyGL uses GLFW to create an OpenGL window and tries to give the user the biggest range of possibilities while it handles the OpenGL backen. It also makes using shaders much more easy through the LoadShader() function. This takes the source path and boom: a working shader ready to use in your window.

## How to use

### Window

### Functions

The Window only contains two functions Window::Show() and Window::Destroy().
They both do just as they are named.

Window::Show() starts the draw loop and starts calling Window.RenderFunction every frame.

Window::Destroy() cleans up all Shader buffers, and other values cleaning the window to what it basically is before the user configured it.

### Usage

The window needs some default values to be able to run.
Most are self explanatory like the title or the width and height, however there are some settings that need to be set that may not be as instinctive.

These being the background color as an float array, the number of shaders and the 'RenderFunction'.

```C
//									Title	 |Width|Height|Number of shaders
Window window = Window("Easy2Edit", 1366, 720  ,		  2		  );

window.backgroundColor[0] = 0.2;
window.backgroundColor[1] = 0.3;
window.backgroundColor[2] = 0.3;
window.RenderFunction = MyRenderFunction;
```

The RenderFunction will be called every frame.

After this the CreateWindow() function has to be called to setup the backend of the window. Then just call StartWindow() and Destroy() to start it and after the user closes it have it be cleaned up.

```C
//									Title	 |Width|Height|Number of shaders
Window window = Window("Easy2Edit", 1366, 720  ,		  2		  );

window.backgroundColor[0] = 0.2;
window.backgroundColor[1] = 0.3;
window.backgroundColor[2] = 0.3;
window.RenderFunction = MyRenderFunction;

//Start the run loop and show the window
window.Show();

//Clean up everything after the window is closed
window.Destroy();
```

### Shaders

### Usage
A shader in EasyGL can be seen as any object that is drawn to the screen like Images, Text or 3D Models. They take in the verticies as well as the amount of verticies and an ShaderUpdate function which is optional but useful to update the shader values in order to not cramp your code too much. It also has the advantage of being able to update multiple shader using the same update routine when it is useful.

The Shader setup is to be called after CreateWindow() but before StartWindow().

```C
//									Title	 |Width|Height|Number of shaders
Window window = Window("Easy2Edit", 1366, 720  ,		  2		  );

window.backgroundColor[0] = 0.2;
window.backgroundColor[1] = 0.3;
window.backgroundColor[2] = 0.3;
window.RenderFunction = MyRenderFunction;

//Load the shader with the 'shader.vert' vertex shader and 'shader.frag' fragment shader
Image img = Image("Example.jpg", 0, 0, &window, "shader.vert", "shader.frag");
window.shader[0] = &img;
window.shader[0]->ShaderUpdate = MyShaderUpdate;

//Set the verticies
window.shader[0]->num = sizeof(verticies)/sizeof( verticies[0]);
window.shader[0]->verticies = malloc(window.shader[0]->num*sizeof(float));
memcpy(window.shader[0]->verticies, verticies, sizeof(verticies));

//Start the run loop and show the window
window.Show();
```

Every object rendered to the screen is a class at some point derriven from Shader. This is because of a few reasons: 
1. To simplify the structure
2. To simplify the management done by the user
3. To uniformly create and update all rendered objects

```C
void MyRenderFunction()
{
	//As every object rendered is derriven from Shader they, 
	//in the best case for custom classes and in all cases of shipped ones, 
	//can be drawn to the screen using the virtual Shader::Show() function
	for(int i = 0; i < window.numShaders; ++i)
		window.shader[i]->Show();
}

void MyShaderUpdate(GLint id, mat4x4 transform)
{
	GLint attribLoc;

	attribLoc = glGetUniformLocation(id, "transform");
	glUniformMatrix4fv(attribLoc, 1, GL_FALSE, (const GLfloat*)transform);

	attribLoc = glGetUniformLocation(id, "projection");
	glUniformMatrix4fv(attribLoc, 1, GL_FALSE, (const GLfloat*)window.projection);

	attribLoc = glGetUniformLocation(id, "view");
	glUniformMatrix4fv(attribLoc, 1, GL_FALSE, (const GLfloat*)window.view);
}
```
