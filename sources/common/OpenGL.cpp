/*     _________  ___
 _____ \_   /\  \/  / common/OpenGL.cpp
|  |  |_/  /__>    <  Copyright © 2014-2016 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include "OpenGL.hpp"

using namespace Zeta;
using namespace std;


GLuint compile_shader(Character const *source_code, GLenum type, string **error_log)
	{
	GLuint id = glCreateShader(type);
	GLint source_code_size = GLint(strlen(source_code));
	glShaderSource(id, 1, (const GLchar* const *)&source_code, &source_code_size);
	glCompileShader(id);

	GLint success;

	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
		{
		if (error_log)
			{
			GLint log_size;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_size);
			*error_log = new string(log_size, ' ');
			glGetShaderInfoLog(id, log_size, &log_size, (GLchar *)(*error_log)->data());
			}

		glDeleteShader(id);
		return 0;
		}

	return id;
	}
