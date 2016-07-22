/*     _________  ___
 _____ \_   /\  \/  / common/OpenGL.hpp
|  |  |_/  /__>    <  Copyright © 2014-2016 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef __mZX_common_OpenGL_HPP
#define __mZX_common_OpenGL_HPP

#include <Z/inspection/OS.h>
#include <Z/types/base.hpp>
#include <string>

#if Z_OS == Z_OS_MAC_OS_X

#	include <OpenGL/gl.h>
#	include <OpenGL/OpenGL.h>

#	define OPEN_GL

#elif Z_OS == Z_OS_IOS

#	include <OpenGLES/ES2/gl.h>
#	include <OpenGLES/ES2/glext.h>

#	define OPEN_GL_ES

#elif Z_OS == Z_OS_LINUX

#	define GL_GLEXT_PROTOTYPES
#	include <GL/gl.h>
#	include <GL/glext.h>

#	define OPEN_GL

#endif


GLuint compile_shader	(Zeta::Character const *source_code,
				 GLenum type,
				 std::string **error_log);


#endif // __mZX_common_OpenGL_HPP
