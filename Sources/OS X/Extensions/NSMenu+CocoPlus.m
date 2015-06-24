// CocoPlus - NSMenu+CocoPlus.m
//   ___	       , __
//  / (_)	      /|/  \ |\
// |	  __   __  __  |___/ | |	__
// |	 / (\_/   / (\_|     |/  |  |  / _\_
//  \___/\__/ \__/\__/ | ___/|__/|_/|_/  \/
// Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
// Released under the terms of the GNU Lesser General Public License v3.

#import "NSMenu+CocoPlus.h"
#import "NSMenuItem+CocoPlus.h"


@implementation NSMenu (CocoPlus)


	- (void) addSeparatorItem
		{[self addItem: [NSMenuItem separatorItem]];}


	- (NSMenuItem *) addItemWithTitle: (NSString *) title
			 action:	   (SEL	      ) action
			 keyEquivalent:	   (NSString *) keyEquivalent
			 modifierMask:	   (NSUInteger) modifierMask
		{
		NSMenuItem *item = [[NSMenuItem alloc]
			initWithTitle: title
			action:	       action
			keyEquivalent: keyEquivalent];

		item.keyEquivalentModifierMask = modifierMask;
		[self addItem: item];
		[item release];
		return item;
		}


	- (NSMenuItem *) addItemWithTitle: (NSString *) title
			 action:	   (SEL	      ) action
			 keyEquivalent:	   (NSString *) keyEquivalent
			 modifierMask:	   (NSUInteger) modifierMask
			 state:		   (NSInteger ) state
		{
		NSMenuItem *item = [[NSMenuItem alloc]
			initWithTitle: title
			action:	       action
			keyEquivalent: keyEquivalent];

		item.keyEquivalentModifierMask = modifierMask;
		item.state = state;
		[self addItem: item];
		[item release];
		return item;
		}



	- (NSMenuItem *) addItemWithSubmenu: (NSMenu *) submenu
		{
		NSMenuItem *item = [NSMenuItem itemWithSubmenu: submenu];

		[self addItem: item];
		return item;
		}


@end
