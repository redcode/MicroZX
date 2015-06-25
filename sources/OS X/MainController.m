/*	_________  ___
  _____ \_   /\  \/  /	OS X/MainController.m
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import "MainController.h"
#import "MachineWindowController.h"
#import "MachineABI.h"


@implementation MainController


#	pragma mark - Helpers


	- (void) newMachineWithABI: (MachineABI *) ABI
		{
		MachineWindowController *controller = [[MachineWindowController alloc] initWithMachineABI: ABI];

		[_machineWindowControllers addObject: controller];
		[controller release];

		NSWindow *window = controller.window;

		[[NSNotificationCenter defaultCenter]
			addObserver: self
			selector:    @selector(windowWillClose:)
			name:	     NSWindowWillCloseNotification
			object:	     window];

		[window makeKeyAndOrderFront: self];
		}


#	pragma mark - NSApplicationDelegate


	- (void) applicationDidFinishLaunching: (NSNotification *) aNotification
		{
		_machineWindowControllers = [[NSMutableArray alloc] init];

		NSUInteger i = 0;

		for (; i < machine_abi_count; i++) [machinesMenu
			addItemWithTitle: [NSString stringWithUTF8String: machine_abi_table[i].model_name]
			action:		  @selector(newMachine:)
			keyEquivalent:	  @""];

		[self newDefaultMachine: nil];
		}


	- (void) applicationWillTerminate: (NSNotification *) notification
		{
		NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];

		[notificationCenter
			removeObserver: self
			name:		NSWindowDidBecomeMainNotification
			object:		nil];

		[_machineWindowControllers release];
		}


#	pragma mark - Callbacks


	- (void) windowWillClose: (NSNotification *) notification
		{
		NSWindow *window = notification.object;
		MachineWindowController *controller = window.windowController;

		[[NSNotificationCenter defaultCenter]
			removeObserver: self
			name:		NSWindowWillCloseNotification
			object:		window];

		if ([controller isKindOfClass: MachineWindowController.class])
			[_machineWindowControllers removeObject: controller];

		else	{
			[_preferencesWindowController release];
			_preferencesWindowController = nil;
			}
		}


#	pragma mark - IBAction


	- (IBAction) preferences: (id) sender
		{
		if (!_preferencesWindowController)
			{
			NSWindow *window = (_preferencesWindowController = [[PreferencesWindowController alloc] init]).window;

			[[NSNotificationCenter defaultCenter]
				addObserver: self
				selector:    @selector(windowWillClose:)
				name:	     NSWindowWillCloseNotification
				object:	     window];

			[window makeKeyAndOrderFront: self];
			}

		else [_preferencesWindowController.window makeKeyAndOrderFront: self];
		}


	- (IBAction) openDocument: (id) sender
		{
		/*NSOpenPanel *panel = [NSOpenPanel openPanel];

		panel.allowedFileTypes = [NSArray arrayWithObjects: @"z80", @"sna", nil];

		if ([panel runModal] == NSFileHandlingPanelOKButton)
			{
			NSString *path = panel.URL.path;
			NSError *error;
			}*/
		}


	- (IBAction) newDefaultMachine: (NSMenuItem *) sender
		{[self newMachineWithABI: &machine_abi_table[4]];}


	- (IBAction) newMachine: (NSMenuItem *) sender
		{[self newMachineWithABI: &machine_abi_table[[machinesMenu.itemArray indexOfObject: sender]]];}


@end
