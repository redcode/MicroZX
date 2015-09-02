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

#define BOUNDS z_rectangle(0.0, 0.0, (zreal)this->width(), (zreal)this->height())

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
	zuint64 frames_per_second = 60;
	zuint64 frame_ticks	  = 1000000000 / frames_per_second;
	zuint64 next_frame_tick   = z_ticks();
	zuint64 delta;

	while (!mustStop)
		{
		pthread_mutex_lock(&mutex);

		for (zsize index = activeViewCount; index;)
			{
			GLOutputView *view = activeViews[--index];
			view->makeCurrent();
			gl_output_draw(&view->output, FALSE);
			view->doneCurrent();
			view->update();
			}

		pthread_mutex_unlock(&mutex);

		if ((delta = (next_frame_tick += frame_ticks) - z_ticks()) <= frame_ticks)
			z_wait(delta);
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

	GLContext = (QGLContext *)context();

	makeCurrent();
	gl_output_initialize(&output);
	gl_output_set_geometry(&output, BOUNDS, Z_SCALING_FIT);
	doneCurrent();
	qDebug("GLOutputView::constructor");
	}


GLOutputView::~GLOutputView()
	{
	stop();
	pthread_mutex_lock(&mutex);
	gl_output_finalize(&output);
	//[_pixelFormat release];
	//[_GLContext release];
	free(output.buffer.buffers[0]);
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
		gl_output_set_geometry(&output, BOUNDS, Z_SCALING_SAME);

	gl_output_draw(&output, FALSE);
	flags.reshaped = FALSE;
	//qDebug("paintGL()");
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
	if (flags.active) pthread_mutex_lock(&mutex);
	gl_output_set_content_size(&output, contentSize);
	if (flags.active) pthread_mutex_unlock(&mutex);
	}


void GLOutputView::setScaling(ZKey(SCALING) scaling)
	{
	if (flags.active) pthread_mutex_lock(&mutex);
	gl_output_set_geometry(&output, BOUNDS, scaling);
	if (flags.active) pthread_mutex_unlock(&mutex);
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
			qDebug("stop()");
			}

		flags.startWhenPossible = FALSE;
	}


void GLOutputView::setLinearInterpolation(bool enabled)
	{
	if (flags.active) pthread_mutex_lock(&mutex);
	gl_output_set_linear_interpolation(&output, enabled);
	if (flags.active) pthread_mutex_unlock(&mutex);
	}


// Qt/GLOutputView.cpp EOF
