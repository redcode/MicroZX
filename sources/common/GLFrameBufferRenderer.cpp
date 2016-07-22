/*     _________  ___
 _____ \_   /\  \/  / common/GLFrameBufferRenderer.c
|  |  |_/  /__>    <  Copyright © 2014-2016 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include "GLFrameBufferRenderer.hpp"
#include <cstdlib>
#include <cstring>
#include <string>

using namespace Zeta;
using namespace std;

static GLfloat const vertices[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};


GLFrameBufferRenderer::GLFrameBufferRenderer()
: _vertex_shader(0), _fragment_shader(0), _shader_program(0), content_scaling(Z_SCALING_FIT)
	{
	buffer.buffers[0] = nullptr;

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Nedded?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Nedded?
	glDisable(GL_TEXTURE_2D);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 4, vertices, GL_STATIC_DRAW);
 	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);
	}


GLFrameBufferRenderer::~GLFrameBufferRenderer()
	{
	if (_shader_program ) glDeleteProgram(_shader_program );
	if (_vertex_shader  ) glDeleteShader (_vertex_shader  );
	if (_fragment_shader) glDeleteShader (_fragment_shader);
	glDeleteTextures(1, &_texture);
	free(buffer.buffers[0]);
	}


void GLFrameBufferRenderer::set_resolution(Value2D<Size> resolution)
	{
	Size frame_buffer_size = resolution.inner_product() * sizeof(UInt32);

	buffer.initialize
		(buffer.buffers[0] = realloc(buffer.buffers[0], frame_buffer_size * 3),
		 frame_buffer_size);

	glEnable(GL_TEXTURE_2D);

	glTexImage2D
		(GL_TEXTURE_2D, 0, GL_RGBA,
		 input_width  = GLsizei(resolution.x),
		 input_height = GLsizei(resolution.y),
		 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glDisable(GL_TEXTURE_2D);
	}


void GLFrameBufferRenderer::set_content_bounds(Rectangle<Real> bounds)
	{
	content_bounds = bounds;

//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();

	_transform = Matrix4x4<float>::translation
		(-1.0f + (bounds.point.x + bounds.size.x / 2.0f) / viewport.size.x * 2.0f,
		 -1.0f + (bounds.point.y + bounds.size.y / 2.0f) / viewport.size.y * 2.0f);

	/*glScalef(bounds.size.x / viewport.size.x,
		 bounds.size.y / viewport.size.y,
		 1.0f);*/

//	glGetFloatv(GL_MODELVIEW_MATRIX, _transform);
	}


void GLFrameBufferRenderer::set_content_size(Value2D<Real> size)
	{
	content_bounds = Rectangle<Real>(viewport.size - size / 2.0, size);

	_transform = _transform = Matrix4x4<float>::scaling
		(content_bounds.size.x / viewport.size.x,
		 content_bounds.size.y / viewport.size.y);
	}


void GLFrameBufferRenderer::set_geometry(Rectangle<Real> viewport, ZKey(SCALING) content_scaling)
	{
	this->viewport = viewport;
	glViewport(viewport.point.x, viewport.point.y, viewport.size.x, viewport.size.y);

	switch (content_scaling ? (this->content_scaling = content_scaling) : this->content_scaling)
		{
		case Z_SCALING_FIT:
		set_content_size(Value2D<Real>(input_width, input_height).fit(viewport.size));
		break;

		case Z_SCALING_NONE:
		set_content_bounds(content_bounds);
		break;

		case Z_SCALING_EXPAND:
		_transform.set_identity();
		//_transform = _transform.translated(1.0, 1.0);
		break;
		}
	}


void GLFrameBufferRenderer::set_linear_interpolation(Boolean value)
	{
	GLint filter = value ? GL_LINEAR : GL_NEAREST;

	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	}


void GLFrameBufferRenderer::draw(Boolean skip_old)
	{
	void *frame = buffer.consume();

	if (!frame)
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

	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glUniformMatrix4fv(_transform_uniform, 1, GL_FALSE, _transform.m);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glFlush();
	}


Boolean GLFrameBufferRenderer::set_vertex_shader(Character *source_code, string **error_log)
	{
	if (_vertex_shader) glDeleteShader(_vertex_shader);
	return !!(_vertex_shader = compile_shader(source_code, GL_VERTEX_SHADER, error_log));
	}


Boolean GLFrameBufferRenderer::set_fragment_shader(Character *source_code, string **error_log)
	{
	if (_fragment_shader) glDeleteShader(_fragment_shader);
	return !!(_fragment_shader = compile_shader(source_code, GL_FRAGMENT_SHADER, error_log));
	}


void GLFrameBufferRenderer::create_shader_program()
	{
	_shader_program = glCreateProgram();

	glAttachShader(_shader_program, _vertex_shader	);
	glAttachShader(_shader_program, _fragment_shader);
	glLinkProgram (_shader_program);
	glUseProgram  (_shader_program);

 	_transform_uniform = glGetUniformLocation(_shader_program, "transform");
	}


void GLFrameBufferRenderer::destroy_shader_program()
	{
	glDetachShader(_shader_program, _fragment_shader);
	glDetachShader(_shader_program, _vertex_shader);
	glDeleteProgram(_shader_program);
	_shader_program = 0;
	}


/* GLFrameBufferRenderer.c EOF */
