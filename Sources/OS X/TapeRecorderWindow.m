/* mZX - TapeRecorderWindow.m
	      __	   __
  _______ ___/ /______ ___/ /__
 / __/ -_) _  / __/ _ \ _  / -_)
/_/  \__/\_,_/\__/\___/_,_/\__/
Copyright © 2013 RedCode Software. All rights reserved. */

#import "TapeRecorderWindow.h"

@implementation TapeRecorderWindow


	- (NSDragOperation) draggingEntered: (id <NSDraggingInfo>) sender
		{
		return [[[sender draggingPasteboard] types] containsObject: NSFilenamesPboardType]
			? NSDragOperationGeneric
			: NSDragOperationNone;
		}


	- (BOOL) performDragOperation: (id <NSDraggingInfo>) sender
		{
		NSPasteboard *pboard;
		NSDragOperation sourceDragMask;

		sourceDragMask = [sender draggingSourceOperationMask];
		pboard = [sender draggingPasteboard];

		if ([[pboard types] containsObject: NSFilenamesPboardType])
			{
			NSArray *files = [pboard propertyListForType: NSFilenamesPboardType];

			[(id <TapeRecorderWindowDelegate>)self.delegate
				tapeRecorderWindow:	     self
				didReceiveDragForFileAtPath: [files objectAtIndex: 0]
			];
			}

		return YES;
		}


@end
