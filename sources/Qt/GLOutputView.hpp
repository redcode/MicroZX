/*     _________  ___
 _____ \_   /\  \/  / Qt/GLOutputView.hpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef __NEStalin_Qt_GLOutputView_HPP
#define __NEStalin_Qt_GLOutputView_HPP

#include <QGLWidget>
#include <QEvent>
#include <pthread.h>

namespace C {
#	include <GLOutput.h>
}

class GLOutputView : public QGLWidget {Q_OBJECT
	private:

	static GLOutputView**  activeViews;
	static size_t	       activeViewCount;
	static pthread_mutex_t mutex;
	static pthread_t       drawingThread;
	static volatile bool   mustStop;

	QGLContext* GLContext;
	C::GLOutput GLOutput;

	struct {bool reshaped	       :1;
		bool active	       :1;
		bool OpenGLReady       :1;
		bool startWhenPossible :1;
	} flags;

	protected:
	void paintEvent(QPaintEvent *);

	public:
	explicit GLOutputView(QWidget *parent = 0);
	~GLOutputView();
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);
	int heightForWidth(int width) const;
	static void drawActiveViews();
	C::QTripleBuffer *getBuffer() {return &GLOutput.buffer;}
	C::Q2D contentSize();
	void setContentSize(C::Q2D content_size);
	void setScaling(QKey(SCALING) scaling);
	void setResolutionAndFormat(C::Q2DSize resolution, C::quint format);
	void start();
	void stop();
	void setLinearInterpolation(bool enabled);
};

#endif // __NEStalin_Qt_GLOutputView_HPP
