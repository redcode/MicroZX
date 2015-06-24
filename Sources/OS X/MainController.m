/*	_________  ___
  _____ \_   /\  \/  /	μZX - OS X/MainController.h
 |  |  |_/  /__>    <	Copyright © 2014 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	All rights reserved.
 |_*/

#import "MainController.h"
#import "MachineWindowController.h"
#import "MachineABI.h"

/*static const char* machineNames[] = {
	"ZX Spectrum 16K (Issue 1)",
	"ZX Spectrum 48K (Issue 2)",
	"ZX Spectrum 48K (Issue 3)",
	"ZX Spectrum +",
	"ZX Spectrum + 128K",
	"ZX Spectrum + 128K (ES)",
	"ZX Spectrum +2",
	"ZX Spectrum +2 (ES)",
	"ZX Spectrum +2 (FR)",
	"ZX Spectrum +2A",
	"ZX Spectrum +2A (ES)",
	"ZX Spectrum +3",
	"ZX Spectrum +3 (ES)",
	"Inves Spectrum +"
};*/


@implementation MainController


#	pragma mark - Helpers


	- (void) newMachineWithABI: (MachineABI *) ABI
		{
		MachineWindowController *controller = [[MachineWindowController alloc] initWithMachineABI: ABI];

		[_machineControllers addObject: controller];
		[controller release];

		NSWindow *window = controller.window;

		[[NSNotificationCenter defaultCenter]
			addObserver: self
			selector:    @selector(machineWindowWillClose:)
			name:	     NSWindowWillCloseNotification
			object:	     window];

		[window makeKeyAndOrderFront: self];
		}



#	pragma mark - NSApplicationDelegate


	- (void) applicationDidFinishLaunching: (NSNotification *) aNotification
		{
		_machineControllers = [[NSMutableArray alloc] init];

		NSUInteger i = 0;

		for (; i < machine_abi_count; i++) [machinesMenu
			addItemWithTitle: [NSString stringWithUTF8String: machine_abi_table[i].model_name]
			action:		  @selector(newMachine:)
			keyEquivalent:	  @""
		];

		machineVolumeMenuItem.view = machineVolumeView;

		NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];

		[notificationCenter
			addObserver: self
			selector:    @selector(windowDidBecomeKey:)
			name:	     NSWindowDidBecomeMainNotification
			object:	     nil];

		[notificationCenter
			addObserver: self
			selector:    @selector(windowDidResignKey:)
			name:	     NSWindowDidResignMainNotification
			object:	     nil];

		[self newDefaultMachine: nil];
		}


	- (void) applicationWillTerminate: (NSNotification *) notification
		{
		NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];

		[notificationCenter
			removeObserver: self
			name:		NSWindowDidBecomeMainNotification
			object:		nil];

		[notificationCenter
			removeObserver: self
			name:		NSWindowDidResignMainNotification
			object:		nil];

		[_machineControllers release];
		}


#	pragma mark - Callbacks


	- (void) windowDidBecomeKey: (NSNotification *) notification
		{
		id controller = [notification.object windowController];

		if (controller && [controller respondsToSelector: @selector(customMenus)])
			{
			NSMenu *mainMenu = [NSApp mainMenu];
			NSUInteger index = 3;

			_currentWindowMenus = [controller customMenus];
			for (NSMenuItem *item in _currentWindowMenus) [mainMenu insertItem: item atIndex: index++];
			}
		}


	- (void) windowDidResignKey: (NSNotification *) notification
		{
		NSLog(@"windowDidResignKey");
		NSMenu *mainMenu = [NSApp mainMenu];

		for (NSMenuItem *item in _currentWindowMenus) [mainMenu removeItem: item];
		_currentWindowMenus = nil;
		}


	- (void) machineWindowWillClose: (NSNotification *) notification
		{
		NSWindow *window = notification.object;
		MachineWindowController *controller = window.windowController;

		[[NSNotificationCenter defaultCenter]
			removeObserver: self
			name:		NSWindowWillCloseNotification
			object:		window];


		NSLog(@"window closed => %@, controller retain count => %lu", controller.window.title, controller.retainCount);

		[controller stop];
		[_machineControllers removeObject: controller];
		}


#	pragma mark - IBAction


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
