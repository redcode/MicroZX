/*     _________  ___
 _____ \_   /\  \/  / common/OpenGL.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef __mZX_common_OpenGL_H
#define __mZX_common_OpenGL_H

#include <Z/inspection/OS.h>
#undef Z_OS_SYMBIAN
#if Z_OS == Z_OS_MAC_OS_X

#	include <OpenGL/gl.h>
#	include <OpenGL/OpenGL.h>

#elif Z_OS == Z_OS_IOS

#	include <OpenGLES/ES2/gl.h>
#	include <OpenGLES/ES2/glext.h>

#	define USE_GL_ES

#elif Z_OS == Z_OS_LINUX

#	define GL_GLEXT_PROTOTYPES
#	include <GL/gl.h>
#	include <GL/glext.h>

#endif

#endif /* __mZX_common_OpenGL_H */
