/*     _________  ___
 _____ \_   /\  \/  / Qt/GLOutputView.cpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include "GLOutputView.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <Z/functions/geometry/constructors.h>
#include <Z/functions/base/Z2DValue.h>
#include <Z/functions/buffering/ZTripleBuffer.h>
#include <Z/hardware/machine/model/computer/ZX Spectrum/ZX Spectrum.h>
#include "system.h"
#include <QTimer>

#define BOUNDS z_rectangle(0.0, 0.0, (zreal)this->width(), (zreal)this->height())


GLOutputView::GLOutputView(QWidget *parent) : QGLWidget(parent)
	{
	active = false;
	timer = new QTimer();

	makeCurrent();
	gl_output_initialize(&output);
	gl_output_set_geometry(&output, BOUNDS, Z_SCALING_FIT);
	doneCurrent();
	}


GLOutputView::~GLOutputView()
	{
	stop();
	gl_output_finalize(&output);
	}


void GLOutputView::initializeGL()
	{
	//QGLFormat format(QGL::SampleBuffers);
	//format.setSwapInterval(1); // vsync
	//setFormat(format);
	}


void GLOutputView::paintGL()
	{
	gl_output_draw(&output, FALSE);
	}


void GLOutputView::resizeGL(int width, int height)
	{
	gl_output_set_geometry(&output, z_rectangle(0.0, 0.0, width, height), Z_SCALING_SAME);
	}


int GLOutputView::heightForWidth(int width) const
	{
	return width;
	}


Z2D GLOutputView::contentSize()
	{
	return output.content_bounds.size;
	}


void GLOutputView::setContentSize(Z2D contentSize)
	{
	makeCurrent();
	gl_output_set_content_size(&output, contentSize);
	doneCurrent();
	}


void GLOutputView::setScaling(ZKey(SCALING) scaling)
	{
	makeCurrent();
	gl_output_set_geometry(&output, BOUNDS, scaling);
	doneCurrent();
	}


void GLOutputView::setResolutionAndFormat(Z2DSize resolution, zuint format)
	{
	Q_UNUSED(format);
	makeCurrent();
	gl_output_set_resolution(&output, resolution);
	doneCurrent();
	}


void GLOutputView::start()
	{
	if (!active)
		{
		active = TRUE;
		timer = new QTimer();
		timer->setInterval(1000 / 60);
		connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
		timer->start();
		}
	}


void GLOutputView::stop()
	{
	if (active)
		{
		timer->stop();
		delete timer;
		active = FALSE;
		}
	}


void GLOutputView::blank()
	{
	if (!active)
		{
		memset(output.buffer.buffers[0], 0, output.input_height * output.input_width * 4 * 3);
		updateGL();
		}
	}


void GLOutputView::setLinearInterpolation(bool enabled)
	{
	makeCurrent();
	gl_output_set_linear_interpolation(&output, enabled);
	doneCurrent();
	}


// Qt/GLOutputView.cpp EOF
