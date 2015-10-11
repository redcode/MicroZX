/*     _________  ___
 _____ \_   /\  \/  / Qt/GLOutputView.cpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include "GLOutputView.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <Z/classes/geometry/Rectangle.hpp>
#include <Z/functions/buffering/ZTripleBuffer.h>
#include <Z/hardware/machine/model/computer/ZX Spectrum/ZX Spectrum.h>
#include "system.h"
#include <QTimer>

#define BOUNDS Rectangle<Real>(0.0, 0.0, Real(this->width()), Real(this->height()))

using namespace ZKit;


GLOutputView::GLOutputView(QWidget *parent) : QGLWidget(parent)
	{
	active = false;
	timer = new QTimer();

	makeCurrent();
	output = new GLOutput();
	output->set_geometry(BOUNDS, Z_SCALING_FIT);
	doneCurrent();
	}


GLOutputView::~GLOutputView()
	{
	stop();
	delete output;
	}


void GLOutputView::initializeGL()
	{
	//QGLFormat format(QGL::SampleBuffers);
	//format.setSwapInterval(1); // vsync
	//setFormat(format);
	}


void GLOutputView::paintGL()
	{
	output->draw(FALSE);
	}


void GLOutputView::resizeGL(int width, int height)
	{
	output->set_geometry(Rectangle<Real>(0.0, 0.0, width, height), Z_SCALING_SAME);
	}


int GLOutputView::heightForWidth(int width) const
	{
	return width;
	}


Value2D<Real> GLOutputView::contentSize()
	{
	return output->content_bounds.size;
	}


void GLOutputView::setContentSize(Value2D<Real> contentSize)
	{
	makeCurrent();
	output->set_content_size(contentSize);
	doneCurrent();
	}


void GLOutputView::setScaling(ZKey(SCALING) scaling)
	{
	makeCurrent();
	output->set_geometry(BOUNDS, scaling);
	doneCurrent();
	}


void GLOutputView::setResolutionAndFormat(Value2D<Size> resolution, UInt format)
	{
	Q_UNUSED(format);
	makeCurrent();
	output->set_resolution(resolution);
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
		memset(output->buffer.buffers[0], 0, output->input_height * output->input_width * 4 * 3);
		updateGL();
		}
	}


void GLOutputView::setLinearInterpolation(bool enabled)
	{
	makeCurrent();
	output->set_linear_interpolation(enabled);
	doneCurrent();
	}


// Qt/GLOutputView.cpp EOF
