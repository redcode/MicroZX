/*     _________  ___
 _____ \_   /\  \/  / common/GLOutput.c
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include <stdlib.h>
#include "GLOutput.h"
#include <Z/functions/base/Z2DValue.h>
#include <Z/functions/base/Z3DValue.h>
#include <Z/functions/geometry/ZRectangle.h>
#include <Z/functions/buffering/ZTripleBuffer.h>

static const GLubyte indices_data[] = {0, 1, 2, 3};

static const GLfloat vertices_data[] = {
	-1.0f, -1.0f, 1.0f, -1.0f,
	-1.0f,  1.0f, 1.0f,  1.0f
};

static GLuint indices  = 0;
static GLuint vertices = 0;

static zsize  gpu_shared_data_owner_count = 0;


static void set_viewport(ZRectangle viewport)
	{
	glLoadIdentity();

	glViewport
		((GLsizei)viewport.point.x, (GLsizei)viewport.point.y,
		 (GLsizei)viewport.size.x,  (GLsizei)viewport.size.y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	}


void gl_output_initialize(GLOutput *object)
	{
	object->effect = NULL;
	object->buffer.buffers[0] = NULL;

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &object->texture);
	glBindTexture(GL_TEXTURE_2D, object->texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Nedded?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Nedded?
	glDisable(GL_TEXTURE_2D);
	}


void gl_output_finalize(GLOutput *object)
	{
	if (object->effect != NULL) gl_output_remove_effect(object);
	glDeleteTextures(1, &object->texture);
	free(object->buffer.buffers[0]);
	}


void gl_output_set_resolution(GLOutput *object, Z2DSize resolution)
	{
	zsize frame_buffer_size = resolution.x * resolution.y * 4;

	z_triple_buffer_initialize
		(&object->buffer,
		 object->buffer.buffers[0] = realloc(object->buffer.buffers[0], frame_buffer_size * 3),
		 frame_buffer_size);

	glEnable(GL_TEXTURE_2D);

	glTexImage2D
		(GL_TEXTURE_2D, 0, GL_RGBA,
		 object->input_width  = (GLsizei)resolution.x,
		 object->input_height = (GLsizei)resolution.y,
		 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glDisable(GL_TEXTURE_2D);
	}


void gl_output_set_content_bounds(GLOutput *object, ZRectangle bounds)
	{
	object->content_bounds = bounds;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslated
		(-1.0 + (bounds.point.x + bounds.size.x / 2.0) / object->viewport.size.x * 2.0,
		 -1.0 + (bounds.point.y + bounds.size.y / 2.0) / object->viewport.size.y * 2.0,
		 0.0);

	glScaled(bounds.size.x / object->viewport.size.x,
		 bounds.size.y / object->viewport.size.y,
		 1.0);

	glGetFloatv(GL_MODELVIEW_MATRIX, object->model_view_matrix);
	}


void gl_output_set_content_size(GLOutput *object, Z2D size)
	{
	ZRectangle bounds;

	bounds.point = z_2d_divide_by_scalar(z_2d_subtract(object->viewport.size, size), 2.0);
	bounds.size = size;
	object->content_bounds = bounds;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glScaled(bounds.size.x / object->viewport.size.x,
		 bounds.size.y / object->viewport.size.y,
		 1.0);

	glGetFloatv(GL_MODELVIEW_MATRIX, object->model_view_matrix);
	}


void gl_output_set_geometry(
	GLOutput*     object,
	ZRectangle    viewport,
	ZKey(SCALING) content_scaling
)
	{
	set_viewport(object->viewport = viewport);

	switch (content_scaling ? (object->content_scaling = content_scaling) : object->content_scaling)
		{
		case Z_SCALING_FIT:
		gl_output_set_content_size
			(object,
			 z_2d_fit(z_2d((zreal)object->input_width, (zreal)object->input_height), viewport.size));
		break;

		case Z_SCALING_NONE:
		gl_output_set_content_bounds(object, object->content_bounds);
		break;

		case Z_SCALING_EXPAND:
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, object->model_view_matrix);
		break;
		}
	}


void gl_output_set_linear_interpolation(GLOutput *object, zboolean value)
	{
	GLint filter = value ? GL_LINEAR : GL_NEAREST;

	glBindTexture(GL_TEXTURE_2D, object->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	}


void gl_output_set_effect(GLOutput *object, GLOutputEffect *effect, void *effect_context)
	{
	if (object->effect != effect)
		{
		gl_output_remove_effect(object);

		if (!gpu_shared_data_owner_count++)
			{
			glGenBuffers(1, &vertices);
			glBindBuffer(GL_ARRAY_BUFFER, vertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_data), vertices_data, GL_STATIC_DRAW);

			glGenBuffers(1, &indices);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_data), indices_data, GL_STATIC_DRAW);
			}

		object->effect_context = effect_context;

		if (!(object->effect = effect)->owner_count++)
			{
			GLuint vertex_shader;
			GLuint fragment_shader;
			GLuint program;
			GLint ok;

			/*----------------------.
			| Create vertex shader. |
			'----------------------*/
			glShaderSource
				(vertex_shader = glCreateShader(GL_VERTEX_SHADER), 1,
				 (const GLchar * const *)&effect->vertex_shader_source,
				 &effect->vertex_shader_source_size);

			glCompileShader(vertex_shader);
			glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &ok);
			//if (!ok) printf("Error con el shader\n");
			effect->vertex_shader = vertex_shader;

			/*------------------------.
			| Create fragment shader. |
			'------------------------*/
			glShaderSource
				(fragment_shader = glCreateShader(GL_FRAGMENT_SHADER), 1,
				 (const GLchar * const *)&effect->fragment_shader_source,
				 &effect->fragment_shader_source_size);

			glCompileShader(fragment_shader);
			glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &ok);
			//if (!ok) printf("Error con el shader\n");
			effect->fragment_shader = fragment_shader;

			/*-----------------------------.
			| Create final shader program. |
			'-----------------------------*/
			program = glCreateProgram();
			glAttachShader(program, vertex_shader);
			glAttachShader(program, fragment_shader);
			glLinkProgram(program);
			glGetProgramiv(program, GL_LINK_STATUS, &ok);
			//if (!ok) printf("Error con el programa del shader\n");
			effect->program = program;

			effect->transform_uniform = glGetUniformLocation(program, "transform");
			effect->vertex_attribute  = glGetAttribLocation (program, "vertex"   );
			effect->texture_uniform   = glGetUniformLocation(program, "texture");
			}
		}
	}


void gl_output_remove_effect(GLOutput *object)
	{
	if (object->effect != NULL)
		{
		GLOutputEffect *effect = object->effect;

		if (!--effect->owner_count)
			{
			glDeleteProgram (effect->program);
			glDeleteShader	(effect->fragment_shader);
			glDeleteShader	(effect->vertex_shader);
			}

		if (!--gpu_shared_data_owner_count)
			{
			glDeleteBuffers(1, &indices);
			glDeleteBuffers(1, &vertices);
			}

		object->effect = NULL;
		}
	}


void gl_output_draw(GLOutput *object, zboolean skip_old)
	{
	void *frame = z_triple_buffer_consume(&object->buffer);

	if (frame == NULL)
		{
		if (skip_old) return;
		frame = z_triple_buffer_consumption_buffer(&object->buffer);
		}

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glTexSubImage2D
		(GL_TEXTURE_2D, 0, 0, 0,
		 object->input_width, object->input_height,
		 GL_RGBA, GL_UNSIGNED_BYTE, frame);

	if (object->content_scaling != Z_SCALING_EXPAND)
		{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		}

	if (object->effect == NULL)
		{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, object->texture);

		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f);
		glEnd();
		}

	else	{
		GLOutputEffect *effect = object->effect;

		glUseProgram(effect->program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, object->texture);
		glUniformMatrix4fv(effect->transform_uniform, 1, GL_FALSE, object->model_view_matrix);

		glBindBuffer(GL_ARRAY_BUFFER, vertices);
		glVertexAttribPointer(effect->vertex_attribute, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void *)0);
		glEnableVertexAttribArray(effect->vertex_attribute);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);

		if (effect->abi.draw == NULL) glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (void *)0);
		else effect->abi.draw(object->effect_context, object->input_width, object->input_height);

		glDisableVertexAttribArray(effect->vertex_attribute);
		}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glFlush();
	}


/* GLOutput.c EOF */
