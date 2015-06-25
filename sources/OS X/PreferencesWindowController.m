/*	_________  ___
  _____ \_   /\  \/  /	OS X/PreferencesWindowController.m
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import "PreferencesWindowController.h"


@implementation PreferencesWindowController


	- (id) init
		{
		if ((self = [super initWithWindowNibName: @"PreferencesWindow"]))
			{
			// Initialization code here.
			}

		return self;
		}


	- (void) windowDidLoad
		{
		[super windowDidLoad];
		// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		}


@end
