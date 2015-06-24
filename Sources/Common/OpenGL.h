/* ACME - OpenGL.h
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#ifndef __ACME_OpenGL_H__
#define __ACME_OpenGL_H__

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


#endif /* __ACME_OpenGL_H__ */
