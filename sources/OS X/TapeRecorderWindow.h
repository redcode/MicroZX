/*     _________  ___
 _____ \_   /\  \/  / OS X/TapeRecorderWindow.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#import <Cocoa/Cocoa.h>

@interface TapeRecorderWindow : NSWindow

@end


@protocol TapeRecorderWindowDelegate <NSObject>

	- (void) tapeRecorderWindow:	      (TapeRecorderWindow *) tapeRecorderWindow
		 didReceiveDragForFileAtPath: (NSString *)	     path;

@end

