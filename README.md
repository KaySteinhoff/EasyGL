# EasyGL an OpenGL framework for C/C++

<a name="About">

## About

EasyGL uses GLFW to create an OpenGL window and tries to give the user the biggest range of possibilities while it handles the OpenGL backend. It also makes using shaders much more easy through the LoadPartialShader() function contained in the Shader class. This takes the shader type, the source path and boom: a working shader ready to use in your window.

<a name="Use">

## How to use

<a name="Window">

### Window

### Functions

The Window only contains two functions Window::Show() and Window::Destroy().

Window::Show() starts the draw loop and starts calling Window.RenderFunction every frame.

Window::Destroy() cleans up all Shader buffers and other values, cleaning the window using the default OpenGL cleanup as well as stopping all shaders.

### Variables

### Protected

|Name|Use|
|---|---|
|cx|Previous mouse x used to calculate mouse deltaX|
|cy|Previous mouse y used to calculate mouse deltaY|

### Public

|Name|Use|
|---|---|
|handle|A GLFWWindow* for the GLFWWindow|
|width|The starting width of the window|
|height|The starting height of the window|
|name|A const char* to the name of the window|
|background|A float array(size 3) of the window background color(0-255)|
|numShaders|The default number of shaders the window contains. Used to preallocate memory at the init|
|shader|A two dimensional Shader pointer pointing to the space allocated by numShaders after the constructor call|
|projection|A mat4x4 containing the projection matrix for this window|
|view|A mat4x4 containing the view matrix for this window|

*_NOTE_*
All linear algebra is done using the linmath.h header from the glfw repository from [github](https://github.com/glfw/glfw/blob/master/deps/linmath.h). 


### Usage

The window needs some default values to be able to run.
Most are self explanatory like the title or the width and height, however there are some settings that need to be set that may not be as intuitive.

These being the background color as a float array, the number of shaders and the 'RenderFunction'.

```C
//                       Title	 |Width|Height|Number of shaders
Window window = Window("Easy2Edit", 1366, 720 ,		2	 );

window.backgroundColor[0] = 0.2; //R
window.backgroundColor[1] = 0.3; //G
window.backgroundColor[2] = 0.3; //B
window.RenderFunction = MyRenderFunction;
```

The RenderFunction will be called every frame. To declare a function suitable to pass as the RenderFunction you'll need to create a void function without any arguments.

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

<a name="Shaders">

### Shaders

### Usage
A shader in EasyGL can be seen as any object that is drawn to the screen like Images, Text, Buttons or 3D Models to name a few. They take in the verticies, the amount of verticies and a ShaderUpdate function which is optional but useful to update the shader values in order to have cleaner code. It also has the advantage of being able to update multiple shaders using the same update routine should it be practical.

```C
//Load the Example.jpg file and render the Image at 0, 0(bottom, left). A window has be passed down to the image as a parent
Image img = Image("Example.jpg", 0, 0, &window);
window.shader[0] = &img;
window.shader[0]->ShaderUpdate = MyShaderUpdate;

//Start the run loop and show the window with the image
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

Image, Text and Button actually do just that. They call Shader::LoadPartialShader, pass a hard coded vertex/fragment shader and thus provide a default class.

```C
//Vertex shader
LoadPartialShader(GL_VERTEX_SHADER, &img.vertexShader, "#version 110\nattribute vec3 aPos;void main(){gl_Position = vec4(aPos, 1.0);}");

//Fragment shader
LoadPartialShader(GL_FRAGMENT_SHADER, &img.fragmentShader, "#version 110\nvoid main(){gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);}");
```

This changed the Vertex shader to use the raw vertex position handed to it and the fragment shader to always return white.

This is not a feature that is meant to be used easily, as knowledge of shaders is required, and I'm going to asume that if you know what Vertex and Fragment shaders are and do you'll know how to use it. ;)

### Shader class

The Shader class is the base class of all rendered objects. This includes things like Images, Text or Buttons. The shader class is designed to be usable in most situations. It can render 3d models(please refer to "Create a class deriven from Shader") or anything it is configured to do.

### Constructor

The Shader class contains a single constructor that takes in the path to the vertex and fragment shader. If you don't want to load any shaders using the constructor(i.e. you hard coded them) you can pass both as empty strings to not load any shader.

```C
class MyClass : public Shader
{
protected:
public:
	MyClass(int myVariable) : Shader("", "") //more code...
```

The first constructor argument is the location of the vertex shader and the second argument is the location of the fragment shader. It's important to pass them in this order as vertex and fragment shader don't have to use a special ending. Yes, most common is the '*.vert' and '*.frag' ending respectively but I did not want to rely on something this easily chageable.

### Functions

The Shader class contains three functions: LoadPartialShader, LoadShaderValues and LoadSource.

LoadPartialShader is public and takes in the type of Shader to compile a pointer to the shaderID as well as the shader souce code, all in that order.

```C
LoadPartialShader(GL_VERTEX_SHADER, &this->vertexShader, "some shader code");
```

This will compile the shader code and put the id to the vertex/fragment shader into the pointer passed to it.

LoadShaderValues is a public funtion that takes in an enum value on how the shader is drawn. It also interprets the vertecies float array in a basic manner: 

- The first three floats are x, y, z
- The Last two floats are the uv-coordinates

This function is not meant to be used in your final product but give a basic interpretation to the vertex array until custom interpretation is needed(i.e. you add vertex colors or normals).

```C
LoadShaderValues(GL_STATIC_DRAW);
```

LoadSource is a public function that takes in the path to a vertex/fragment shader file, reads everything that it contains and then returns a const char* to the loaded shader code.

```C
const char* vertexShaderCode = LoadSource("MyShader.vert");
```

This function is meant to be used in combination with LoadPartialShader by passing the vertex and fragment shader locations in the constructor.

```C
LoadPartialShader(
	GL_VERTEX_SHADER,
	&this->vertexShader,
	LoadSource("MyShader.vert");
);
```

### Variables

### Protected

|Name|Use|
|---|---|
|verticies|A float pointer to the verticies to display|
|num|The number of verticies in vertiecies. Needs to be manually set|

### Public

|Name|Use|
|---|---|
|id|The id of the shader program|
|vbo|The vertex array object|
|vao|The vertex buffer object|
|vertexShader|A PartialShader object for the vertex shader|
|fragmentShader|A PartialShader object for the fragment shader|
|transform|The transform 4x4 matrix of that shader|
|position|The float array(size 3) for the position|
|rotation|The float array(size 3) for the rotation|
|scale|The float array(size 3) for the scale|
|success|An integer indicating the success of the shader compilation|
|infoLog|A char array(size 512) containing the information about the shader compilation|

<a name="Events">

### Events

To subscribe to events you need to create a class.
This class acts as your listener and can be used to subscribe to events. After you instanciate an object of your listener class you'll need to subscribe to the event using the AddListener<T>(T* obj, void(T::*func)(EventArgs*)) function.

### Subscribing

```C
//The event logic
class MyEventListener
{
public:
	MyEventListener(){}
	void Move(EventArgs* e)
	{
		printf("Listener fired!\n");
	}
};

//code...

MyEventListener listener = MyEventListener();
//Subscribe to the MouseMove event using AddListener
//AddListener needs to be passed the type in brackets <T> as well as the object
//the invoked function is supposed be invoked on. The fucntion to be invoked needs
//to be passed as a function pointer
window.MouseMove.AddListener<MyEventListener>(&listener, &MyEventListener::Move);

window.Show();

window.Destroy();
```

```C
//The EventArgs class with all necesary event variables

class MyArgs : public EventArgs
{
public:
	int myVar;
	MyArgs(int myVar)
	{
		this->myVar = myVar;
	}
};

class MyShader : public Shader{

	//some code...
	
	EventHandler MyFailEvent; // Declare EventHandler, preferably public so you can subscribe to it ;)
	EventHandler MyOtherFailEvent;
	
	//some more code...
	
	MyFailEvent(new MyArgs(1));
	
	//Incase you don't want to pass any arguments just pass a nullptr
	MyOtherFailEvent(nullptr);
};

```

You can subscribe to one event with multiple member functions at the same time.

### Unsubscribing

Right now there is only one way to unsubscribe from an event. This is to call RemoveListener(int index) and pass the index of the listener that is to be removed.

```C
window.MouseMove.RemoveListener(0);
```

_*NOTE*_
I am aware that this is not the best solution but due to the difficulties of function pointers not holding non-static member functions this was the best solution I could come up with right now. I will definetly rework the Event system to be even more user friendly.

List of all events:

(To get the mouse coordinates use the Mouse class)
|Event|EventArgs|Parameters|
|---|---|---|
|Window::MouseMove|MouseArgs|int deltaX, int deltaY|
|Window::MouseLeftButtonDown|ModArgs|int mods|
|Window::MouseLeftButtonUp|ModArgs|int mods|
|Window::MouseRightButtonDown|ModArgs|int mods|
|Window::MouseRightButtonUp|ModArgs|int mods|
|Window::KeyDown|KeyArgs|int key, int mods|
|Window::KeyUp|KeyArgs|int key, int mods|
|Window::Resize|---|nullptr|
|Button::OnClick|---|nullptr|
|Button::OnRelease|---|nullptr|

<a name="Shipped">

## Shipped classes

EasyGL ships four classes as default: Image, Text, Button and Mouse. Three are deriven from Shader as they are rendered to the screen and have a Shader::Show() function to update/redraw them.

### Image class

Image has two functions: LoadImage(const char* path) and Show().

LoadImage(const char* path) loads an image using the path passed to it. This may take a bit depending on size/file format.

Show() is the default function required by Shader in order to render the Image to the screen.

### Constructor

The Image constructor takes in four arguments: Image path, x, y, parent window.

```C
Image img = Image("Example.jpg", 0, 0, &parentWindow);
```

The parent window should be set to the Window it is rendered in. This is because it needs to keep its relative position to it.

To resize the image you can use the float array scale(size 3) of the shader to rescale it. Keep in mind that the scale is normalized to that axis, meaning: a width of 800 pixels and a height of 400 still keeps the scale at {1, 1, 1}! It does not change it to {2, 1, 1}!

### Variables

### Protected

|Name|Use|
|---|---|
|parent|A Window pointer pointing to the EasyGl Window parent|
|texture|A GLuint containing the loaded image texture|
|x|The x position of the image(in pixels from the bottom left)|
|y|The y position of the image(in pixels from the bottom left)|
|dx|The scaled width of the image|
|dy|The scaled height of the image|

### Public

|Name|Use|
|---|---|
|width|The width of the image|
|height|The height of the image|

### Text class

The Text class is just a default class with the default Show() function provided by the Shader class.

Quick tip: one text instance can draw multiple different things to the screen i.e.: should you want to draw "Hello World" in red to the screen but also want a shadow underneath it you can change the color of it to black set it to an offset call Show() change the color and position back and call Show() again.
This will cause a shadow effect using a single text instance.

```C
//some code...

//Draw shadow
myText.x += 5;
myText.y += 5;
myText.color[0] = 0;
myText.color[1] = 0;
myText.color[2] = 0;

myText.Show();

//Revert all changes and draw text
myText.x -= 5;
myText.y -= 5;
myText.color[0] = 255;
myText.color[1] = 0;
myText.color[2] = 0;

myText.Show();
```

### Constructor

The Text constructor takes in four Parameters the path to the font to use, the font size, the default Text and the parent window.

Just as with the image the parent window should be the window its rendered in because it needs to reposition and scale accordingly to it.

```C
Text myText = Text("ariel.ttf", 1, "Example", &parentWindow);
```

### Variables

### Protected

|Name|Use|
|---|---|
|parent|A Window* to the EasyGL Window parent|
|tex_uniform|The OpenGL texture that gets sampled for displaying characters|
|color_uniform|The attribute location to the OpenGL vec3 of the text color|
|coord_uniform|The attribute location of the text location on the screen|
|vbo|The vertex buffer object|
|vao|The vertex array object|

### Public

|Name|Use|
|---|---|
|color|A float array(size 3) of the text color|
|texture|A GLuint to the ttf texture for the font|
|fl|A FT_Library object, doesn't need to be managed by user|
|ff|A FT_Face object, doesn't need to be managed by user|
|fg|A FT_GlyphSlot object, doesn't need to be managed by user|
|fontSize|The fontSize of this Text object|
|chars|The loaded characters. Currently only supports ASCII but you can expand it by inheriting from Text and adding a funtion to load another Encoding|
|txt|A const char* to the text currently displayed
|x|The x position of the text|
|y|The y position of the text|
|width|The width of the text(in pixels)|
|height|The height of the text(in pixels)|

### Button class

The Button class displays a default button with two functions: Show() and CenterText().

The Show() function is the overriden shader function displaying the button and the text.

The CenterText() function centers the Text over the button. !IMPORTANT! The Button does not clip the Text! Should the text be longer than the button is wide it will overflow.

### Constructor

The Button takes in six parameters: The default text, the x position, the y position, the width, the height and a pointer to the parent window.

```C
Button myButton = Button("Button text", 0, 0, 150, 50, &parentWindow);
```

### Variables

### Protected

|Name|Use|
|---|---|
|parent|A pointer to the parent window|
|txt|The Text object displaying the button text|
|pressed|An integer signaling wether the button is pressed or not|
|click|A MouseClick object(deriven from IEventListener) to catch the OnMouseLeftButtonDown event|
|release|A MouseRelease object(deriven from IEventListener) to catch the OnMouseLeftButtonUp event|
|currentColor|A float array(size 3) containing the current background color|

### Public

|Name|Use|
|---|---|
|backgroundColor|The background color of the button when not pressed|
|pressedColor|The background color of the button when pressed|
|text|A const char* to the current button text|
|x|The current x coordinate|
|y|The current y coordinate|
|width|The current width|
|height|The current height|
|OnClick|The EventHandler firing when the button is clicked|
|OnRelease|The EventHandler firing when the button is released|

### Mouse class

The Mouse class is not a shader but holds a static X and Y position of the cursor on the window.

```C
double x = Mouse::X;
double y = Mouse::Y;
```

### Variables

### Public

|Name|Use|
|---|---|
|X|Current x coordinate of the cursor|
|Y|Current y coordinate of the cursor|

<a name="Create">

## Create a class deriven from Shader

### Setup

_WARNING: EasyGL is not yet optimized for 3d rendering. My main focus was on making a framework suitable for creating crude apps and to have as much freedom as possible while making a framework to provide basic elements! This is purely an example and not meant to be used._

If you, for example, want to create a class able to read and render .obj files you can create and use it rather simple:

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
protected:
	float* verts;

	float* LoadFile(const char* path)
	{
		//Load the file here ...
		//Also set the Shader::num variable here
	}

public:
	
	Obj(const char* path):Shader("", "")
	{
		verts = LoadFile(path);
		memcpy(verticies, verts, sizeof(float)*num);
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
	float* verts;

	float* LoadFile(const char* path)
	{
		//Load the file here ...
	}

public:
	
	Obj(const char* path):Shader("objShader.vert", "objShader.frag")
	{
		verts = LoadFile(path);
		memcpy(verticies, verts, sizeof(float)*num);
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
	float* verts;

	float* LoadFile(const char* path)
	{
		//Load the file here ...
	}

public:
	
	Obj(const char* path):Shader("objShader.vert", "objShader.frag")
	{
		verts = LoadFile(path);
		memcpy(verticies, verts, sizeof(float)*num);
	}
	
	void Show()
	{
		//Render your object here ...
	}
	
};
```

I'm not gonna show any examples for the render process nor the shaders as you're going to find 1001 examples on google that will work just fine.
