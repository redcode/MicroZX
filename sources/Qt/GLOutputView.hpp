/*     _________  ___
 _____ \_   /\  \/  / Qt/GLOutputView.hpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef __mZX_Qt_GLOutputView_HPP
#define __mZX_Qt_GLOutputView_HPP

#include <QGLWidget>
#include <QTimer>
#include "GLOutput.h"

class GLOutputView : public QGLWidget {Q_OBJECT
	private:
	static size_t	       activeViewCount;
	static volatile bool   mustStop;

	QTimer*	    timer;
	QGLContext* GLContext;
	GLOutput    output;

	struct {bool reshaped	       :1;
		bool active	       :1;
		bool OpenGLReady       :1;
		bool startWhenPossible :1;
	} flags;

	public:
	explicit GLOutputView(QWidget *parent = 0);
	~GLOutputView();
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);
	int heightForWidth(int width) const;
	static void drawActiveViews();
	ZTripleBuffer *getBuffer() {return &output.buffer;}
	Z2D contentSize();
	void setContentSize(Z2D content_size);
	void setScaling(ZKey(SCALING) scaling);
	void setResolutionAndFormat(Z2DSize resolution, zuint format);
	void start();
	void stop();
	void setLinearInterpolation(bool enabled);
};

#endif // __mZX_Qt_GLOutputView_HPP
