/* NSWindow - RedCode Software Extensions
	      __	   __
  _______ ___/ /______ ___/ /__
 / __/ -_) _  / __/ _ \ _  / -_)
/_/  \__/\_,_/\__/\___/_,_/\__/
Copyright © 2013 RedCode Software. All rights reserved. */

#import <AppKit/AppKit.h>

@interface NSWindow (RedCode)

	@property (nonatomic, readonly) NSSize	borderSize;
	@property (nonatomic, readonly) CGFloat toolbarHeight;

	- (void) replaceContentViewWithView: (NSView *) view
		 animate:		     (BOOL)	animate;

	- (void) animateIntoScreenFrame: (NSRect) screenFrame
		 fromTopCenterToSize:	 (NSSize) size;

@end
