/*     _________  ___
 _____ \_   /\  \/  / Qt/GLVideoOutputView.cpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include "GLVideoOutputView.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <Z/classes/mathematics/geometry/euclidean/Rectangle.hpp>
#include "system.h"
#include <QTimer>

#define BOUNDS Rectangle<Real>(0.0, 0.0, Real(this->width()), Real(this->height()))

using namespace ZKit;


GLVideoOutputView::GLVideoOutputView(QWidget *parent) : QGLWidget(parent)
	{
	active = false;
	timer = new QTimer();

	makeCurrent();
	videoOutput = new GLVideoOutput();
	videoOutput->set_geometry(BOUNDS, Z_SCALING_FIT);
	doneCurrent();
	}


GLVideoOutputView::~GLVideoOutputView()
	{
	stop();
	delete videoOutput;
	}


void GLVideoOutputView::initializeGL()
	{
	//QGLFormat format(QGL::SampleBuffers);
	//format.setSwapInterval(1); // vsync
	//setFormat(format);
	}


void GLVideoOutputView::paintGL()
	{videoOutput->draw(FALSE);}


void GLVideoOutputView::resizeGL(int width, int height)
	{videoOutput->set_geometry(Rectangle<Real>(0.0, 0.0, width, height), Z_SCALING_SAME);}


int GLVideoOutputView::heightForWidth(int width) const
	{return width;}


Value2D<Real> GLVideoOutputView::contentSize()
	{return videoOutput->content_bounds.size;}


void GLVideoOutputView::setContentSize(Value2D<Real> contentSize)
	{
	makeCurrent();
	videoOutput->set_content_size(contentSize);
	doneCurrent();
	}


void GLVideoOutputView::setScaling(ZKey(SCALING) scaling)
	{
	makeCurrent();
	videoOutput->set_geometry(BOUNDS, scaling);
	doneCurrent();
	}


void GLVideoOutputView::setResolutionAndFormat(Value2D<Size> resolution, UInt format)
	{
	Q_UNUSED(format);
	makeCurrent();
	videoOutput->set_resolution(resolution);
	doneCurrent();
	}


void GLVideoOutputView::start()
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


void GLVideoOutputView::stop()
	{
	if (active)
		{
		timer->stop();
		delete timer;
		active = FALSE;
		}
	}


void GLVideoOutputView::blank()
	{
	if (!active)
		{
		memset(videoOutput->buffer.buffers[0], 0, videoOutput->input_height * videoOutput->input_width * 4 * 3);
		updateGL();
		}
	}


void GLVideoOutputView::setLinearInterpolation(bool enabled)
	{
	makeCurrent();
	videoOutput->set_linear_interpolation(enabled);
	doneCurrent();
	}


// Qt/GLVideoOutputView.cpp EOF
