/*	_________  ___
  _____ \_   /\  \/  /	OS X/MainController.h
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import "PreferencesWindowController.h"

@interface MainController : NSObject <NSApplicationDelegate> {
	IBOutlet NSMenu* machinesMenu;

	PreferencesWindowController* _preferencesWindowController;
	NSMutableArray*		     _machineWindowControllers;
}
	- (IBAction) preferences:	(id) sender;
	- (IBAction) newDefaultMachine: (id) sender;
	- (IBAction) newMachine:	(id) sender;
@end
