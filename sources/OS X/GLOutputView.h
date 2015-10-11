/*     _________  ___
 _____ \_   /\  \/  / OS X/GLOutputView.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>
#import "GLOutput.hpp"
#import <Z/classes/base/Value2D.hpp>

@interface GLOutputView : NSView {

	NSOpenGLContext*     _GLContext;
	CGLContextObj	     _CGLContext;
	NSOpenGLPixelFormat* _pixelFormat;
	GLOutput*	     _output;

	struct {BOOL reshaped :1;
		BOOL active   :1;
		BOOL blank    :1;
	} _flags;
}
	@property (nonatomic, readonly) GLOutput*		  GLOutput;
	@property (nonatomic, readonly) ZTripleBuffer*		  buffer;
	@property (nonatomic, assign)	ZKit::Value2D<ZKit::Real> contentSize;
	@property (nonatomic, assign)	ZKey(SCALING)  scaling;

	- (void) setResolution:	(ZKit::Value2D<ZKit::Size>) resolution
		 format:	(ZKit::UInt		  ) format;

	- (void) start;

	- (void) stop;

	- (void) blank;

	- (void) setLinearInterpolation: (BOOL) value;

@end
