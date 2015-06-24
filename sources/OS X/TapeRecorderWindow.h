/* mZX - TapeRecorderWindow.h
	      __	   __
  _______ ___/ /______ ___/ /__
 / __/ -_) _  / __/ _ \ _  / -_)
/_/  \__/\_,_/\__/\___/_,_/\__/
Copyright © 2013 RedCode Software. All rights reserved. */

#import <Cocoa/Cocoa.h>

@interface TapeRecorderWindow : NSWindow

@end


@protocol TapeRecorderWindowDelegate <NSObject>

	- (void) tapeRecorderWindow:	      (TapeRecorderWindow *) tapeRecorderWindow
		 didReceiveDragForFileAtPath: (NSString *)	     path;

@end

