/*	_________  ___
  _____ \_   /\  \/  /	iOS/MainViewController.m
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import "MainViewController.h"
#import "MachineViewController.h"
#import "MachineABI.h"


@implementation MainViewController


	- (void) newMachineWithABI: (MachineABI *) ABI
		{
		MachineViewController *controller = [[MachineViewController alloc] initWithMachineABI: ABI];

		[_machineViewControllers addObject: controller];

		UIView *view = controller.view;

		view.frame = CGRectMake(0.0, 0.0, 352.0, 296);

		[self.view addSubview: view];
		[controller release];
		}



	- (void) viewDidLoad
		{
		[super viewDidLoad];
		[self newMachineWithABI: &machine_abi_table[4]];
		}


	- (void) didReceiveMemoryWarning
		{
		[super didReceiveMemoryWarning];
		// Dispose of any resources that can be recreated.
		}


#	pragma mark - IBAction


	- (IBAction) showTapeRecorder: (id) sender
		{
		}


	- (IBAction) showKeyboard: (id) sender
		{
		}


	- (IBAction) showJoystick: (id) sender
		{
		}


@end
