/*     _________  ___
 _____ \_   /\  \/  / OS X/GLVideoOutputView.mm
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#define Z_USE_INTEROPERABILITY_WITH_CG_GEOMETRY
#define Z_USE_INTEROPERABILITY_WITH_NS_GEOMETRY

#import "GLVideoOutputView.h"
#import <Z/functions/buffering/ZTripleBuffer.h>
#import <Z/functions/casting.hpp>
#import <stdlib.h>
#import <QuartzCore/QuartzCore.h>
#include <Z/functions/base/Z2DValue.h>
#include <mutex>

#define SET_CONTEXT	CGLSetCurrentContext(_CGLContext)
#define RESTORE_CONTEXT CGLSetCurrentContext(NULL)

using namespace ZKit;


GLVideoOutputEffect effect;

GLint texture_size_uniform;

void draw_effect(void *context, GLsizei texture_width, GLsizei texture_height)
	{
	glUniform2f(texture_size_uniform, (GLfloat)texture_width, (GLfloat)texture_height);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (void *)0);
	}


using namespace ZKit;


@implementation GLVideoOutputView


	static GLVideoOutputView** activeInstances     = NULL;
	static zsize		   activeInstanceCount = 0;
	static std::mutex	   mutex;
	static CVDisplayLinkRef	   displayLink;


	static CVReturn draw(
		CVDisplayLinkRef   displayLink,
		const CVTimeStamp* now,
		const CVTimeStamp* outputTime,
		CVOptionFlags	   flagsIn,
		CVOptionFlags*	   flagsOut,
		void*		   dummy
	)
		{
		mutex.lock();
		zsize index = activeInstanceCount;

		while (index)
			{
			GLVideoOutputView *view = activeInstances[--index];
			CGLSetCurrentContext(view->_CGLContext);
			view->_videoOutput->draw(TRUE);
			}

		CGLSetCurrentContext(NULL);
		mutex.unlock();
		return kCVReturnSuccess;
		}


#	pragma mark - Overwritten


	- (id) initWithFrame: (NSRect) frame
		{
		if ((self = [super initWithFrame: frame]))
			{
			NSOpenGLPixelFormatAttribute attrs[] = {
				//NSOpenGLPFAWindow,
				//NSOpenGLPFADoubleBuffer,
				//NSOpenGLProfileVersion3_2Core,
				//NSOpenGLPFADepthSize, 32,
				// Must specify the 3.2 Core Profile to use OpenGL 3.2
				0
			};

			_pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes: attrs];
			_GLContext   = [[NSOpenGLContext alloc] initWithFormat: _pixelFormat shareContext: nil];

			GLint swapInterval = 1;

			[_GLContext setValues: &swapInterval forParameter: NSOpenGLCPSwapInterval];
			_GLContext.view = self;
			_CGLContext = _GLContext.CGLContextObj;

			SET_CONTEXT;
			_videoOutput = new GLVideoOutput();
			_videoOutput->set_geometry(self.bounds, Z_SCALING_EXPAND);
			RESTORE_CONTEXT;
			/*gl_output_set_effect(&_GLOutput, &effect, NULL);

			OPEN_GL_CONTEXT_SET_CURRENT(_GLOutput.context);
			texture_size_uniform = glGetUniformLocation(effect.program, "textureSize");
			OPEN_GL_CONTEXT_RESTORE;*/


			/*NSBundle *bundle = [NSBundle mainBundle];

			NSData *data = [NSData dataWithContentsOfFile: [bundle pathForResource: @"SABR" ofType: @"vsh"]];

			effect.owner_count = 0;

			effect.vertex_shader_source_size = (GLint)[data length];
			effect.vertex_shader_source = malloc(effect.vertex_shader_source_size);
			memcpy(effect.vertex_shader_source, [data bytes], effect.vertex_shader_source_size);

			data = [NSData dataWithContentsOfFile: [bundle pathForResource: @"SABR" ofType: @"fsh"]];

			effect.fragment_shader_source_size = (GLint)[data length];
			effect.fragment_shader_source = malloc(effect.fragment_shader_source_size);
			memcpy(effect.fragment_shader_source, [data bytes], effect.fragment_shader_source_size);

			effect.abi.draw = draw_effect;*/
			}

		return self;
		}


	- (void) dealloc
		{
		[self stop];
		mutex.lock();
		SET_CONTEXT;
		delete _videoOutput;
		RESTORE_CONTEXT;
		[_pixelFormat release];
		[_GLContext release];
		mutex.unlock();
		[super dealloc];
		}


	- (void) viewDidMoveToWindow
		{
		[super viewDidMoveToWindow];
		if (![self window]) [_GLContext clearDrawable];
		}


	- (void) lockFocus
		{
		[super lockFocus];
		if ([_GLContext view] != self) [_GLContext setView: self];
		[_GLContext makeCurrentContext];
		}


	- (void) setFrame: (NSRect) frame
		{
		if (!NSEqualSizes(frame.size, self.bounds.size)) _flags.reshaped = YES;
		super.frame = frame;
		}


	- (void) drawRect: (NSRect) frame
		{
		if (_flags.active) mutex.lock();

		SET_CONTEXT;

		/*if (_flags.blank)
			{
			glClearColor(1.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();
			}*/

		//else	{
			if (_flags.reshaped)
				{
				[_GLContext update];
				_videoOutput->set_geometry(self.bounds, Z_SCALING_SAME);
				}

			_videoOutput->draw(false);
			_flags.reshaped = NO;
			//}

		RESTORE_CONTEXT;

		if (_flags.active) mutex.unlock();
		}


#	pragma mark - Accessors

	- (GLVideoOutput *) videoOutput	{return _videoOutput;}
	- (Value2D<Real>  ) contentSize {return _videoOutput->content_bounds.size;}
	- (ZKey(SCALING)  ) scaling	{return _videoOutput->content_scaling;}


	- (void) setContentSize: (Value2D<Real>) contentSize
		{
		if (_flags.active) mutex.lock();
		SET_CONTEXT;
		_videoOutput->set_content_size(contentSize);
		RESTORE_CONTEXT;
		if (_flags.active) mutex.unlock();
		}


	- (void) setScaling: (ZKey(SCALING)) scaling
		{
		if (_flags.active) mutex.lock();
		SET_CONTEXT;
		_videoOutput->set_geometry(self.bounds, scaling);
		RESTORE_CONTEXT;
		if (_flags.active) mutex.unlock();
		}


#	pragma mark - Public


	- (void) setResolution: (Value2D<ZKit::Size>) resolution
		 format:	(UInt		    ) format
		{
		SET_CONTEXT;
		_videoOutput->set_resolution(resolution);
		RESTORE_CONTEXT;
		}


	- (void) start
		{
		if (activeInstanceCount)
			{
			mutex.lock();
			activeInstances = (GLVideoOutputView **)realloc(activeInstances, sizeof(void *) * (activeInstanceCount + 1));
			activeInstances[activeInstanceCount++] = self;
			mutex.unlock();
			}

		else	{
			*(activeInstances = (GLVideoOutputView **)malloc(sizeof(void *))) = self;
			activeInstanceCount = 1;

			CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
			CVDisplayLinkSetOutputCallback(displayLink, (CVDisplayLinkOutputCallback)draw, NULL);

			CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext
				(displayLink, [_GLContext CGLContextObj], [_pixelFormat CGLPixelFormatObj]);

			CVDisplayLinkStart(displayLink);
			}

		_flags.active = YES;
		_flags.blank  = NO;
		}


	- (void) stop
		{
		if (_flags.active)
			{
			mutex.lock();

			if (activeInstanceCount > 1)
				{
				zsize index = activeInstanceCount--;

				while (activeInstances[--index] != self);
				memmove(activeInstances + index, activeInstances + index + 1, sizeof(void *) * (activeInstanceCount - index));
				activeInstances = (GLVideoOutputView **)realloc(activeInstances, sizeof(void *) * activeInstanceCount);
				}

			else	{
				if (CVDisplayLinkIsRunning(displayLink))
					{
					CVDisplayLinkStop(displayLink);
					while (CVDisplayLinkIsRunning(displayLink));
					}

				CVDisplayLinkRelease(displayLink);
				activeInstanceCount = 0;
				free(activeInstances);
				activeInstances = NULL;
				}

			_flags.active = NO;
			mutex.unlock();
			}
		}


	- (void) blank
		{
		if (!_flags.active)
			{
			memset(_videoOutput->buffer.buffers[0], 0, _videoOutput->input_height * _videoOutput->input_width * 4 * 3);
			self.needsDisplay = YES;
			}
		}


	- (void) setLinearInterpolation: (BOOL) value
		{
		if (_flags.active) mutex.lock();
		SET_CONTEXT;
		_videoOutput->set_linear_interpolation(value);
		RESTORE_CONTEXT;
		if (_flags.active) mutex.unlock();
		}

@end
