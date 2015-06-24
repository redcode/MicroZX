/*	_________  ___
  _____ \_   /\  \/  /	μZX - OS X/MainController.h
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import <Cocoa/Cocoa.h>

@interface MainController : NSObject <NSApplicationDelegate> {

	IBOutlet NSMenu*	machinesMenu;
	IBOutlet NSMenuItem*	machineVolumeMenuItem;
	IBOutlet NSView*	machineVolumeView;

	NSMutableArray*		_machineControllers;
	NSArray*		_currentWindowMenus;
}

	- (IBAction) newDefaultMachine: (NSMenuItem *) sender;
	- (IBAction) newMachine: (NSMenuItem *) sender;

@end
