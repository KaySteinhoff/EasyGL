#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct
{
	GLuint texture;
	float sx, sy;
	float bx, by;
	unsigned int advance;
}Character;

class Text : public Shader
{
	void LoadASCII()
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		chars = (Character*)malloc(128*sizeof(Character));

		for(unsigned char c = 0; c < 128; ++c)
		{
			if(FT_Load_Char(ff, c, FT_LOAD_RENDER))
			{
				printf("Failed to load glyph %c", c);
				continue;
			}

			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
			  GL_TEXTURE_2D,
			  0,
			  GL_RED,
			  ff->glyph->bitmap.width,
			  ff->glyph->bitmap.rows,
			  0,
			  GL_RED,
			  GL_UNSIGNED_BYTE,
			  ff->glyph->bitmap.buffer
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			chars[c].texture = texture;
			chars[c].sx = ff->glyph->bitmap.width;
			chars[c].sy = ff->glyph->bitmap.rows;
			chars[c].bx = ff->glyph->bitmap_left;
			chars[c].by = ff->glyph->bitmap_top;
			chars[c].advance = ff->glyph->advance.x;		
		}

		FT_Done_Face(ff);
		FT_Done_FreeType(fl);
	}

	Window* parent;
	
public:
	//Shader
	GLfloat color[3];
	GLuint texture;
	
	//Freetype related variables
	FT_Library fl;
	FT_Face ff;
	FT_GlyphSlot fg;
	
	const char* font;
	int fontSize;
	float sx, sy;
	Character* chars;
	
	//OpenGL related variables
	GLuint tex;
	GLuint tex_uniform;
	GLuint color_uniform;
	GLuint coord_attribute;
	
	GLuint vbo, vao;
	
	const char* txt;
	int x, y;
	
	Text(const char* font, int fontSize, const char* txt, Window* parent) : Shader("include/textShader.vert", "include/textShader.frag")
	{
		this->fontSize = fontSize;
		this->txt = txt;
		this->parent = parent;
		
		if(FT_Init_FreeType(&fl))
		{
			printf("Could not load freetype2!\n");
			return;
		}

		if(FT_New_Face(fl, font, 0, &ff))
		{
			printf("Could not open font!\n");
			return;
		}

		FT_Set_Pixel_Sizes(ff, 0, 48);

		if(FT_Load_Char(ff, 'X', FT_LOAD_RENDER))
		{
			printf("Could not load character 'X'!\n");
			return;
		}

		tex_uniform = glGetUniformLocation(id, "tex");
		coord_attribute = glGetAttribLocation(id, "coord");
		color_uniform = glGetUniformLocation(id, "color");

		LoadASCII();

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	
	void Show()
	{
		mat4x4 proj;
	
		glUseProgram(id);
		glBindVertexArray(vao);
	
		mat4x4_ortho(proj, 0, parent->width, 0, parent->height, -1, 1);
		glUniformMatrix4fv(glGetUniformLocation(id, "p"), 1, GL_FALSE, (const GLfloat*)proj);
	
		glUniform3f(glGetUniformLocation(id, "color"), color[0], color[1], color[2]);

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(vao);

		const char* p;
		for(p = txt; *p; p++)
		{
			Character ch = chars[(unsigned int)*p];

			float xpos = x + ch.bx*fontSize;
			float ypos = y - (ch.sy - ch.by) * fontSize;

			float w = ch.sx * fontSize;
			float h = ch.sy * fontSize;

			float verts[6][4] = {
				{xpos    , ypos + h, 0, 0},
				{xpos    , ypos    , 0, 1},
				{xpos + w, ypos    , 1, 1},

				{xpos    , ypos + h, 0, 0},
				{xpos + w, ypos    , 1, 1},
				{xpos + w, ypos + h, 1, 0}
			};

			glBindTexture(GL_TEXTURE_2D, ch.texture);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			x += (ch.advance >> 6) * fontSize;
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
};

/*int init_text(Text *text)
{
	if(FT_Init_FreeType(&text->fl))
	{
		printf("Could not load freetype2!\n");
		return 1;
	}
	
	if(FT_New_Face(text->fl, text->font, 0, &text->ff))
	{
		printf("Could not open font!\n");
		return 1;
	}
	
	FT_Set_Pixel_Sizes(text->ff, 0, 48);
	
	if(FT_Load_Char(text->ff, 'X', FT_LOAD_RENDER))
	{
		printf("Could not load character 'X'!\n");
		return 1;
	}
		
	text->tex_uniform = glGetUniformLocation(text->shader.id, "tex");
	text->coord_attribute = glGetAttribLocation(text->shader.id, "coord");
	text->color_uniform = glGetUniformLocation(text->shader.id, "color");
	
	LoadASCII(text);
	
	glGenVertexArrays(1, &text->vao);
	glGenBuffers(1, &text->vbo);
	
	glBindVertexArray(text->vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, text->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return 0;

}

/*void renderText(Text* text, const char* t, float x, float y)
{
	glUseProgram(text->shader.id);
	glUniform3f(glGetUniformLocation(text->shader.id, "color"), text->color[0], text->color[1], text->color[2]);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(text->vao);
	
	const char* p;
	for(p = t; *p; p++)
	{
		Character ch = text->chars[(unsigned int)*p];
		
		float xpos = x + ch.bx*text->fontSize;
		float ypos = y - (ch.sy - ch.by) * text->fontSize;
		
		float w = ch.sx * text->fontSize;
		float h = ch.sy * text->fontSize;
		
		float verts[6][4] = {
			{xpos    , ypos + h, 0, 0},
			{xpos    , ypos    , 0, 1},
			{xpos + w, ypos    , 1, 1},
			
			{xpos    , ypos + h, 0, 0},
			{xpos + w, ypos    , 1, 1},
			{xpos + w, ypos + h, 1, 0}
		};
		
		glBindTexture(GL_TEXTURE_2D, ch.texture);
		glBindBuffer(GL_ARRAY_BUFFER, text->vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		x += (ch.advance >> 6) * text->fontSize;
	}
	
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}*/