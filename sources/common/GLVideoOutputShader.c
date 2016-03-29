/* ACME - GLVideoOutputShader.c
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#include "OpenGL.h"
#include <Z/types/base.h>

/*

#version 110
uniform vec2 textureSize;
uniform mat4 transform;
attribute vec2 vertex;
varying vec2 texcoord;

void main()
	{
	gl_Position = transform * vec4(vertex.x, -vertex.y, 0.0, 1.0);
	texcoord = vertex * vec2(0.5) + vec2(0.5);
	};

#version 110
uniform vec2 textureSize;
uniform sampler2D texture;
varying vec2 texcoord;

void main()
	{
	gl_FragColor = texture2D(texture, texcoord);
	};


*/


