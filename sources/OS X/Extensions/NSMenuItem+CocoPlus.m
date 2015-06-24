// CocoPlus - NSMenuItem+CocoPlus.m
//   ___	       , __
//  / (_)	      /|/  \ |\
// |	  __   __  __  |___/ | |	__
// |	 / (\_/   / (\_|     |/  |  |  / _\_
//  \___/\__/ \__/\__/ | ___/|__/|_/|_/  \/
// Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
// Released under the terms of the GNU Lesser General Public License v3.

#import "NSMenuItem+CocoPlus.h"


@implementation NSMenuItem (CocoPlus)


	+ (NSMenuItem *) itemWithSubmenu: (NSMenu *) submenu
		{
		NSMenuItem *item = [[NSMenuItem alloc] init];

		item.title = submenu.title;
		item.submenu = submenu;
		return [item autorelease];
		}


@end
