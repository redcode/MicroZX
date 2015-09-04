/*     _________  ___
 _____ \_   /\  \/  / Qt/GLOutputView.cpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include "GLOutputView.hpp"
#include <stdlib.h>
#include <GL/glx.h>
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
	//buffer.buffers[0]	= NULL;
	flags.active		= false;
	flags.reshaped		= false;
	flags.OpenGLReady	= false;
	flags.startWhenPossible = false;
	//setAttribute(Qt::WA_PaintOutsidePaintEvent, true);

	GLContext = (QGLContext *)context();

	timer = new QTimer();

	makeCurrent();
	gl_output_initialize(&output);
	gl_output_set_geometry(&output, BOUNDS, Z_SCALING_FIT);
	doneCurrent();
	qDebug("GLOutputView::constructor");
	}


GLOutputView::~GLOutputView()
	{
	stop();
	gl_output_finalize(&output);
	free(output.buffer.buffers[0]);
	qDebug("~GLOutputView()");
	}


void GLOutputView::initializeGL()
	{
	qDebug("GLOutputView::initializeGL");
	//QGLFormat format(QGL::SampleBuffers);
	//format.setSwapInterval(1); // vsync
	//setFormat(format);

	flags.OpenGLReady = TRUE;
	if (flags.startWhenPossible) this->start();
	}


void GLOutputView::paintGL()
	{
	//qDebug("paintGL()");
	if (flags.reshaped)
		gl_output_set_geometry(&output, BOUNDS, Z_SCALING_SAME);

	gl_output_draw(&output, FALSE);
	flags.reshaped = FALSE;
	}


void GLOutputView::resizeGL(int w, int h)
	{
	Q_UNUSED(w);
	Q_UNUSED(h);
	flags.reshaped = TRUE;
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
	qDebug("GLOutputView::setResolutionAndFormat");
	gl_output_set_resolution(&output, resolution);
	doneCurrent();
	}


void GLOutputView::start()
	{
	qDebug("start");
	if (flags.OpenGLReady)
		{
		qDebug("OpenGL is ready");
		flags.active = TRUE;
		timer = new QTimer();
		timer->setInterval(1000 / 60);
		connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
		timer->start();
		}

	else flags.startWhenPossible = TRUE;
	}


void GLOutputView::stop()
	{
	if (flags.active)
		{
		timer->stop();
		delete timer;
		flags.active = FALSE;
		qDebug("stop()");
		}

	flags.startWhenPossible = FALSE;
	}


void GLOutputView::blank()
	{
	if (!flags.active)
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
