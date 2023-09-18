enum HorizontalAlignment
{
	Left,
	HCenter,
	Right
};

enum VerticalAlignment
{
	Top,
	VCenter,
	Bottom
};

class Button : public Shader
{
protected:
	
	Window* parent;
	Text txt = Text("arial.ttf", 24, "Button", parent);
	int pressed = 0;

	vec3 currentColor;
	
	void MouseDown(EventArgs* args)
	{
		if(Mouse::X < x && Mouse::Y < y)
			return;
		if(Mouse::X > x+width && Mouse::Y > y+height)
			return;
			
		pressed = 1;
		
		OnClick(nullptr);
	}
	
	void MouseUp(EventArgs* args)
	{
		if(!pressed)
			return;
		
		pressed = 0;
		
		OnRelease(nullptr);
	}
	
	void Left()
	{
		txt.x = x;
	}
	
	void HCenter()
	{
		int textMiddleX = txt.width >> 1;
		int buttonMiddleX = width >> 1;
		txt.x = x + (buttonMiddleX-textMiddleX);
	}
	
	void Right()
	{
		txt.x = (x + width)-txt.width;
	}
	
	void Top()
	{
		txt.y = (y+height)-txt.height;
	}
	
	void VCenter()
	{
		int buttonMiddleY = height >> 1;
		int textMiddleY = txt.height >> 1;
		txt.y = buttonMiddleY-textMiddleY;
	}
	
	void Bottom()
	{
		txt.y = y;
	}
	
public:

	vec3 backgroundColor = {1.0, 1.0, 1.0};
	vec3 pressedColor = {0.5, 0.5, 0.5};
	
	const char* text;
	
	float x, y;
	int width, height;
	
	EventHandler OnClick;
	EventHandler OnRelease;
	
	HorizontalAlignment horizontalAlignment = HorizontalAlignment::HCenter;
	VerticalAlignment verticalAlignment = VerticalAlignment::VCenter;
	
	Button(const char* text, float x, float y, int width, int height, Window* parent) : Shader("", "")
	{
		this->x = x;
		this->y = y;
		this->text = text;
		
		this->currentColor[0] = backgroundColor[0];
		this->currentColor[1] = backgroundColor[1];
		this->currentColor[2] = backgroundColor[2];
		
		txt = Text("arial.ttf", 24, text, parent);
		txt.x = x;
		txt.y = y;
		txt.color[0] = 0;
		txt.color[1] = 0;
		txt.color[2] = 0;

		this->parent = parent;
		
		this->parent->MouseLeftButtonDown.AddListener<Button>(this, &Button::MouseDown);
		this->parent->MouseLeftButtonUp.AddListener<Button>(this, &Button::MouseUp);
		
		this->width = width;
		this->height = height;
		
		//Vertex shader
		LoadPartialShader(GL_VERTEX_SHADER, &this->vertexShader, "#version 110\nattribute vec3 aPos;uniform mat4 transform;uniform mat4 projection;void main(){gl_Position = projection * transform * vec4(aPos, 1.0);}");
		
		//Fragment shader
		LoadPartialShader(GL_FRAGMENT_SHADER, &this->fragmentShader, "#version 110\nuniform vec3 color;void main(){gl_FragColor = vec4(color, 1.0);}");
		
		//Attaching them to the final Shader
		this->id = glCreateProgram();
		glAttachShader(this->id, this->vertexShader.id);
		glAttachShader(this->id, this->fragmentShader.id);
		glLinkProgram(this->id);
		
		//Reading infolog and success
		glGetProgramiv(this->id, GL_LINK_STATUS, &success);
		glGetProgramInfoLog(this->id, 512, NULL, infoLog);
		
		printf("%s\n", infoLog);
		
		float verts[] = {
			x      , y+height, 0.0, 0.0, 1.0,
			x+width, y+height, 0.0, 1.0, 1.0,
			x      , y       , 0.0, 0.0, 0.0,
			x      , y       , 0.0, 0.0, 0.0,
			x+width, y+height, 0.0, 1.0, 1.0,
			x+width, y       , 0.0, 1.0, 0.0
		};
		
		num = (sizeof(verts)/sizeof( verts[0]));
		
		verticies = (float*)malloc(num*sizeof(float));
		memcpy(verticies, verts, sizeof(verts));
		
		LoadShaderValues(GL_STATIC_DRAW);
	}
	
	void Show() 
	{
		glUseProgram(this->id);
		
		mat4x4 tmp;

		mat4x4_identity(tmp);
		mat4x4_identity(this->transform);
		mat4x4_scale_aniso(this->transform, tmp, 1.0, 1.0, 1.0);

		GLint attribLoc;
		mat4x4 proj;
		mat4x4_identity(proj);
		mat4x4_ortho(proj, 0, parent->width, 0, parent->height, -1, 1);

		attribLoc = glGetUniformLocation(id, "transform");
		glUniformMatrix4fv(attribLoc, 1, GL_FALSE, (const GLfloat*)transform);

		attribLoc = glGetUniformLocation(id, "projection");
		glUniformMatrix4fv(attribLoc, 1, GL_FALSE, (const GLfloat*)proj);
		
		attribLoc = glGetUniformLocation(id, "color");
		glUniform3f(attribLoc, currentColor[0], currentColor[1], currentColor[2]);
		
		glBindVertexArray(this->vao);

		glDrawArrays(GL_TRIANGLES, 0, this->num);
		
		if(text != txt.txt)
			txt.txt = text;
		
		Shader* textElement = &txt;
		textElement->Show();
		
		switch(horizontalAlignment)
		{
			case HorizontalAlignment::Left:
				Left();
				break;
			case HorizontalAlignment::HCenter:
				HCenter();
				break;
			case HorizontalAlignment::Right:
				Right();
				break;
		}
		
		switch(verticalAlignment)
		{
			case VerticalAlignment::Top:
				Top();
				break;
			case VerticalAlignment::VCenter:
				VCenter();
				break;
			case VerticalAlignment::Bottom:
				Bottom();
				break;
		}
		
	}

};