// CocoPlus - NSMenu+CocoPlus.h
//   ___	       , __
//  / (_)	      /|/  \ |\
// |	  __   __  __  |___/ | |	__
// |	 / (\_/   / (\_|     |/  |  |  / _\_
//  \___/\__/ \__/\__/ | ___/|__/|_/|_/  \/
// Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
// Released under the terms of the GNU Lesser General Public License v3.

#import <Cocoa/Cocoa.h>

@interface NSMenu (CocoPlus)

	- (void) addSeparatorItem;

	- (NSMenuItem *) addItemWithTitle: (NSString *) title
			 action:	   (SEL	      ) action
			 keyEquivalent:	   (NSString *) keyEquivalent
			 modifierMask:	   (NSUInteger) modifierMask;

	- (NSMenuItem *) addItemWithTitle: (NSString *) title
			 action:	   (SEL	      ) action
			 keyEquivalent:	   (NSString *) keyEquivalent
			 modifierMask:	   (NSUInteger) modifierMask
			 state:		   (NSInteger ) state;

	- (NSMenuItem *) addItemWithSubmenu: (NSMenu *) submenu;

@end
