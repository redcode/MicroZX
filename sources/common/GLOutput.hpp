/*     _________  ___
 _____ \_   /\  \/  / common/GLOutput.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef __mZX_common_GLOutput_HPP
#define __mZX_common_GLOutput_HPP

#include "OpenGL.h"
#include <Z/classes/geometry/Rectangle.hpp>
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

class GLOutput {
	public:
	GLsizei			    input_width;
	GLsizei			    input_height;
	ZTripleBuffer		    buffer;
	ZKit::Rectangle<ZKit::Real> viewport;
	ZKit::Rectangle<ZKit::Real> content_bounds;
	ZKey(SCALING)		    content_scaling;
	GLfloat			    model_view_matrix[16];
	GLuint			    texture;
	GLOutputEffect*		    effect;
	void*			    effect_context;

	GLOutput();
	~GLOutput();

	void set_resolution(ZKit::Value2D<ZKit::Size> resolution);
	void set_content_bounds(ZKit::Rectangle<ZKit::Real> bounds);
	void set_content_size(ZKit::Value2D<ZKit::Real> size);
	void set_geometry(ZKit::Rectangle<ZKit::Real> viewport, ZKey(SCALING) content_scaling);
	void set_effect(GLOutputEffect *effect, void *effect_context);
	void remove_effect();
	void draw(ZKit::Boolean skip_old);
	void set_linear_interpolation(ZKit::Boolean value);
};

#endif /* __mZX_common_GLOutput_HPP */
