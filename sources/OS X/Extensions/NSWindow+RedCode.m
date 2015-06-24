/* NSWindow - RedCode Software Extensions
	      __	   __
  _______ ___/ /______ ___/ /__
 / __/ -_) _  / __/ _ \ _  / -_)
/_/  \__/\_,_/\__/\___/_,_/\__/
Copyright © 2013 RedCode Software. All rights reserved. */

#import "NSWindow+RedCode.h"
#import "geometry.h"


@implementation NSWindow (RedCode)


	- (NSSize) borderSize
		{
		return QToNSSize(q_2d_subtract
			(NSSizeToQ(self.frame.size),
			 NSSizeToQ(((NSView *)self.contentView).bounds.size)));
		}


	- (CGFloat) toolbarHeight
		{
		NSToolbar *toolbar = self.toolbar;
		CGFloat toolbarHeight = 0.0;
		NSRect windowFrame;

		if (toolbar && [toolbar isVisible])
			{
			windowFrame = [NSWindow contentRectForFrameRect: self.frame styleMask: self.styleMask];
			toolbarHeight = NSHeight(windowFrame) - NSHeight(((NSView *)self.contentView).frame);
			}

		return toolbarHeight;
		}


	- (void) replaceContentViewWithView: (NSView *) view
		 animate:		     (BOOL)	animate
		{
		if (view != self.contentView)
			{
			NSRect frame = self.frame;;
			NSRect contentFrame = ((NSView *)self.contentView).frame;
			NSRect viewFrame = view.frame;

			viewFrame.origin.y = 0.0;
			view.frame = viewFrame;
			[self setContentView: view];

			frame.origin.y = frame.origin.y + contentFrame.size.height - viewFrame.size.height;
			frame.size = viewFrame.size;

			[self setContentView: view];

			[self	setFrame: [self frameRectForContentRect: frame]
				display:  YES
				animate:  self.isVisible ? animate : NO
			];
			}
		}


	- (void) animateIntoScreenFrame: (NSRect) screenFrame
		 fromTopCenterToSize:	 (NSSize) size
		{
		NSRect oldFrame = self.frame;

		NSRect newFrame = NSMakeRect
			(floor(oldFrame.origin.x + oldFrame.size.width / 2.0 - size.width / 2.0),
			 floor(oldFrame.origin.y + oldFrame.size.height      - size.height),
			 size.width, size.height);

		if (!NSContainsRect(screenFrame, newFrame))
			{
			if (newFrame.origin.x < screenFrame.origin.x)
				newFrame.origin.x = screenFrame.origin.x;

			else if (NSMaxX(newFrame) > NSMaxX(screenFrame))
				newFrame.origin.x = screenFrame.origin.x + screenFrame.size.width - size.width;

			if (newFrame.origin.y < screenFrame.origin.y)
				newFrame.origin.y = screenFrame.origin.y;

			else if (NSMaxY(newFrame) > NSMaxY(screenFrame))
				newFrame.origin.y = screenFrame.origin.y + screenFrame.size.height - size.height;
			}

		BOOL visible = self.isVisible;
		[self setFrame: newFrame display: visible animate: visible];
		}


@end
