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

typedef struct
{
	//Shader
	Shader shader;
	GLfloat color[3];
	
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
	
}Text;

void LoadASCII(Text* text)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	text->chars = malloc(128*sizeof(Character));
	
	for(unsigned char c = 0; c < 128; ++c)
	{
		if(FT_Load_Char(text->ff, c, FT_LOAD_RENDER))
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
        text->ff->glyph->bitmap.width,
        text->ff->glyph->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        text->ff->glyph->bitmap.buffer
   	);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		text->chars[c].texture = texture;
		text->chars[c].sx = text->ff->glyph->bitmap.width;
		text->chars[c].sy = text->ff->glyph->bitmap.rows;
		text->chars[c].bx = text->ff->glyph->bitmap_left;
		text->chars[c].by = text->ff->glyph->bitmap_top;
		text->chars[c].advance = text->ff->glyph->advance.x;		
	}
	
	FT_Done_Face(text->ff);
	FT_Done_FreeType(text->fl);
}

int init_text(Text *text)
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
	
	LoadShader(&text->shader, "include/textShader.vert", "include/textShader.frag");
	
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

}

void renderText(Text* text, const char* t, float x, float y)
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
}