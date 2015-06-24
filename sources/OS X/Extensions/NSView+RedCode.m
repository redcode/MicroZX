/* NSView - RedCode Software Extensions
	      __	   __
  _______ ___/ /______ ___/ /__
 / __/ -_) _  / __/ _ \ _  / -_)
/_/  \__/\_,_/\__/\___/_,_/\__/
Copyright © 2013 RedCode Software. All rights reserved. */

#import "NSView+RedCode.h"


@implementation NSView (RedCode)


	- (NSRect) frameInScreen
		{
		NSRect frame = [self convertRect: self.bounds toView: nil];

		frame.origin = [self.window convertBaseToScreen: frame.origin];
		return frame;
		}


	- (NSImage *) imageCapture
		{
		NSImage *image;
		NSRect bounds = self.bounds;
		NSBitmapImageRep *bitmap = [self bitmapImageRepForCachingDisplayInRect: bounds];

		bitmap.size = bounds.size;
		[self cacheDisplayInRect: bounds toBitmapImageRep: bitmap];
		image = [[NSImage alloc] initWithSize: bounds.size];
		[image addRepresentation: bitmap];
		return [image autorelease];
		}


@end
