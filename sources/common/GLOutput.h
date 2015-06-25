/*	_________  ___
  _____ \_   /\  \/  /	common/GLOutput.h
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#ifndef __mZX_common_GLOutput_H
#define __mZX_common_GLOutput_H

#include <Q/types/mathematics.h>
#include <Q/types/buffering.h>
#include <Q/keys/layout.h>
#include <Q/macros/key.h>
#include "OpenGL.h"

typedef struct {
	QString* name;
	qsize	 owner_count;
	qsize	 context_size;
	QString* vertex_shader_file_name;
	QString* vertex_shader_source;
	GLint	 vertex_shader_source_size;
	GLuint	 vertex_shader;
	QString* fragment_shader_file_name;
	QString* fragment_shader_source;
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
	OpenGLContext   context;
	GLsizei	        input_width;
	GLsizei		input_height;
	QTripleBuffer*  input_buffer;
	QRectangle      viewport;
	QRectangle      content_bounds;
	QKey(SCALING)   content_scaling;
	qboolean        texture_loaded;
	GLfloat	        model_view_matrix[16];
	GLuint		texture;
	GLOutputEffect* effect;
	void*		effect_context;
} GLOutput;

#define GL_OUTPUT(p) ((GLOutput *)(p))

Q_C_SYMBOLS_BEGIN

void gl_output_initialize	  (GLOutput*	   object,
				   OpenGLContext   context);

void gl_output_finalize		  (GLOutput*	   object);

void gl_output_set_input	  (GLOutput*	   object,
				   QTripleBuffer*  buffer,
				   Q2DSize	   resolution);

void gl_output_set_content_bounds (GLOutput*	   object,
				   QRectangle	   bounds);

void gl_output_set_content_size	  (GLOutput*	   object,
				   Q2D		   size);

void gl_output_set_geometry	  (GLOutput*	   object,
				   QRectangle	   viewport,
				   QKey(SCALING)   content_scaling);

void gl_output_set_effect	  (GLOutput*	   object,
				   GLOutputEffect* effect,
				   void*	   effect_context);

void gl_output_remove_effect	  (GLOutput*	   object);

void gl_output_draw		  (GLOutput*	   object,
				   qboolean	   skip_old);

void gl_output_set_linear_interpolation(GLOutput *object, qboolean value);

Q_C_SYMBOLS_END

#endif /* __mZX_common_GLOutput_H */
