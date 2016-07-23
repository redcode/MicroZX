//#version 110

uniform sampler2D texture;
varying lowp vec2 texture_point;

void main(void)
	{
	gl_FragColor = texture2D(texture, texture_point);
 	}
