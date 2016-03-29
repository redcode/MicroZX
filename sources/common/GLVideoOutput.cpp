/*     _________  ___
 _____ \_   /\  \/  / common/GLOutput.c
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include "GLVideoOutput.hpp"
#include <Z/functions/buffering/ZTripleBuffer.h>
#include <stdlib.h>

using namespace ZKit;

static GLubyte const indices_data[] = {0, 1, 2, 3};

static GLfloat const vertices_data[] = {
	-1.0f, -1.0f, 1.0f, -1.0f,
	-1.0f,  1.0f, 1.0f,  1.0f
};

static GLuint indices  = 0;
static GLuint vertices = 0;

static Size gpu_shared_data_owner_count = 0;


static void set_viewport(Rectangle<Real> viewport)
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


GLVideoOutput::GLVideoOutput() : effect(NULL), content_scaling(Z_SCALING_FIT)
	{
	buffer.buffers[0] = NULL;

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Nedded?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Nedded?
	glDisable(GL_TEXTURE_2D);
	}


GLVideoOutput::~GLVideoOutput()
	{
	if (effect != NULL) remove_effect();
	glDeleteTextures(1, &texture);
	free(buffer.buffers[0]);
	}


void GLVideoOutput::set_resolution(Value2D<Size> resolution)
	{
	Size frame_buffer_size = resolution.inner_product() * 4;

	buffer.initialize
		(buffer.buffers[0] = realloc(buffer.buffers[0], frame_buffer_size * 3),
		 frame_buffer_size);

	glEnable(GL_TEXTURE_2D);

	glTexImage2D
		(GL_TEXTURE_2D, 0, GL_RGBA,
		 input_width  = (GLsizei)resolution.x,
		 input_height = (GLsizei)resolution.y,
		 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glDisable(GL_TEXTURE_2D);
	}


void GLVideoOutput::set_content_bounds(Rectangle<Real> bounds)
	{
	content_bounds = bounds;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslated
		(-1.0 + (bounds.point.x + bounds.size.x / 2.0) / viewport.size.x * 2.0,
		 -1.0 + (bounds.point.y + bounds.size.y / 2.0) / viewport.size.y * 2.0,
		 0.0);

	glScaled(bounds.size.x / viewport.size.x,
		 bounds.size.y / viewport.size.y,
		 1.0);

	glGetFloatv(GL_MODELVIEW_MATRIX, model_view_matrix);
	}


void GLVideoOutput::set_content_size(Value2D<Real> size)
	{
	content_bounds = Rectangle<Real>(viewport.size - size / 2.0, size);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glScaled(content_bounds.size.x / viewport.size.x,
		 content_bounds.size.y / viewport.size.y,
		 1.0);

	glGetFloatv(GL_MODELVIEW_MATRIX, model_view_matrix);
	}


void GLVideoOutput::set_geometry(Rectangle<Real> viewport, ZKey(SCALING) content_scaling)
	{
	set_viewport(this->viewport = viewport);

	switch (content_scaling ? (this->content_scaling = content_scaling) : this->content_scaling)
		{
		case Z_SCALING_FIT:
		set_content_size(Value2D<Real>(input_width, input_height).fit(viewport.size));
		break;

		case Z_SCALING_NONE:
		set_content_bounds(content_bounds);
		break;

		case Z_SCALING_EXPAND:
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, model_view_matrix);
		break;
		}
	}


void GLVideoOutput::set_linear_interpolation(Boolean value)
	{
	GLint filter = value ? GL_LINEAR : GL_NEAREST;

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	}


void GLVideoOutput::set_effect(GLVideoOutputEffect *effect, void *effect_context)
	{
	if (this->effect != effect)
		{
		remove_effect();

		if (!gpu_shared_data_owner_count++)
			{
			glGenBuffers(1, &vertices);
			glBindBuffer(GL_ARRAY_BUFFER, vertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_data), vertices_data, GL_STATIC_DRAW);

			glGenBuffers(1, &indices);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_data), indices_data, GL_STATIC_DRAW);
			}

		this->effect_context = effect_context;

		if (!(this->effect = effect)->owner_count++)
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


void GLVideoOutput::remove_effect()
	{
	if (this->effect != NULL)
		{
		GLVideoOutputEffect *effect = this->effect;

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

		this->effect = NULL;
		}
	}


void GLVideoOutput::draw(Boolean skip_old)
	{
	void *frame = buffer.consume();

	if (frame == NULL)
		{
		if (skip_old) return;
		frame = buffer.consumption_buffer();
		}

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glTexSubImage2D
		(GL_TEXTURE_2D, 0, 0, 0,
		 input_width, input_height,
		 GL_RGBA, GL_UNSIGNED_BYTE, frame);

	if (content_scaling != Z_SCALING_EXPAND)
		{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		}

	if (this->effect == NULL)
		{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, texture);

		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f);
		glEnd();
		}

	else	{
		GLVideoOutputEffect *effect = this->effect;

		glUseProgram(effect->program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniformMatrix4fv(effect->transform_uniform, 1, GL_FALSE, model_view_matrix);

		glBindBuffer(GL_ARRAY_BUFFER, vertices);
		glVertexAttribPointer(effect->vertex_attribute, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void *)0);
		glEnableVertexAttribArray(effect->vertex_attribute);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);

		if (effect->abi.draw == NULL) glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (void *)0);
		else effect->abi.draw(effect_context, input_width, input_height);

		glDisableVertexAttribArray(effect->vertex_attribute);
		}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glFlush();
	}


/* GLOutput.c EOF */
