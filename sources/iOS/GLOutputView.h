/*	_________  ___
  _____ \_   /\  \/  /	iOS/GLOutputView.h
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@interface GLOutputView : UIView {
	CAEAGLLayer* _EAGLLayer;
	EAGLContext* _GLContext;
	GLuint	     _renderBuffer;
}

@end
