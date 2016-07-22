/*     _________  ___
 _____ \_   /\  \/  / OS X/GLVideoView.mm
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#import "GLVideoView.h"
#import <QuartzCore/QuartzCore.h>
#import <mutex>
#import <vector>

#if Z_OS == Z_OS_MAC_OS_X

#	define SET_CONTEXT	CGLSetCurrentContext(_CGLContext)
#	define RESTORE_CONTEXT	CGLSetCurrentContext(nullptr)
#	define IF_ACTIVE_LOCK	if (_flags.active) mutex.lock()
#	define IF_ACTIVE_UNLOCK if (_flags.active) mutex.unlock()
#	define LOCK		mutex.lock()
#	define UNLOCK		mutex.unlock()
#	define EQUAL_SIZES	NSEqualSizes

#elif Z_OS == Z_OS_IOS

#	define SET_CONTEXT	[EAGLContext setCurrentContext: _GLContext]
#	define RESTORE_CONTEXT  [EAGLContext setCurrentContext: nil]
#	define IF_ACTIVE_LOCK
#	define IF_ACTIVE_UNLOCK
#	define LOCK
#	define UNLOCK
#	define EQUAL_SIZES	 CGSizeEqualToSize
#	define NSRect		 CGRect

#endif

using namespace Zeta;


@implementation GLVideoView

	static std::vector<GLVideoView *> activeInstances = {};
	static std::mutex		  mutex;


#	if Z_OS == Z_OS_MAC_OS_X

		static CVDisplayLinkRef displayLink;

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
			zsize index = activeInstances.size();

			while (index)
				{
				GLVideoView *view = activeInstances[--index];
				CGLSetCurrentContext(view->_CGLContext);
				view->_renderer->draw(true);
				}

			RESTORE_CONTEXT;
			mutex.unlock();
			return kCVReturnSuccess;
			}


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
				_renderer = new GLFrameBufferRenderer();
				_renderer->set_geometry(self.bounds, Z_SCALING_EXPAND);

				NSBundle *bundle = [NSBundle mainBundle];
				std::string *error;

				NSString *sourceCode = [NSString
					stringWithContentsOfFile: [bundle pathForResource: @"Simple" ofType: @"vsh"]
					encoding:		  NSUTF8StringEncoding
					error:			  nullptr];

				if (!_renderer->set_vertex_shader((Character *)sourceCode.UTF8String, &error))
					{
					[[NSAlert alertWithError: [NSError errorWithDomain: @"OpenGL" code: 0 userInfo:
						@{NSLocalizedDescriptionKey:		 @"Can not compile OpenGL vertext shader",
						  NSLocalizedRecoverySuggestionErrorKey: [NSString stringWithUTF8String: error->c_str()]}]]
							runModal];

					delete error;
					}

				sourceCode = [NSString
					stringWithContentsOfFile: [bundle pathForResource: @"Simple" ofType: @"fsh"]
					encoding:		  NSUTF8StringEncoding
					error:			  nullptr];

				if (!_renderer->set_fragment_shader((Character *)sourceCode.UTF8String, &error))
					{
					[[NSAlert alertWithError: [NSError errorWithDomain: @"OpenGL" code: 0 userInfo:
						@{NSLocalizedDescriptionKey:		 @"Can not compile OpenGL fragment shader",
						  NSLocalizedRecoverySuggestionErrorKey: [NSString stringWithUTF8String: error->c_str()]}]]
							runModal];

					delete error;
					}

				_renderer->create_shader_program();
				RESTORE_CONTEXT;
				}

			return self;
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


#	elif Z_OS == Z_OS_IOS


		static CADisplayLink* displayLink = nil;


		+ (void) draw: (CADisplayLink *) sender
			{
			zsize index = activeInstances.size();

			while (index)
				{
				GLVideoView *view = activeInstances[--index];
				[EAGLContext setCurrentContext: view->_GLContext];
				view->_renderer->draw(true);
				}

			RESTORE_CONTEXT;
			}


		+ (Class) layerClass {return [CAEAGLLayer class];}


		- (void) render
			{
			glClearColor(0, 104.0/255.0, 55.0/255.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);
			[_GLContext presentRenderbuffer: GL_RENDERBUFFER];
			}


		- (id) initWithCoder: (NSCoder *) coder
			{
			if ((self = [super initWithCoder: coder]))
				{
				(_EAGLLayer = (CAEAGLLayer *) self.layer).opaque = YES;

				if (!(_GLContext = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES2]))
					{
					NSLog(@"Failed to initialize OpenGLES 2.0 context");
					exit(1);
					}

				if (![EAGLContext setCurrentContext: _GLContext])
					{
					NSLog(@"Failed to set current OpenGL context");
					exit(1);
					}

				glGenRenderbuffers(1, &_renderBuffer);
				glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
				[_GLContext renderbufferStorage: GL_RENDERBUFFER fromDrawable: _EAGLLayer];

				GLuint framebuffer;
				glGenFramebuffers(1, &framebuffer);
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderBuffer);

				[self render];
				}

			//NSLog(@"init GL");

			return self;
			}

#	endif


	- (void) dealloc
		{
		[self stop];
		LOCK;
		SET_CONTEXT;
		delete _renderer;
		RESTORE_CONTEXT;

#		if Z_OS == Z_OS_MAC_OS_X
			[_pixelFormat release];
#		endif

		[_GLContext release];
		UNLOCK;
		[super dealloc];
		}



	- (void) setFrame: (NSRect) frame
		{
		if (!EQUAL_SIZES(frame.size, self.bounds.size)) _flags.reshaped = YES;
		super.frame = frame;
		}


	- (void) drawRect: (NSRect) frame
		{
		IF_ACTIVE_LOCK; SET_CONTEXT;

		/*if (_flags.blank)
			{
			glClearColor(1.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();
			}*/

		//else	{
			if (_flags.reshaped)
				{
#				if Z_OS == Z_OS_MAC_OS_X
					[_GLContext update];
#				endif

				_renderer->set_geometry(self.bounds, Z_SCALING_SAME);
				}

			_renderer->draw(false);
			_flags.reshaped = NO;
			//}

		RESTORE_CONTEXT; IF_ACTIVE_UNLOCK;
		}


#	pragma mark - Accessors

	- (TripleBuffer *) buffer      {return &_renderer->buffer;}
	- (Value2D<Real> ) contentSize {return _renderer->content_bounds.size;}
	- (ZKey(SCALING) ) scaling     {return _renderer->content_scaling;}


	- (void) setContentSize: (Value2D<Real>) contentSize
		{
		IF_ACTIVE_LOCK; SET_CONTEXT;
		_renderer->set_content_size(contentSize);
		RESTORE_CONTEXT; IF_ACTIVE_UNLOCK;
		}


	- (void) setScaling: (ZKey(SCALING)) scaling
		{
		IF_ACTIVE_LOCK; SET_CONTEXT;
		_renderer->set_geometry(self.bounds, scaling);
		RESTORE_CONTEXT; IF_ACTIVE_UNLOCK;
		}


#	pragma mark - Public


	- (void) setResolution: (Value2D<Zeta::Size>) resolution
		 format:	(UInt		    ) format
		{
		SET_CONTEXT;
		_renderer->set_resolution(resolution);
		RESTORE_CONTEXT;
		}


	- (void) start
		{
		if (activeInstances.size())
			{
			LOCK;
			activeInstances.push_back(self);
			UNLOCK;
			}

		else	{
			activeInstances.push_back(self);

#			if Z_OS == Z_OS_MAC_OS_X
				CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
				CVDisplayLinkSetOutputCallback(displayLink, (CVDisplayLinkOutputCallback)draw, NULL);

				CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext
					(displayLink, [_GLContext CGLContextObj], [_pixelFormat CGLPixelFormatObj]);

				CVDisplayLinkStart(displayLink);

#			elif Z_OS == Z_OS_IOS
				[displayLink = [CADisplayLink displayLinkWithTarget: self.class selector: @selector(draw:)]
					addToRunLoop: NSRunLoop.currentRunLoop
					forMode:      NSDefaultRunLoopMode];
#			endif
			}

		_flags.active = YES;
		_flags.blank  = NO;
		}


	- (void) stop
		{
		if (_flags.active)
			{
			LOCK;

			if (activeInstances.size() == 1)
				{
#				if Z_OS == Z_OS_MAC_OS_X

					if (CVDisplayLinkIsRunning(displayLink))
						{
						CVDisplayLinkStop(displayLink);
						while (CVDisplayLinkIsRunning(displayLink));
						}

					CVDisplayLinkRelease(displayLink);

#				elif Z_OS == Z_OS_IOS

					if (displayLink)
						{
						[displayLink removeFromRunLoop: NSRunLoop.currentRunLoop forMode: NSDefaultRunLoopMode];
						[displayLink invalidate];
						displayLink = nil;
						}
#				endif
				}

			for (auto index = activeInstances.size(); index;) if (activeInstances[--index] == self)
				{
				activeInstances.erase(activeInstances.begin() + index);
				break;
				}

			_flags.active = NO;
			UNLOCK;
			}
		}


	- (void) blank
		{
		if (!_flags.active)
			{
			memset(_renderer->buffer.buffers[0], 0, _renderer->input_height * _renderer->input_width * 4 * 3);

			#if Z_OS == Z_OS_MAC_OS_X
				self.needsDisplay = YES;
			#elif Z_OS == Z_OS_IOS
				[self setNeedsDisplay];
			#endif
			}
		}


	- (void) setLinearInterpolation: (BOOL) value
		{
		IF_ACTIVE_LOCK;
		SET_CONTEXT;
		_renderer->set_linear_interpolation(value);
		RESTORE_CONTEXT;
		IF_ACTIVE_UNLOCK;
		}

@end
