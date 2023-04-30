#version 110

attribute vec4 coord;
varying vec2 texcoord;
uniform mat4 p;

void main(void)
{
	gl_Position = p * vec4(coord.xy, 1, 1);
	texcoord = coord.zw;
}