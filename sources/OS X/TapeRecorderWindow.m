/*     _________  ___
 _____ \_   /\  \/  / OS X/TapeRecorderWindow.m
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

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
