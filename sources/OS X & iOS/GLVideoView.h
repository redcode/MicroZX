/*     _________  ___
 _____ \_   /\  \/  / OS X/GLVideoOutputView.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#import "GLFrameBufferRenderer.hpp"

#if Z_OS == Z_OS_MAC_OS_X
#	import <Cocoa/Cocoa.h>
#elif Z_OS == Z_OS_IOS
#	import <UIKit/UIKit.h>
#endif

@interface GLVideoView :

#	if Z_OS == Z_OS_MAC_OS_X

		NSView {NSOpenGLContext*     _GLContext;
			CGLContextObj	     _CGLContext;
			NSOpenGLPixelFormat* _pixelFormat;

#	elif Z_OS == Z_OS_IOS

		UIView {CAEAGLLayer* _EAGLLayer;
			EAGLContext* _GLContext;
			GLuint	     _renderBuffer;
#	endif

	GLFrameBufferRenderer* _renderer;

	struct {BOOL reshaped :1;
		BOOL active   :1;
		BOOL blank    :1;
	} _flags;
}
	@property (nonatomic, readonly) Zeta::TripleBuffer*	  buffer;
	@property (nonatomic, assign)	Zeta::Value2D<Zeta::Real> contentSize;
	@property (nonatomic, assign)	ZKey(SCALING)		  scaling;

	- (void) setResolution:	(Zeta::Value2D<Zeta::Size>) resolution
		 format:	(Zeta::UInt		  ) format;

	- (void) start;

	- (void) stop;

	- (void) blank;

	- (void) setLinearInterpolation: (BOOL) value;
@end
