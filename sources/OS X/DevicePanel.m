/* ACME - DevicePanel.m
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2014 Manuel Sainz de Baranda y Goñi <manuel@redcodesoftware.com>
Released under the terms of the GNU General Public License v3. */

#import "DevicePanel.h"


@implementation DevicePanel


	- (id) initWithContentRect: (NSRect) contentRect
		{
		if ((self = [super
			initWithContentRect: contentRect
			styleMask:	     NSBorderlessWindowMask
			backing:	     NSBackingStoreBuffered
			defer:		     YES]
		))
			{
			self.hasShadow = YES;
			}

		return self;
		}


	- (BOOL) isMovableByWindowBackground
		{return YES;}


@end
