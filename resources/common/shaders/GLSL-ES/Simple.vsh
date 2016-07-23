//#version 110

attribute vec2 vertex;
uniform mat4 transform;
varying vec2 texture_point;

void main(void)
	{
	gl_Position = vec4(vertex.x, vertex.y, 0.0, 1.0) * transform;
	texture_point = vec2(vertex.x, -vertex.y) * vec2(0.5) + vec2(0.5);
	}
