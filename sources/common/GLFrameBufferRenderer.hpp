/*     _________  ___
 _____ \_   /\  \/  / common/GLFrameBufferRenderer.h
|  |  |_/  /__>    <  Copyright © 2014-2016 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef __mZX_common_GLFrameBufferRenderer_HPP
#define __mZX_common_GLFrameBufferRenderer_HPP

#include <Z/classes/buffering/TripleBuffer.hpp>
#include <Z/classes/mathematics/geometry/euclidean/Rectangle.hpp>
#include <Z/keys/layout.h>
#include <Z/macros/key.h>
#include "OpenGL.hpp"
#include "Matrix.hpp"

class GLFrameBufferRenderer {
	private:
	Zeta::Matrix4x4<float> _transform;
	GLuint	_texture;
	GLuint  _vbo;
	GLuint  _vertex_shader;
	GLuint	_fragment_shader;
	GLuint	_shader_program;
	GLuint  _index_buffer_id;
	//GLuint  _vertex_attribute_id;
	GLuint  _transform_uniform;
	//GLuint  _texture_uniform_id;
	//GLuint  _texture_size_uniform_id;

	public:
	GLsizei			    input_width;
	GLsizei			    input_height;
	Zeta::TripleBuffer	    buffer;
	Zeta::Rectangle<Zeta::Real> viewport;
	Zeta::Rectangle<Zeta::Real> content_bounds;
	ZKey(SCALING)		    content_scaling;

	GLFrameBufferRenderer();
	~GLFrameBufferRenderer();

	void set_resolution(Zeta::Value2D<Zeta::Size> resolution);
	void set_content_bounds(Zeta::Rectangle<Zeta::Real> bounds);
	void set_content_size(Zeta::Value2D<Zeta::Real> size);
	void set_geometry(Zeta::Rectangle<Zeta::Real> viewport, ZKey(SCALING) content_scaling);
	void draw(Zeta::Boolean skip_old);
	void set_linear_interpolation(Zeta::Boolean value);
	Zeta::Boolean set_vertex_shader(Zeta::Character *source_code, std::string **error_log);
	Zeta::Boolean set_fragment_shader(Zeta::Character *source_code, std::string **error_log);
	void create_shader_program();
	void destroy_shader_program();
};

#endif // __mZX_common_GLFrameBufferRenderer_HPP
