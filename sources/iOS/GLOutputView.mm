/*	_________  ___
  _____ \_   /\  \/  /	iOS/GLOutputView.m
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import "GLOutputView.h"


@implementation GLOutputView


#	pragma mark - Overwritten


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



	- (void) dealloc
		{
		[_GLContext release];
		_GLContext = nil;
		[super dealloc];
		}



@end
