#include "window.h"

class Image : public Shader
{
	Window* parent;

public:
	float x, y;
	
	Image(const char* path, int x, int y, Window* parent, const char* vertexShader, const char* fragmentShader) : Shader(vertexShader, fragmentShader)
	{	
		this->parent = parent;
		position[0] = x;
		position[1] = y;
	
		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		//set texture wrapping/filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//load and generate texture
		int width, height, channels;
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

		mat4x4 t;
		mat4x4_identity(t);
		mat4x4_identity(this->transform);
		mat4x4_scale_aniso(this->transform, t, width/(float)height, 1.0, 1.0);

		this->x = this->transform[0][0];
		this->y = this->transform[1][1];

		stbi_image_free(data);
		
		printf("done");
	}
	
	void Show() 
	{
		glUseProgram(this->id);
		mat4x4 tmp;

		mat4x4_identity(tmp);
		mat4x4_identity(this->transform);
		mat4x4_scale_aniso(this->transform, tmp, x*this->zoom, y*this->zoom, 1.0);

		this->ShaderUpdate(this->id, this->transform);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glBindVertexArray(this->vao);

		glDrawArrays(GL_TRIANGLES, 0, this->num);
	}
	
};