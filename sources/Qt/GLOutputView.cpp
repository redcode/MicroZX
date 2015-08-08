/*	_________  ___
  _____ \_   /\  \/  /	Qt/GLOutputView.cpp
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#include "GLOutputView.hpp"
#include <stdlib.h>
#include <GL/glx.h>
#include <stdio.h>

namespace Q {
#	include <Q/functions/geometry/constructors.h>
#	include <Q/functions/base/Q2DValue.h>
#	include <Q/functions/buffering/QTripleBuffer.h>
#	include <Q/hardware/machine/model/computer/ZX Spectrum/ZX Spectrum.h>
#	include "system.h"
}

#define BOUNDS Q::q_rectangle(0.0, 0.0, (Q::qreal)this->width(), (Q::qreal)this->height())

GLOutputView**	GLOutputView::activeViews     = NULL;
size_t		GLOutputView::activeViewCount = 0;
pthread_t	GLOutputView::drawingThread;
pthread_mutex_t	GLOutputView::mutex	      = PTHREAD_MUTEX_INITIALIZER;
volatile bool	GLOutputView::mustStop	      = FALSE;



static void *drawing_thread_entry(void *)
	{
	GLOutputView::drawActiveViews();
	return NULL;
	}


void GLOutputView::drawActiveViews()
	{
	Q::quint64 frames_per_second = 60;
	Q::quint64 frame_ticks	     = 1000000000 / frames_per_second;
	Q::quint64 next_frame_tick   = Q::q_ticks();
	Q::quint64 delta;

	while (!mustStop)
		{
		pthread_mutex_lock(&mutex);

		for (Q::qsize index = activeViewCount; index;)
			{
			GLOutputView *view = activeViews[--index];
			view->makeCurrent();
			Q::gl_output_draw(&view->GLOutput, FALSE);
			view->doneCurrent();
			view->update();
			}

		pthread_mutex_unlock(&mutex);

		if ((delta = (next_frame_tick += frame_ticks) - Q::q_ticks()) <= frame_ticks)
			Q::q_wait(delta);
		}
	}


GLOutputView::GLOutputView(QWidget *parent) : QGLWidget(parent)
	{
	//buffer.buffers[0]	= NULL;
	flags.active		= false;
	flags.reshaped		= false;
	flags.OpenGLReady	= false;
	flags.startWhenPossible = false;
	//setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
	qDebug("GLVideoOutputView::constructor");
	}


GLOutputView::~GLOutputView()
	{
	stop();
	pthread_mutex_lock(&mutex);
	gl_output_finalize(&GLOutput);
	//[_pixelFormat release];
	//[_GLContext release];
	free(GLOutput.buffer.buffers[0]);
	pthread_mutex_unlock(&mutex);
	//[super dealloc];
	qDebug("~GLOutputView()");
	}


void GLOutputView::initializeGL()
	{
	qDebug("GLOutputView::initializeGL");
//	QGLFormat format(QGL::SampleBuffers);
//	format.setSwapInterval(1); // vsync
//	setFormat(format);
	Q::gl_output_initialize(&GLOutput);
	Q::gl_output_set_geometry(&GLOutput, BOUNDS, Q_SCALING_FIT);
	GLContext = (QGLContext *)context();
	flags.OpenGLReady = TRUE;
	if (flags.startWhenPossible) this->start();
	}


void GLOutputView::paintEvent(QPaintEvent *event)
	{
	if (flags.active) pthread_mutex_lock(&mutex);
	QGLWidget::paintEvent(event);
	if (flags.active) pthread_mutex_unlock(&mutex);
	}


void GLOutputView::paintGL()
	{
	if (flags.reshaped)
		Q::gl_output_set_geometry(&GLOutput, BOUNDS, Q_SCALING_SAME);

	Q::gl_output_draw(&GLOutput, FALSE);
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


Q::Q2D GLOutputView::contentSize()
	{
	return GLOutput.content_bounds.size;
	}


void GLOutputView::setContentSize(Q::Q2D contentSize)
	{
	if (flags.active) pthread_mutex_lock(&mutex);
	Q::gl_output_set_content_size(&GLOutput, contentSize);
	if (flags.active) pthread_mutex_unlock(&mutex);
	}


void GLOutputView::setScaling(QKey(SCALING) scaling)
	{
	if (flags.active) pthread_mutex_lock(&mutex);
	Q::gl_output_set_geometry(&GLOutput, BOUNDS, scaling);
	if (flags.active) pthread_mutex_unlock(&mutex);
	}


void GLOutputView::setResolutionAndFormat(Q::Q2DSize resolution, Q::quint format)
	{
	Q_UNUSED(format);
	makeCurrent();
	qDebug("GLVideoOutputView::setResolutionAndFormat");
	Q::gl_output_set_resolution(&GLOutput, resolution);
	doneCurrent();
	}


void GLOutputView::start()
	{
	qDebug("start");
	if (flags.OpenGLReady)
		{
		qDebug("OpenGL is ready");
		flags.active = TRUE;

		if (activeViewCount)
			{
			pthread_mutex_lock(&mutex);
			activeViews = (GLOutputView **)realloc(activeViews, sizeof(void *) * (activeViewCount + 1));
			activeViews[activeViewCount++] = this;
			pthread_mutex_unlock(&mutex);
			}

		else	{
			qDebug("crear hilo");
			*(activeViews = (GLOutputView **)malloc(sizeof(void *))) = this;
			activeViewCount = 1;
			mustStop = FALSE;
			pthread_attr_t thread_attributes;
			pthread_attr_init(&thread_attributes);
			pthread_create(&drawingThread, &thread_attributes, (void *(*)(void *))drawing_thread_entry, NULL);
			pthread_attr_destroy(&thread_attributes);
			}
		}

	else flags.startWhenPossible = TRUE;
	}


void GLOutputView::stop()
	{
	if (flags.active)
		{
		pthread_mutex_lock(&mutex);

		if (activeViewCount > 1)
			{
			size_t index = activeViewCount--;

				while (activeViews[--index] != this);
				memmove(activeViews + index, activeViews + index + 1, sizeof(void *) * (activeViewCount - index));
				activeViews = (GLOutputView **)realloc(activeViews, sizeof(void *) * activeViewCount);
				}

			else	{
				mustStop = TRUE;
				pthread_join(drawingThread, NULL);
				activeViewCount = 0;
				free(activeViews);
				activeViews = NULL;
				}

			flags.active = FALSE;
			pthread_mutex_unlock(&mutex);
			}

		flags.startWhenPossible = FALSE;
	}


void GLOutputView::setLinearInterpolation(bool enabled)
	{
	if (flags.active) pthread_mutex_lock(&mutex);
	Q::gl_output_set_linear_interpolation(&GLOutput, enabled);
	if (flags.active) pthread_mutex_unlock(&mutex);
	}


// Qt/GLOutputView.cpp EOF
