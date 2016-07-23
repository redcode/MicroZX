#version 110

uniform mat4 transform;
varying vec2 texture_point;

void main(void)
	{
	gl_Position = gl_Vertex * transform;
	texture_point = vec2(gl_Vertex.x, -gl_Vertex.y) * vec2(0.5) + vec2(0.5);
	}
