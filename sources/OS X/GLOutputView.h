/*	_________  ___
  _____ \_   /\  \/  /	OS X/GLOutputView.h
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>
#import "GLOutput.h"

@interface GLOutputView : NSView {

	NSOpenGLContext*	_GLContext;
	NSOpenGLPixelFormat*	_pixelFormat;
	QTripleBuffer		_buffer;
	GLOutput		_GLOutput;

	struct {BOOL OpenGLReady       :1;
		BOOL startWhenPossible :1;
		BOOL reshaped	       :1;
		BOOL active	       :1;
	} _flags;
}
	@property (nonatomic, readonly) GLOutput*      GLOutput;
	@property (nonatomic, readonly) QTripleBuffer* buffer;
	@property (nonatomic, assign)	Q2D	       contentSize;
	@property (nonatomic, assign)	QKey(SCALING)  scaling;

	- (void) setResolution:	(Q2DSize) resolution
		 format:	(quint	) format;

	- (void) start;

	- (void) stop;

	- (void) setLinearInterpolation: (BOOL) value;
@end
