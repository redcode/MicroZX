/*	_________  ___
  _____ \_   /\  \/  /	iOS/MachineViewController.mm
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import "MachineViewController.h"
#import <Z/hardware/machine/platform/computer/ZX Spectrum.h>
#import <Z/hardware/machine/model/computer/ZX Spectrum/ZX Spectrum.h>

#define SCREEN_SIZE_X	Z_JOIN_2(Z_ZX_SPECTRUM_SCREEN_WIDTH,  .0)
#define SCREEN_SIZE_Y	Z_JOIN_2(Z_ZX_SPECTRUM_SCREEN_HEIGHT, .0)

using namespace Zeta;

@implementation MachineViewController


	- (id) initWithMachineABI: (MachineABI *) machineABI
		{
		if ((self = [super initWithNibName: @"MachineView" bundle: nil]))
			{
			_smooth = YES;

			/*----------------------------.
			| Create video output object. |
			'----------------------------*/
			_videoOutputView = [[GLVideoView alloc] initWithFrame:
				CGRectMake(0.0, 0.0, SCREEN_SIZE_X, SCREEN_SIZE_Y)];

			[_videoOutputView
				setResolution: Value2D<Zeta::Size>(SCREEN_SIZE_X, SCREEN_SIZE_Y)
				format:	       0];

			//_videoOutputView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

			/*----------------------------.
			| Create audio output object. |
			'----------------------------*/
			_audioOutputPlayer = new CoreAudioOutputPlayer();
			//_audioOutput = [[ALOutputPlayer alloc] init];


			_keyboardBuffer = new TripleBuffer(malloc(Z_UINT64_SIZE * 3), Z_UINT64_SIZE);
			_keyboard = (Z64Bit *)_keyboardBuffer->production_buffer();
			memset(_keyboardBuffer->buffers[0], 0xFF, Z_UINT64_SIZE * 3);

			_machine = new Machine(machineABI, _videoOutputView.buffer, _audioOutputPlayer->buffer(), _keyboardBuffer);

			/*-----------------.
			| Load needed ROMs |
			'-----------------*/
			Zeta::Size index = machineABI->rom_count;
			NSBundle *bundle = [NSBundle mainBundle];
			ROM *rom;

			while (index)
				{
				rom = &machineABI->roms[--index];

				NSData *ROM = [NSData dataWithContentsOfFile: [bundle
					pathForResource: [NSString stringWithUTF8String: rom->file_name]
					ofType:		 @"rom"
					inDirectory:	 @"ROMs"]];

				_machine->write_memory(rom->base_address, (void *)[ROM bytes], rom->size);
				}

			_keyboardState.value_uint64 = Type<Zeta::UInt64>::maximum;
			_attachInputBuffer = NO;
			}

		return self;
		}


	- (void) viewDidLoad
		{
		}


@end
