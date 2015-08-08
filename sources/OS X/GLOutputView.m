/*	_________  ___
  _____ \_   /\  \/  /	OS X/GLOutputView.m
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import "GLOutputView.h"
#import "geometry.h"
#import <Q/types/data.h>
#import <Q/functions/geometry/QRectangle.h>
#import <Q/functions/buffering/QTripleBuffer.h>
#import <Q/macros/casting.h>
#import <pthread.h>
#import <stdlib.h>

#define SET_CONTEXT	CGLSetCurrentContext(_CGLContext)
#define RESTORE_CONTEXT CGLSetCurrentContext(NULL)


GLOutputEffect effect;

GLint texture_size_uniform;

void draw_effect(void *context, GLsizei texture_width, GLsizei texture_height)
	{
	glUniform2f(texture_size_uniform, (GLfloat)texture_width, (GLfloat)texture_height);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (void *)0);
	}


@implementation GLOutputView


	static GLOutputView**	activeOutputs_     = NULL;
	static qsize		activeOutputCount_ = 0;
	static pthread_mutex_t	mutex_		   = PTHREAD_MUTEX_INITIALIZER;
	static CVDisplayLinkRef	displayLink_;


	static CVReturn draw(
		CVDisplayLinkRef   displayLink,
		const CVTimeStamp* now,
		const CVTimeStamp* outputTime,
		CVOptionFlags	   flagsIn,
		CVOptionFlags*	   flagsOut,
		void*		   dummy
	)
		{
		pthread_mutex_lock(&mutex_);
		qsize index = activeOutputCount_;

		while (index)
			{
			GLOutputView *view = activeOutputs_[--index];
			CGLSetCurrentContext(view->_CGLContext);
			gl_output_draw(&view->_GLOutput, TRUE);
			}

		CGLSetCurrentContext(NULL);
		pthread_mutex_unlock(&mutex_);
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
			gl_output_initialize(&_GLOutput);
			gl_output_set_geometry(&_GLOutput, Q_CAST(NSRect, QRectangle, self.bounds), Q_SCALING_EXPAND);
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
		pthread_mutex_lock(&mutex_);
		SET_CONTEXT;
		gl_output_finalize(&_GLOutput);
		RESTORE_CONTEXT;
		[_pixelFormat release];
		[_GLContext release];
		pthread_mutex_unlock(&mutex_);
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
		if (_flags.active) pthread_mutex_lock(&mutex_);

		SET_CONTEXT;

		if (_flags.blank)
			{
			glClearColor(1.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();
			}

		else	{
			if (_flags.reshaped)
				{
				[_GLContext update];
				gl_output_set_geometry(&_GLOutput, Q_CAST(NSRect, QRectangle, self.bounds), Q_SCALING_SAME);
				}

			gl_output_draw(&_GLOutput, FALSE);
			_flags.reshaped = NO;
			}

		RESTORE_CONTEXT;

		if (_flags.active) pthread_mutex_unlock(&mutex_);
		}


#	pragma mark - Accessors

	- (GLOutput	 *) GLOutput	{return &_GLOutput;}
	- (QTripleBuffer *) buffer	{return &_GLOutput.buffer;}
	- (Q2D		  ) contentSize {return _GLOutput.content_bounds.size;}
	- (QKey(SCALING)  ) scaling	{return _GLOutput.content_scaling;}


	- (void) setContentSize: (Q2D) contentSize
		{
		if (_flags.active) pthread_mutex_lock(&mutex_);
		SET_CONTEXT;
		gl_output_set_content_size(&_GLOutput, contentSize);
		RESTORE_CONTEXT;
		if (_flags.active) pthread_mutex_unlock(&mutex_);
		}


	- (void) setScaling: (QKey(SCALING)) scaling
		{
		if (_flags.active) pthread_mutex_lock(&mutex_);
		SET_CONTEXT;
		gl_output_set_geometry(&_GLOutput, Q_CAST(NSRect, QRectangle, self.bounds), scaling);
		RESTORE_CONTEXT;
		if (_flags.active) pthread_mutex_unlock(&mutex_);
		}


#	pragma mark - Public


	- (void) setResolution: (Q2DSize) resolution
		 format:	(quint	) format
		{
		SET_CONTEXT;
		gl_output_set_resolution(&_GLOutput, resolution);
		RESTORE_CONTEXT;
		}


	- (void) start
		{
		if (activeOutputCount_)
			{
			pthread_mutex_lock(&mutex_);
			activeOutputs_ = realloc(activeOutputs_, sizeof(void *) * (activeOutputCount_ + 1));
			activeOutputs_[activeOutputCount_++] = self;
			pthread_mutex_unlock(&mutex_);
			}

		else	{
			*(activeOutputs_ = malloc(sizeof(void *))) = self;
			activeOutputCount_ = 1;

			CVDisplayLinkCreateWithActiveCGDisplays(&displayLink_);
			CVDisplayLinkSetOutputCallback(displayLink_, (CVDisplayLinkOutputCallback)draw, NULL);

			CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext
				(displayLink_, [_GLContext CGLContextObj], [_pixelFormat CGLPixelFormatObj]);

			CVDisplayLinkStart(displayLink_);
			}

		_flags.active = YES;
		}


	- (void) stop
		{
		if (_flags.active)
			{
			pthread_mutex_lock(&mutex_);

			if (activeOutputCount_ > 1)
				{
				qsize index = activeOutputCount_--;

				while (activeOutputs_[--index] != self);
				memmove(activeOutputs_ + index, activeOutputs_ + index + 1, sizeof(void *) * (activeOutputCount_ - index));
				activeOutputs_ = realloc(activeOutputs_, sizeof(void *) * activeOutputCount_);
				}

			else	{
				if (CVDisplayLinkIsRunning(displayLink_))
					{
					CVDisplayLinkStop(displayLink_);
					while (CVDisplayLinkIsRunning(displayLink_));
					}

				CVDisplayLinkRelease(displayLink_);
				activeOutputCount_ = 0;
				free(activeOutputs_);
				activeOutputs_ = NULL;
				}

			_flags.active = NO;
			pthread_mutex_unlock(&mutex_);
			}
		}



	- (void) setLinearInterpolation: (BOOL) value
		{
		if (_flags.active) pthread_mutex_lock(&mutex_);
		SET_CONTEXT;
		gl_output_set_linear_interpolation(&_GLOutput, value);
		RESTORE_CONTEXT;
		if (_flags.active) pthread_mutex_unlock(&mutex_);
		}

@end
