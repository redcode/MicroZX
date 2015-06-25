/*	_________  ___
  _____ \_   /\  \/  /	μZX - OS X/MainController.h
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import <Cocoa/Cocoa.h>

@interface MainController : NSObject <NSApplicationDelegate> {
	IBOutlet NSMenu* machinesMenu;

	NSMutableArray*  _machineControllers;
}

	- (IBAction) newDefaultMachine: (id) sender;
	- (IBAction) newMachine:	(id) sender;
@end
