# EasyGL an OpenGL framework for C/C++

## About

EasyGL uses GLFW to create an OpenGL window and tries to give the user the biggest range of possibilities while it handles the OpenGL backen. It also makes using shaders much more easy through the LoadShader() function. This takes the source path and boom: a working shader ready to use in your window.

## How to use

### Window

### Functions

The Window only contains two functions Window::Show() and Window::Destroy().

Window::Show() starts the draw loop and starts calling Window.RenderFunction every frame.

Window::Destroy() cleans up all Shader buffers and other values, cleaning the window to what it basically is before the user configured it.

### Usage

The window needs some default values to be able to run.
Most are self explanatory like the title or the width and height, however there are some settings that need to be set that may not be as instinctive.

These being the background color as an float array, the number of shaders and the 'RenderFunction'.

```C
//                       Title	 |Width|Height|Number of shaders
Window window = Window("Easy2Edit", 1366, 720  ,		  2		  );

window.backgroundColor[0] = 0.2; //R
window.backgroundColor[1] = 0.3; //G
window.backgroundColor[2] = 0.3; //B
window.RenderFunction = MyRenderFunction;
```

The RenderFunction will be called every frame.

Then just call Window::Show() to start it and Window::Destroy() to clean everything up after the user closes it.

```C
//                       Title	 |Width|Height|Number of shaders
Window window = Window("Easy2Edit", 1366, 720  ,		  2		  );

window.backgroundColor[0] = 0.2; //R
window.backgroundColor[1] = 0.3; //G
window.backgroundColor[2] = 0.3; //B
window.RenderFunction = MyRenderFunction;

//Start the run loop and show the window
window.Show();

//Clean up everything after the window is closed won't be called until window is closed
window.Destroy();
```

### Shaders

### Usage
A shader in EasyGL can be seen as any object that is drawn to the screen like Images, Text or 3D Models. They take in the verticies as well as the amount of verticies and an ShaderUpdate function which is optional but useful to update the shader values in order to not cramp your code too much. It also has the advantage of being able to update multiple shader using the same update routine should it be practical.

```C
//Load the Example.jpg file and render the Image at 0, 0(bottom, left). A window has be passed down to the image as a parent
Image img = Image("Example.jpg", 0, 0, &window);
window.shader[0] = &img;
window.shader[0]->ShaderUpdate = MyShaderUpdate;

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

//Simplest shader update function
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

Should you want to set the Shaders of any class deriven from shader to a custom one you can call the Shader::LoadPartialShader(GLenum type, PartialShader* shader, const char* source) function and set the PartialShader that's passed to your code.

Image and Text actually do just that.

```C
//Vertex shader
		LoadPartialShader(GL_VERTEX_SHADER, &this->vertexShader, "#version 110\nattribute vec3 aPos;void main(){gl_Position = vec4(aPos, 1.0);}");

		//Fragment shader
		LoadPartialShader(GL_FRAGMENT_SHADER, &this->fragmentShader, "#version 110\nvoid main(){gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);}");
```

This changed the Vertex shader to use the raw vertex position handed to it and the fragment shader to always return white.

This is not a feature that is meant to be used easily, as knowledge of shaders is required, and I'm going to asume that if you know what Vertex and Fragment shaders are you'll know how to use it. ;)

### Events

Events can be subscribed to by treating it like a variable and setting a function equal to it.

```C

window.MouseMove = MyMouseMove;

```

It is important to know that only one function can be subscribed to the event at a time. (Improvements are planed)

List of all events:

Window::MouseMove 					Parameters: int deltaX, int deltaY
Window::MouseLeftButtonDown 		Parameters: int mods
Window::MouseLeftButtonUp 			Parameters: itn mods
Window::MouseRightButtonDown 		Parameters: int mods
Window::MouseRightButtonUp 		Parameters: int mods
Window::KeyDown 						Parameters: int key, int mods
Window::KeyUp 							Parameters: int key, int mods
Window::Resize 						Parameters: None

Image::OnClick 						Parameters: None

## Shipped classes

EasyGL ships three classes as default: Image, Text and Mouse. Both are deriven from Shader as they are rendered to the screen and have a Shader::Show() function to update/redraw them.

### Image class

Image has three functions: LoadImage(const char* path), Show() and CheckClick(double x, double y).

LoadImage(const char* path) loads an image using the path passed to it. This may take a bit depending on size/file format. I have made the observation that .png file load faster than .jpg files.

Show() is the default function required by Shader in order to render the Image to the screen.

CheckClick(double x, double y) checks whether or not the provided x and y coordinates lay inside the image.
If this is the case it will call the OnClick event should it be subscribed to.

### Text class

The Text class is just a default class with the default Show() function provided by the Shader class.

Quick tip: one text instance can draw multiple different things to the screen i.e.: should you want to draw "Hello World" in red to the screen but also want a shadow underneath it you can change the color of it to black set it to an offset call Show() change the color and position back and call Show() again.
This will cause a shadow effect using a single text instance.

### Mouse class

The Mouse class is not a shader but holds a static X and Y position of the cursor on the window.

## Create a deriven from Shader

### Setup

_WARNING: EasyGL is not yet optimized for 3d rendering. My main focus layed in making a framework suitable for creating crude apps in to have as much freedom as possible while making a desktop app! This is purely an example and not meant to be used._

If you for example want to create a class able to read and render .obj files you can create and use it rather simple:

```C
class Obj : public Shader
{
public:
	
	Obj():Shader("", "")
	{
		
	}
	
	void Show()
	{
		
	}
	
};
```

This class compiles but, as one probably expects, does nothing. To add life to it we need two things: Shaders and draw logic.

First lets make the constructor fit your goal:

```C
class Obj : public Shader
{
	float[] verts;

	float[] LoadFile(const char* path)
	{
		//Load the file here ...
	}

public:
	
	Obj(const char* path):Shader("", "")
	{
		verts = LoadFile(path);
		num = sizeof(verts)/sizeof(verts[0]);
		memcpy(verticies, verts, sizeof(verts));
	}
	
	void Show()
	{
		
	}
	
};
```

Now the constructor loads the obj file and puts all values into their respective variables. You can either set the shaders like shown previously by hard coding the shader code into the program or you can write it into files and give it into the Shader constructor.

```C
class Obj : public Shader
{
	float[] verts;

	float[] LoadFile(const char* path)
	{
		//Load the file here ...
	}

public:
	
	Obj(const char* path):Shader("objShader.vert", "objShader.frag")
	{
		verts = LoadFile(path);
		num = sizeof(verts)/sizeof(verts[0]);
		memcpy(verticies, verts, sizeof(verts));
	}
	
	void Show()
	{
		
	}
	
};
```

Now all you need to do is put your render code into the Show() function and you're good-to-go.

```C
class Obj : public Shader
{
	float[] verts;

	float[] LoadFile(const char* path)
	{
		//Load the file here ...
	}

public:
	
	Obj(const char* path):Shader("objShader.vert", "objShader.frag")
	{
		verts = LoadFile(path);
		num = sizeof(verts)/sizeof(verts[0]);
		memcpy(verticies, verts, sizeof(verts));
	}
	
	void Show()
	{
		//Render your object here ...
	}
	
};
```

I'm not gonna show any examples for the render process nor the shaders as you're going to find 1001 examples on google that will work just fine.
