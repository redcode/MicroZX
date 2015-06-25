/*	_________  ___
  _____ \_   /\  \/  /	common/OpenGL.h
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#ifndef __mZX_common_OpenGL_H
#define __mZX_common_OpenGL_H

#include <Q/inspection/OS.h>

#if Q_OS == Q_OS_MAC_OS_X

#	include <OpenGL/gl.h>
#	include <OpenGL/OpenGL.h>

	typedef CGLContextObj OpenGLContext;

#	define OPEN_GL_CONTEXT_SET_CURRENT(context) CGLSetCurrentContext(context)
#	define OPEN_GL_CONTEXT_RESTORE		    CGLSetCurrentContext(NULL)

#else
#	include <GL/gl.h>
#endif


#endif /* __mZX_common_OpenGL_H */
