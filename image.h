#pragma once

class Image : public Shader
{
protected:
	Window* parent;
	GLuint texture;

	float x, y, dx, dy;

public:

	int width, height;

	Image(const char* path, float x, float y, Window* parent) : Shader("", "")
	{
		float nx, ny;
		this->x = x;
		this->y = y;
		
		stbi_set_flip_vertically_on_load(true);
		
		this->parent = parent;
		
		//Vertex shader
		LoadPartialShader(GL_VERTEX_SHADER, &this->vertexShader, "#version 110\nattribute vec3 aPos;attribute vec2 texCoord;uniform mat4 transform;uniform mat4 projection;varying vec2 tCoord;void main(){gl_Position = projection * transform * vec4(aPos, 1.0);tCoord = texCoord;}");

		//Fragment shader
		LoadPartialShader(GL_FRAGMENT_SHADER, &this->fragmentShader, "#version 110\nvarying vec2 tCoord;uniform sampler2D tex;void main(){gl_FragColor = texture2D(tex, tCoord) * vec4(1.0, 1.0, 1.0, 1.0);}");

		//Attaching them to the final Shader
		this->id = glCreateProgram();
		glAttachShader(this->id, this->vertexShader.id);
		glAttachShader(this->id, this->fragmentShader.id);
		glLinkProgram(this->id);

		//Reading infolog and success
		glGetProgramiv(this->id, GL_LINK_STATUS, &success);
		glGetProgramInfoLog(this->id, 512, NULL, infoLog);

		printf("%s\n", infoLog);
		
		position[0] = x;
		position[1] = y;
		
		
		LoadImage(path);
		
		float verts[] = {
			x      , y+height, 0.0, 0.0, 1.0,
			x+width, y+height, 0.0, 1.0, 1.0,
			x      , y       , 0.0, 0.0, 0.0,
			x      , y       , 0.0, 0.0, 0.0,
			x+width, y+height, 0.0, 1.0, 1.0,
			x+width, y       , 0.0, 1.0, 0.0
		};
				
		this->scale[0] = 1;
		this->scale[1] = 1;
		
		num = (sizeof(verts)/sizeof( verts[0]));
		
		verticies = (float*)malloc(num*sizeof(float));
		memcpy(verticies, verts, sizeof(verts));
		
		LoadShaderValues(GL_STATIC_DRAW);
		
		dx = width*scale[0];
		dy = height*scale[1];
	}
	
	void LoadImage(const char* path)
	{
		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		//set texture wrapping/filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//load and generate texture
		int channels;
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

		stbi_image_free(data);
	}
	
	void Show() 
	{
		glUseProgram(this->id);
		
		if(dx != width*scale[0] || dy != height*scale[1])
		{
			dx = width*scale[0];
			dy = height*scale[1];
		
			float verts[] = {
				x   , y+dy, 0.0, 0.0, 1.0,
				x+dx, y+dy, 0.0, 1.0, 1.0,
				x   , y   , 0.0, 0.0, 0.0,
				x   , y   , 0.0, 0.0, 0.0,
				x+dx, y+dy, 0.0, 1.0, 1.0,
				x+dx, y   , 0.0, 1.0, 0.0
			};
			
			num = (sizeof(verts)/sizeof( verts[0]));
			
			verticies = (float*)malloc(num*sizeof(float));
			memcpy(verticies, verts, sizeof(verts));
			
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num, verticies, GL_STATIC_DRAW);
		}
		
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
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glBindVertexArray(this->vao);

		glDrawArrays(GL_TRIANGLES, 0, this->num);
	}
	
};
