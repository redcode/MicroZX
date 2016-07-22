/*	_________  ___
  _____ \_   /\  \/  /	iOS/MainViewController.m
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import "MainViewController.h"
#import "MachineViewController.h"


@implementation MainViewController


	- (void) viewDidLoad
		{
		[super viewDidLoad];
		// Do any additional setup after loading the view, typically from a nib.
		MachineViewController *controller = [[MachineViewController alloc] init];

		[_machineViewControllers addObject: controller];

		[self.view addSubview: controller.view];
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
