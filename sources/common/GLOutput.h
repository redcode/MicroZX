/*     _________  ___
 _____ \_   /\  \/  / common/GLOutput.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef __mZX_common_GLOutput_H
#define __mZX_common_GLOutput_H

#include "OpenGL.h"
#include <Z/types/mathematics.h>
#include <Z/types/buffering.h>
#include <Z/keys/layout.h>
#include <Z/macros/key.h>

typedef struct {
	ZString* name;
	zsize	 owner_count;
	zsize	 context_size;
	ZString* vertex_shader_file_name;
	ZString* vertex_shader_source;
	GLint	 vertex_shader_source_size;
	GLuint	 vertex_shader;
	ZString* fragment_shader_file_name;
	ZString* fragment_shader_source;
	GLint	 fragment_shader_source_size;
	GLuint	 fragment_shader;
	GLuint	 program;
	GLint	 transform_uniform;
	GLint	 vertex_attribute;
	GLint	 texture_uniform;

	struct {//void (* initialize)

		void (* draw) (void*   context,
			       GLsizei texture_width,
			       GLsizei texture_height);
	} abi;
} GLOutputEffect;

typedef struct {
	GLsizei	        input_width;
	GLsizei		input_height;
	ZTripleBuffer	buffer;
	ZRectangle      viewport;
	ZRectangle      content_bounds;
	ZKey(SCALING)   content_scaling;
	GLfloat	        model_view_matrix[16];
	GLuint		texture;
	GLOutputEffect* effect;
	void*		effect_context;
} GLOutput;

#define GL_OUTPUT(p) ((GLOutput *)(p))

Z_C_SYMBOLS_BEGIN

void gl_output_initialize	  (GLOutput*	   object);

void gl_output_finalize		  (GLOutput*	   object);

void gl_output_set_resolution	  (GLOutput*	   object,
				   Z2DSize	   resolution);

void gl_output_set_content_bounds (GLOutput*	   object,
				   ZRectangle	   bounds);

void gl_output_set_content_size	  (GLOutput*	   object,
				   Z2D		   size);

void gl_output_set_geometry	  (GLOutput*	   object,
				   ZRectangle	   viewport,
				   ZKey(SCALING)   content_scaling);

void gl_output_set_effect	  (GLOutput*	   object,
				   GLOutputEffect* effect,
				   void*	   effect_context);

void gl_output_remove_effect	  (GLOutput*	   object);

void gl_output_draw		  (GLOutput*	   object,
				   zboolean	   skip_old);

void gl_output_set_linear_interpolation(GLOutput *object, zboolean value);

Z_C_SYMBOLS_END

#endif /* __mZX_common_GLOutput_H */
