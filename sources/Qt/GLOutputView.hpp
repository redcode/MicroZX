/* NEStalin - Qt/GLVideoOutputView.hpp
 __ __  ____  ____   __ 	__
|  \  ||  __|/	__| |  |  ___  |  | __  ____
|     ||  _| \__  \|_  _|/ _ | |  |(__)|    |
|__\__||____||____/ |__| \__,_||__||__||__|_|
Copyright © 2014 Manuel Sainz de Baranda y Goñi
Released under the terms of the GNU General Public License v3. */

#ifndef __NEStalin_Qt_GLVideoOutputView_HPP
#define __NEStalin_Qt_GLVideoOutputView_HPP

#include <QGLWidget>
#include <QEvent>
#include <pthread.h>

namespace Q {
#	include <GLOutput.h>
}

class GLVideoOutputView : public QGLWidget {Q_OBJECT
	private:

	static GLVideoOutputView**	activeViews;
	static size_t		activeViewCount;
	static pthread_mutex_t	mutex;
	static pthread_t	drawingThread;
	static volatile bool	mustStop;

	QGLContext*	 GLContext;
	Q::GLOutput	 GLOutput;
	Q::QTripleBuffer buffer;

	struct {bool reshaped	       :1;
		bool active	       :1;
		bool OpenGLReady       :1;
		bool startWhenPossible :1;
	} flags;

	protected:
	void paintEvent(QPaintEvent *);

	public:
	explicit GLVideoOutputView(QWidget *parent = 0);
	~GLVideoOutputView();
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);
	int heightForWidth(int width) const;
	static void drawActiveViews();
	Q::QTripleBuffer *getBuffer() {return &buffer;}
	Q::Q2D contentSize();
	void setContentSize(Q::Q2D content_size);
	void setScaling(QKey(SCALING) scaling);
	void setResolutionAndFormat(Q::Q2DSize resolution, Q::quint format);
	void start();
	void stop();
	void setLinearInterpolation(bool enabled);
};

#endif // __NEStalin_Qt_GLVideoOutputView_HPP
