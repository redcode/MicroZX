/*	_________  ___
  _____ \_   /\  \/  /	μZX - OS X/MainController.h
 |  |  |_/  /__>    <	Copyright © 2013 RedCode Software.
 |   ____________/\__\	All rights reserved.
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
