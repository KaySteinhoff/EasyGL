# EasyGL an OpenGL framework for C

## About

EasyGL uses GLFW to create an OpenGL window and tries to give the user the biggest range of possibilities while it handles the OpenGL backen. It also makes using shaders much more easy through the LoadShader() and LoadPartialShader() functions. These take Shader struct to apply them to and the source path and boom: a working shader ready to use in your window.

## How to use

### Window

The window struct can be created but it needs some default values to be able to run.
Most are self explanatory like the title or the width and height, however there are some settings that need to be set that may not be as instinctive.

These being the background color as an float array, the number of shaders and the 'RenderFunction'.

```C
Window window;

window.width = 800;
window.height = 600;
window.name = "Example";
window.backgroundColor[0] = 0.2;
window.backgroundColor[1] = 0.3;
window.backgroundColor[2] = 0.3;
window.numShaders = 1;
window.RenderFunction = MyRenderFunction;
```

The RenderFunction will be called every frame.

After this the CreateWindow() function has to be called to setup the backend of the window. Then just call StartWindow() and Destroy() to start it and after the user closes it have it be cleaned up.

```C
Window window;

window.width = 800;
window.height = 600;
window.name = "Example";
window.backgroundColor[0] = 0.2;
window.backgroundColor[1] = 0.3;
window.backgroundColor[2] = 0.3;
window.numShaders = 1;
window.RenderFunction = MyRenderFunction;

//Setup GLFW window and create OpenGL backend
CreateWindow(&window);

//Start the run loop and show the window
StartWindow(&window);

//Clean up everything after the window is closed
Destroy(&window);
```

### Shaders

A shader in EasyGL can be seen as any object that is drawn to the screen like Images, Text or 3D Models. They take in the verticies as well as the amount of verticies and an ShaderUpdate function which is optional but useful to update the shader values in order to not cramp your code too much. It also has the advantage of being able to update multiple shader using the same update routine when it is useful.

The Shader setup is to be called after CreateWindow() but before StartWindow().

```C
//Setup GLFW window and create OpenGL backend
CreateWindow(&window);

//Load the shader with the 'shader.vert' vertex shader and 'shader.frag' fragment shader
LoadShader(&window.shader[0], "shader.vert", "shader.frag");
window.shader[0].ShaderUpdate = MyShaderUpdate;

//Set the verticies
window.shader[0].num = sizeof(verticies)/sizeof( verticies[0]);
window.shader[0].verticies = malloc(window.shader[0].num*sizeof(float));
memcpy(window.shader[0].verticies, verticies, sizeof(verticies));

//Start the run loop and show the window
StartWindow(&window);
```

Now just Update the shader values in your ShaderUpdate function according to your program and use glUseProgram() to use your shader in your RenderFunction.

```C
void MyRenderFunction()
{
	glUseProgram(window.shader[0].id);
	window.shader[0].ShaderUpdate(window.shader[0].id, window.shader[0].transform);
	
	glBindVertexArray(window.shader[0].vao);
	
	glDrawArrays(GL_TRIANGLES, 0, window.shader[0].num);
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
