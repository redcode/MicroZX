/*	_________  ___
  _____ \_   /\  \/  /	μZX - OS X/MachineWindowController.m
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import "MachineWindowController.h"
#import "geometry.h"
#import "KeyCodes.h"
#import <Q/hardware/machine/platform/computer/ZX Spectrum.h>
#import <Q/hardware/machine/model/computer/ZX Spectrum/ZX Spectrum.h>
#import "NSWindow+RedCode.h"
#import "NSView+RedCode.h"
#import <pthread.h>
#import "TapeRecorderController.h"
#import "NSMenu+CocoPlus.h"
#import "NSMenuItem+CocoPlus.h"
#include <stdio.h>

#include "system.h"
#import <Q/functions/buffering/QTripleBuffer.h>
#import <Q/functions/buffering/QRingBuffer.h>
#import <Q/functions/geometry/QRectangle.h>
#import <Q/macros/casting.h>



#define kScreenZoomIncrement	1.5
#define MACHINE_SCREEN_SIZE	q_2d((qreal)Q_ZX_SPECTRUM_SCREEN_WIDTH, (qreal)Q_ZX_SPECTRUM_SCREEN_HEIGHT)
#define NS_MACHINE_SCREEN_SIZE	NSMakeSize(Q_ZX_SPECTRUM_SCREEN_WIDTH, Q_ZX_SPECTRUM_SCREEN_HEIGHT)
#include <Q/macros/color.h>


static void *EmulationMain(MachineWindowController *controller)
	{
	quint64 frames_per_second = 50;
	quint64 frame_ticks	  = 1000000000 / frames_per_second;
	quint64 next_frame_tick   = q_ticks();
	quint64 delta;
	quint	maximum_frameskip = 5;
	quint	loops;
	void*	audio_output_buffer;
	quint64* keyboard;

	while (!controller->_mustStop)
		{
		loops = 0;

		do controller->_machineABI->run_one_frame(controller->_machine);
		while ((next_frame_tick += frame_ticks) < q_ticks() && ++loops < maximum_frameskip);

		if ((audio_output_buffer = q_ring_buffer_try_produce(controller->_audioOutputBuffer)) != NULL)
			controller->_machine->audio_output_buffer = audio_output_buffer;

		controller->_machine->video_output_buffer = q_triple_buffer_produce(controller->_videoOutputBuffer);

		//----------------.
		// Consume input. |
		//----------------'
		if ((keyboard = q_triple_buffer_consume(controller->_keyboardBuffer)) != NULL)
			{controller->_machine->state.keyboard.value_uint64 = *keyboard;}

		if (controller->_audioInputBuffer != NULL)
			{
			controller->_machine->audio_input_buffer = ring_buffer_try_read(controller->_audioInputRing, controller->_audioInputBuffer)
				? controller->_audioInputBuffer : NULL;
			}

		//----------------------------------------.
		// Schedule next iteration time and wait. |
		//----------------------------------------'
		if ((delta = next_frame_tick - q_ticks()) <= frame_ticks)
			q_wait(delta);

		//else printf("delta => %lu, next => %lu\n", delta, next_frame_tick);
		}

	return NULL;
	}


@implementation MachineWindowController


#	pragma mark - Helpers


	- (void) hidePointer: (NSTimer *) timer
		{
		_pointerVisibilityTimer = nil;
		[NSCursor setHiddenUntilMouseMoves: YES];
		}


	- (void) startHidePointerTimer
		{
		[_pointerVisibilityTimer invalidate];

		_pointerVisibilityTimer = [NSTimer
			scheduledTimerWithTimeInterval: 1.0
			target:				self
			selector:			@selector(hidePointer:)
			userInfo:			nil
			repeats:			NO];
		}


	- (void) copyImageCaptureToPasteboard
		{
		NSPasteboard *pasteboard  = [NSPasteboard generalPasteboard];

		[pasteboard declareTypes: [NSMutableArray arrayWithObject: NSTIFFPboardType] owner: nil];
		[pasteboard setData: [[_videoOutput imageCapture] TIFFRepresentation] forType: NSTIFFPboardType];
		}


	- (CGFloat) currentZoom
		{
		return _flags.isFullScreen
			? _videoOutput.contentSize.x / (CGFloat)Q_ZX_SPECTRUM_SCREEN_WIDTH
			: ((NSView *)self.window.contentView).bounds.size.width / (CGFloat)Q_ZX_SPECTRUM_SCREEN_WIDTH;
		}


	- (void) setZoom: (qreal) zoom
		{
		if (_flags.isFullScreen)
			{
			Q2D boundsSize = Q_CAST(_videoOutput.bounds.size, NSSize, Q2D);
			Q2D zoomedSize = q_2d(Q_ZX_SPECTRUM_SCREEN_WIDTH * zoom, Q_ZX_SPECTRUM_SCREEN_HEIGHT * zoom);

			_videoOutput.contentSize = q_2d_contains(boundsSize, zoomedSize)
				? zoomedSize
				: q_2d_fit(MACHINE_SCREEN_SIZE, boundsSize);
			}

		else	{
			NSWindow *window = self.window;
			QRectangle screenFrame = Q_CAST(window.screen.visibleFrame, NSRect, QRectangle);
			Q2D borderSize = Q_CAST(window.borderSize, NSSize, Q2D);
			Q2D newSize = q_2d(borderSize.x + Q_ZX_SPECTRUM_SCREEN_WIDTH * zoom, borderSize.y + Q_ZX_SPECTRUM_SCREEN_HEIGHT * zoom);

			[window animateIntoScreenFrame: Q_CAST(screenFrame, QRectangle, NSRect)
				fromTopCenterToSize:	Q_CAST
					(q_2d_contains(screenFrame.size, newSize)
						? newSize
						: q_2d_add(q_2d_fit(MACHINE_SCREEN_SIZE, q_2d_subtract(screenFrame.size, borderSize)), borderSize),
					 Q2D, NSSize)];
			}
		}


	- (BOOL) saveSnapshotAtPath: (NSString *) path
		 error:		     (NSError **) error
		{
		return YES;
		}


#	pragma mark - Accessors


	@synthesize customMenus;


#	pragma mark - Life cycle


	- (id) retain
		{
		return [super retain];
		}


	- (id) initWithMachineABI: (MachineABI *) machineABI
		{
		if ((self = [super initWithWindowNibName: @"MachineWindow"]))
			{
			_smooth = YES;

			/*--------------------------.
			| Create auxiliary controls |
			'--------------------------*/
			[NSBundle loadNibNamed: @"Title Sheet" owner: self];

			NSMenu *machineMenu = [[NSMenu alloc] initWithTitle: @"Machine"];

			[machineMenu addItemWithTitle: @"Power"		action: @selector(toggleMachinePower:) keyEquivalent: @"e" modifierMask: NSCommandKeyMask | NSAlternateKeyMask];
			[machineMenu addItemWithTitle: @"Reset"		action: @selector(resetMachine:)       keyEquivalent: @"r" modifierMask: NSCommandKeyMask | NSAlternateKeyMask];
			[machineMenu addItemWithTitle: @"Pause"		action: @selector(toggleMachinePause:) keyEquivalent: @"p" modifierMask: NSCommandKeyMask | NSAlternateKeyMask];
			[machineMenu addSeparatorItem];
			[machineMenu addItemWithTitle: @"Volume"	action: @selector(toggleMachinePause:) keyEquivalent: @""];
			[machineMenu addSeparatorItem];
			[machineMenu addItemWithTitle: @"Keyboard"	action: @selector(toggleKeyboard:)     keyEquivalent: @"k" modifierMask: NSCommandKeyMask];
			[machineMenu addItemWithTitle: @"Tape Recorder" action: @selector(toggleTapeRecorder:) keyEquivalent: @"t" modifierMask: NSCommandKeyMask];
			[machineMenu addItemWithTitle: @"Debugger"	action: @selector(toggleTapeRecorder:) keyEquivalent: @"d" modifierMask: NSCommandKeyMask];

			NSMenu *viewMenu = [[NSMenu alloc] initWithTitle: @"View"];

			[viewMenu addItemWithTitle: @"Actual Size" action: @selector(zoomImageToActualSize:) keyEquivalent: @"0" modifierMask: NSCommandKeyMask];
			[viewMenu addItemWithTitle: @"Zoom x2"	   action: @selector(zoomImageX2:)	     keyEquivalent: @"2" modifierMask: NSCommandKeyMask];
			[viewMenu addItemWithTitle: @"Zoom x3"	   action: @selector(zoomImageX3:)	     keyEquivalent: @"3" modifierMask: NSCommandKeyMask];
			[viewMenu addItemWithTitle: @"Zoom to Fit" action: @selector(zoomImageToFit:)	     keyEquivalent: @"9" modifierMask: NSCommandKeyMask];
			[viewMenu addItemWithTitle: @"Zoom In"	   action: @selector(zoomIn:)		     keyEquivalent: @"+" modifierMask: NSCommandKeyMask];
			[viewMenu addItemWithTitle: @"Zoom Out"	   action: @selector(zoomOut:)		     keyEquivalent: @"-" modifierMask: NSCommandKeyMask];
			[viewMenu addSeparatorItem];
			[viewMenu addItemWithTitle: @"Smooth" action: @selector(toggleLinearInterpolation:) keyEquivalent: @"l" modifierMask: NSCommandKeyMask state: NSOnState];

			customMenus = [[NSArray alloc] initWithObjects:
				[NSMenuItem itemWithSubmenu: machineMenu],
				[NSMenuItem itemWithSubmenu: viewMenu],
				nil];

			[machineMenu release];
			[viewMenu release];

			/*--------------------------------------.
			| Create the machine and its components |
			'--------------------------------------*/
			_machine		      = malloc(machineABI->context_size);
			_machine->cpu_abi.run	      = (void *)z80_run;
			_machine->cpu_abi.irq	      = (void *)z80_irq;
			_machine->cpu_abi.reset	      = (void *)z80_reset;
			_machine->cpu_abi.power	      = (void *)z80_power;
			_machine->cpu		      = malloc(sizeof(Z80));
			_machine->cpu_cycles	      = &_machine->cpu->cycles;
			_machine->memory	      = malloc(machineABI->memory_size);

			memset(_machine->memory, 0, machineABI->memory_size);
			_machineABI = machineABI;

			/*-----------------------------------.
			| Create needed Input/Output objects |
			'-----------------------------------*/
			_videoOutput = [[GLOutputView alloc] initWithFrame:
				NSMakeRect(0.0, 0.0, (CGFloat)Q_ZX_SPECTRUM_SCREEN_WIDTH, (CGFloat)Q_ZX_SPECTRUM_SCREEN_HEIGHT)];

			[_videoOutput
				setResolution: q_2d_value(SIZE)(Q_ZX_SPECTRUM_SCREEN_WIDTH, Q_ZX_SPECTRUM_SCREEN_HEIGHT)
				format:	       0];

			_videoOutput.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
			_audioOutput = [[CoreAudioOutput alloc] init];
			//_audioOutput = [[ALOutputPlayer alloc] init];

			_videoOutputBuffer = _videoOutput.buffer;
			_audioOutputBuffer = _audioOutput.buffer;

			_machine->video_output_buffer = q_triple_buffer_production_buffer(_videoOutputBuffer);
			_machine->audio_output_buffer = q_ring_buffer_production_buffer(_audioOutputBuffer);

			_keyboardBuffer = malloc(sizeof(QTripleBuffer));
			q_triple_buffer_initialize(_keyboardBuffer, malloc(sizeof(quint64) * 3), sizeof(quint64));
			_keyboard = q_triple_buffer_production_buffer(_keyboardBuffer);
			memset(_keyboardBuffer->buffers[0], 0xFF, sizeof(quint64) * 3);

			/*-----------------.
			| Load needed ROMs |
			'-----------------*/
			qsize index = machineABI->rom_count;
			NSBundle *bundle = [NSBundle mainBundle];
			ROM *rom;

			while (index)
				{
				rom = &_machineABI->roms[--index];

				NSData *ROM = [NSData dataWithContentsOfFile: [bundle
					pathForResource: [NSString stringWithUTF8String: rom->file_name]
					ofType:		 @"rom"
					inDirectory:	 @"ROMs"]];

				memcpy(_machine->memory + rom->base_address, [ROM bytes], rom->size);
				}

			machineABI->initialize(_machine);

			_keyboardState.value_uint64 = Q_UINT64(0xFFFFFFFFFFFFFFFF);

			/*NSData *data = [NSData dataWithContentsOfFile: @"/Users/manuel/Desktop/Batman.sna"];
			QSNAv48K *sna = (QSNAv48K *)[data bytes];

			sna_v48k_decode(sna, &_machine->state, &_machine->cpu->state, _machine->memory);*/
			//_machine->border_color = Q_RGBA32(FF, 00, 00, 00);*/

			_attachInputBuffer = NO;

			[_audioOutput start];

			pthread_attr_t threadAttributes;
			pthread_attr_init(&threadAttributes);
			pthread_create(&_thread, &threadAttributes, (void *(*)(void *))EmulationMain, self);
			pthread_attr_destroy(&threadAttributes);
			}

		return self;
		}


	- (void) stop
		{
		_mustStop = YES;
		pthread_join(_thread, NULL);
		}


#	pragma mark - Overwritten


	- (void) dealloc
		{
		[_videoOutput stop];
		[self stop];
		[titleWindow release];
		[_tapeRecorderController release];
		[_pointerVisibilityTimer invalidate];

		[_videoOutput release];
		[_audioOutput release];
		[_fullScreenWindow release];
		free(_machine->cpu);
		free(_machine->memory);
		//free(_machine->audio_input_buffer);
		free(_machine);
		free(_keyboardBuffer->buffers[0]);
		free(_keyboardBuffer);
		[super dealloc];
		}


	- (void) windowDidLoad
		{
		[super windowDidLoad];

		NSWindow *window = self.window;
		NSSize contentSize = NS_MACHINE_SCREEN_SIZE;

		if (	[window respondsToSelector: @selector(setAnimationBehavior:) ] &&
			[window respondsToSelector: @selector(setCollectionBehavior:)]
		)
			[window setCollectionBehavior: [window collectionBehavior] | NSWindowCollectionBehaviorFullScreenPrimary];

		_minimumWindowSize = q_2d_add(MACHINE_SCREEN_SIZE, NSSizeToQ(window.borderSize));
		window.title = [NSString stringWithUTF8String: _machineABI->model_name];
		[window.contentView addSubview: _videoOutput];
		[window setContentAspectRatio: contentSize];
		[window setContentMinSize: contentSize];
		[_videoOutput start];

		}


#	pragma mark - Input Control

#	define KEY_DOWN(line, bit) _keyboardState.array_uint8[line] &= ~(1 << bit)
#	define KEY_UP(	line, bit) _keyboardState.array_uint8[line] |=  (1 << bit)

	- (void) keyDown: (NSEvent *) event
		{
		//NSLog(@"keyDown:");

		switch ([event keyCode])
			{
			// Line 0
			case kVK_ANSI_Z: KEY_DOWN(0, 1); break;
			case kVK_ANSI_X: KEY_DOWN(0, 2); break;
			case kVK_ANSI_C: KEY_DOWN(0, 3); break;
			case kVK_ANSI_V: KEY_DOWN(0, 4); break;

			// Line 1
			case kVK_ANSI_A: KEY_DOWN(1, 0); break;
			case kVK_ANSI_S: KEY_DOWN(1, 1); break;
			case kVK_ANSI_D: KEY_DOWN(1, 2); break;
			case kVK_ANSI_F: KEY_DOWN(1, 3); break;
			case kVK_ANSI_G: KEY_DOWN(1, 4); break;

			//Line 2
			case kVK_ANSI_Q: KEY_DOWN(2, 0); break;
			case kVK_ANSI_W: KEY_DOWN(2, 1); break;
			case kVK_ANSI_E: KEY_DOWN(2, 2); break;
			case kVK_ANSI_R: KEY_DOWN(2, 3); break;
			case kVK_ANSI_T: KEY_DOWN(2, 4); break;

			// Line 3
			case kVK_ANSI_1: KEY_DOWN(3, 0); break;
			case kVK_ANSI_2: KEY_DOWN(3, 1); break;
			case kVK_ANSI_3: KEY_DOWN(3, 2); break;
			case kVK_ANSI_4: KEY_DOWN(3, 3); break;
			case kVK_ANSI_5: KEY_DOWN(3, 4); break;

			// Line 4
			case kVK_ANSI_0: KEY_DOWN(4, 0); break;
			case kVK_ANSI_9: KEY_DOWN(4, 1); break;
			case kVK_ANSI_8: KEY_DOWN(4, 2); break;
			case kVK_ANSI_7: KEY_DOWN(4, 3); break;
			case kVK_ANSI_6: KEY_DOWN(4, 4); break;

			// Line 5
			case kVK_ANSI_P: KEY_DOWN(5, 0); break;
			case kVK_ANSI_O: KEY_DOWN(5, 1); break;
			case kVK_ANSI_I: KEY_DOWN(5, 2); break;
			case kVK_ANSI_U: KEY_DOWN(5, 3); break;
			case kVK_ANSI_Y: KEY_DOWN(5, 4); break;

			// Line 6
			case kVK_Return: KEY_DOWN(6, 0); break;
			case kVK_ANSI_L: KEY_DOWN(6, 1); break;
			case kVK_ANSI_K: KEY_DOWN(6, 2); break;
			case kVK_ANSI_J: KEY_DOWN(6, 3); break;
			case kVK_ANSI_H: KEY_DOWN(6, 4); break;

			// Line 7
			case kVK_Space:	 KEY_DOWN(7, 0); break;
			case kVK_ANSI_M: KEY_DOWN(7, 2); break;
			case kVK_ANSI_N: KEY_DOWN(7, 3); break;
			case kVK_ANSI_B: KEY_DOWN(7, 4); break;

			// Composed
			case kVK_Delete:	KEY_DOWN(4, 0); KEY_DOWN(0, 0); break;
			case kVK_ANSI_Comma:	KEY_DOWN(7, 1); KEY_DOWN(7, 3); break;
			case kVK_DownArrow:	KEY_DOWN(0, 0); KEY_DOWN(4, 4); break;
			case kVK_UpArrow:	KEY_DOWN(0, 0); KEY_DOWN(4, 3); break;
			case kVK_LeftArrow:	KEY_DOWN(0, 0); KEY_DOWN(3, 4); break;
			case kVK_RightArrow:	KEY_DOWN(0, 0); KEY_DOWN(4, 2); break;

			default: break;
			}

		_keyboard->value_uint64 = _keyboardState.value_uint64;
		_keyboard = q_triple_buffer_produce(_keyboardBuffer);
		}


	- (void) keyUp: (NSEvent *) event
		{
		//NSLog(@"keyUp:");

		switch ([event keyCode])
			{
			// Line 0
			case kVK_ANSI_Z: KEY_UP(0, 1); break;
			case kVK_ANSI_X: KEY_UP(0, 2); break;
			case kVK_ANSI_C: KEY_UP(0, 3); break;
			case kVK_ANSI_V: KEY_UP(0, 4); break;
			// Line 1
			case kVK_ANSI_A: KEY_UP(1, 0); break;
			case kVK_ANSI_S: KEY_UP(1, 1); break;
			case kVK_ANSI_D: KEY_UP(1, 2); break;
			case kVK_ANSI_F: KEY_UP(1, 3); break;
			case kVK_ANSI_G: KEY_UP(1, 4); break;
			//Line 2
			case kVK_ANSI_Q: KEY_UP(2, 0); break;
			case kVK_ANSI_W: KEY_UP(2, 1); break;
			case kVK_ANSI_E: KEY_UP(2, 2); break;
			case kVK_ANSI_R: KEY_UP(2, 3); break;
			case kVK_ANSI_T: KEY_UP(2, 4); break;
			// Line 3
			case kVK_ANSI_1: KEY_UP(3, 0); break;
			case kVK_ANSI_2: KEY_UP(3, 1); break;
			case kVK_ANSI_3: KEY_UP(3, 2); break;
			case kVK_ANSI_4: KEY_UP(3, 3); break;
			case kVK_ANSI_5: KEY_UP(3, 4); break;
			// Line 4
			case kVK_ANSI_0: KEY_UP(4, 0); break;
			case kVK_ANSI_9: KEY_UP(4, 1); break;
			case kVK_ANSI_8: KEY_UP(4, 2); break;
			case kVK_ANSI_7: KEY_UP(4, 3); break;
			case kVK_ANSI_6: KEY_UP(4, 4); break;
			// Line 5
			case kVK_ANSI_P: KEY_UP(5, 0); break;
			case kVK_ANSI_O: KEY_UP(5, 1); break;
			case kVK_ANSI_I: KEY_UP(5, 2); break;
			case kVK_ANSI_U: KEY_UP(5, 3); break;
			case kVK_ANSI_Y: KEY_UP(5, 4); break;
			// Line 6
			case kVK_Return: KEY_UP(6, 0); break;
			case kVK_ANSI_L: KEY_UP(6, 1); break;
			case kVK_ANSI_K: KEY_UP(6, 2); break;
			case kVK_ANSI_J: KEY_UP(6, 3); break;
			case kVK_ANSI_H: KEY_UP(6, 4); break;
			// Line 7
			case kVK_Space:	 KEY_UP(7, 0); break;
			case kVK_ANSI_M: KEY_UP(7, 2); break;
			case kVK_ANSI_N: KEY_UP(7, 3); break;
			case kVK_ANSI_B: KEY_UP(7, 4); break;
			// Composed
			case kVK_Delete:	KEY_UP(4, 0); KEY_UP(0, 0); break;
			case kVK_ANSI_Comma:	KEY_UP(7, 1); KEY_UP(7, 3); break;
			case kVK_DownArrow:	KEY_UP(0, 0); KEY_UP(4, 4); break;
			case kVK_UpArrow:	KEY_UP(0, 0); KEY_UP(4, 3); break;
			case kVK_LeftArrow:	KEY_UP(0, 0); KEY_UP(3, 4); break;
			case kVK_RightArrow:	KEY_UP(0, 0); KEY_UP(4, 2); break;

			default: break;
			}

		_keyboard->value_uint64 = _keyboardState.value_uint64;
		_keyboard = q_triple_buffer_produce(_keyboardBuffer);
		}


	- (void) flagsChanged: (NSEvent *) event
		{
		_keyboardInput = YES;
		NSUInteger flags = [event modifierFlags];

		switch ([event keyCode])
			{
			case kVK_Shift:	 if (flags & NSShiftKeyMask)	 KEY_DOWN(0, 0); else KEY_UP(0, 0); break;
			case kVK_Option: if (flags & NSAlternateKeyMask) KEY_DOWN(7, 1); else KEY_UP(7, 1); break;
			case kVK_Command: _flags.ignoreKeyboardInput = !!(flags & NSCommandKeyMask); break;
			default: break;
			}

		_keyboard->value_uint64 = _keyboardState.value_uint64;
		_keyboard = q_triple_buffer_produce(_keyboardBuffer);
		}


#	pragma mark - NSWindowDelegate


	- (NSSize) window:			 (NSWindow *) window
		   willUseFullScreenContentSize: (NSSize    ) proposedSize
		{
		[window setContentAspectRatio: NSZeroSize];

		_videoOutput.scaling = Q_SCALING_FIT;
		return proposedSize;
		}


	- (void) windowWillClose: (NSNotification *) notification
		{
		}


	- (void) windowWillEnterFullScreen: (NSNotification *) notification
		{
		_flags.isFullScreen = YES;
		}


	- (void) windowDidEnterFullScreen: (NSNotification *) notification
		{
		NSWindow *window = self.window;

		_videoOutput.scaling = Q_SCALING_NONE;

		_trackingArea = [[NSTrackingArea alloc]
			initWithRect: _videoOutput.bounds
			options:      NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveInKeyWindow
			owner:	      self
			userInfo:     nil];

		[window.contentView addTrackingArea: _trackingArea];
		[_trackingArea release];
		[self startHidePointerTimer];
		}


	- (void) windowWillExitFullScreen: (NSNotification *) notification
		{
		NSWindow *window = self.window;

		[window setContentAspectRatio: NS_MACHINE_SCREEN_SIZE];
		[window.contentView removeTrackingArea: _trackingArea];
		[_pointerVisibilityTimer invalidate];
		_pointerVisibilityTimer = nil;
		_trackingArea = nil;
		_flags.isFullScreen = NO;
		_videoOutput.scaling = Q_SCALING_EXPAND;
		[NSCursor unhide];
		}


#	pragma mark - Full Screen Tracking Area Listeners


	- (void) mouseEntered: (NSEvent *) event
		{[self startHidePointerTimer];}


	- (void) mouseExited: (NSEvent *) event
		{
		[_pointerVisibilityTimer invalidate];
		_pointerVisibilityTimer = nil;
		}


	- (void) mouseMoved: (NSEvent *) event
		{[self startHidePointerTimer];}



#	pragma mark - IBAction


	- (IBAction) saveDocument: (id) sender
		{
		}


	- (IBAction) saveDocumentAs: (id) sender
		{
		NSSavePanel *panel = [NSSavePanel savePanel];

		NSDateComponents *today = [[NSCalendar currentCalendar]
			components:
				NSEraCalendarUnit    | NSYearCalendarUnit   |
				NSMonthCalendarUnit  | NSDayCalendarUnit    |
				NSHourCalendarUnit   | NSMinuteCalendarUnit |
				NSSecondCalendarUnit
			fromDate: [NSDate date]];

		panel.allowedFileTypes = [NSArray arrayWithObjects: @"z80", @"sna", nil];
		panel.canSelectHiddenExtension = YES;

		panel.nameFieldStringValue = STRING
			(@"Snapshot (%04li-%02li-%02li %02li.%02li.%02li)",
			 (long)[today year], (long)[today month],  (long)[today day],
			 (long)[today hour], (long)[today minute], (long)[today second]);

		[panel beginSheetModalForWindow: self.window completionHandler: ^(NSInteger result)
			{
			if (result == NSFileHandlingPanelOKButton)
				{
				NSString *path = panel.URL.path;
				NSError *error;

				if ([self saveSnapshotAtPath: path error: &error])
					{
					}

				else [[NSAlert alertWithError: error] runModal];
				}
			}];
		}


	- (IBAction) copy: (id) sender
		{
		}


	- (IBAction) zoomImageToActualSize: (id) sender	{[self setZoom: 1.0];}
	- (IBAction) zoomImageX2:	    (id) sender {[self setZoom: 2.0];}
	- (IBAction) zoomImageX3:	    (id) sender {[self setZoom: 3.0];}


	- (IBAction) zoomImageToFit: (id) sender
		{[self setZoom: self.window.screen.frame.size.height / (qreal)Q_ZX_SPECTRUM_SCREEN_HEIGHT];}


	- (IBAction) zoomIn: (id) sender
		{
		[self setZoom: floor([self currentZoom] / 0.5) * 0.5 + 0.5];
		}

	- (IBAction) zoomOut: (id) sender
		{
		CGFloat factor = floor([self currentZoom] / 0.5) * 0.5 - 0.5;

		[self setZoom: factor <= 1.0 ? 1.0 : factor];
		}


	- (IBAction) toggleLinearInterpolation: (NSMenuItem *) sender
		{
		BOOL enable = sender.state == NSOnState ? NSOffState : NSOnState;

		sender.state = enable;
		[_videoOutput setLinearInterpolation: enable];
		}


	- (IBAction) editWindowTitle: (id) sender
		{
		NSString *currentTitle = self.window.title;
		NSString *placeHolder = [NSString stringWithUTF8String: _machineABI->model_name];

		[titleTextField.cell setPlaceholderString: placeHolder];
		[titleTextField setStringValue: [currentTitle isEqualToString: placeHolder] ? @"" : currentTitle];

		[NSApp	beginSheet:	titleWindow
			modalForWindow: [self window]
			modalDelegate:	self
			didEndSelector:	nil
			contextInfo:	nil];
		}


	- (IBAction) editWindowTitleOK: (id) sender
		{
		NSString *title = [[titleTextField stringValue]
			stringByTrimmingCharactersInSet: [NSCharacterSet whitespaceAndNewlineCharacterSet]];

		self.window.title = (title && ![title isEqualToString: @""])
			? title
			: [NSString stringWithUTF8String: _machineABI->model_name];

		[NSApp endSheet: titleWindow];
		[titleWindow orderOut: self];
		}


	- (IBAction) editWindowTitleCancel: (id) sender
		{
		[NSApp endSheet: titleWindow];
		[titleWindow orderOut: self];
		}



#	pragma mark - NSAnimationDelegate Protocol


/*	- (void) animationDidEnd: (NSAnimation *) animation
		{
		NSLog(@"animationDidEnd");
		_videoOutput.contentSize = q_2d_fit(MACHINE_SCREEN_SIZE, NSSizeToQ(_fullScreenWindow.frame.size));
		_videoOutput.scaling = Q_SCALING_NONE;
		[_videoOutput retain];
		[_videoOutput removeFromSuperview];
		_fullScreenWindow.contentView = _videoOutput;
		[_videoOutput release];
		_fullScreenTransitionView = nil;
		[NSMenu setMenuBarVisible: NO];
		}


	- (IBAction) toggleFullScreen: (id) sender
		{
		NSWindow *window = self.window;
		NSRect screenFrame = window.screen.frame;
		NSRect startFrame = [_videoOutput frameInScreen];
		NSRect videoOutputEndFrame;

		_flags.isFullScreen = YES;

		_fullScreenWindow = [[NSWindow alloc]
			initWithContentRect: startFrame
			styleMask:	     NSBorderlessWindowMask
			backing:	     NSBackingStoreBuffered
			defer:		     YES];

		_fullScreenWindow.level = NSFloatingWindowLevel;
		_fullScreenWindow.title = window.title;
		_fullScreenWindow.backgroundColor = [NSColor blackColor];

		_fullScreenTransitionView = [[NSImageView alloc] initWithFrame: _videoOutput.bounds];
		_fullScreenWindow.contentView = _fullScreenTransitionView;
		[_fullScreenTransitionView release];
		_fullScreenTransitionView.imageScaling = NSImageScaleProportionallyUpOrDown;
		_fullScreenTransitionView.image = [_videoOutput imageCapture];

		[_fullScreenWindow makeKeyAndOrderFront: nil];

		NSViewAnimation *animation = [[NSViewAnimation alloc] initWithViewAnimations: [NSArray arrayWithObjects:
			[NSMutableDictionary dictionaryWithObjectsAndKeys:
				_fullScreenWindow,			 NSViewAnimationTargetKey,
				//NSViewAnimationFadeOutEffect,		 NSViewAnimationEffectKey,
				[NSValue valueWithRect: startFrame],	 NSViewAnimationStartFrameKey,
				[NSValue valueWithRect: screenFrame],	 NSViewAnimationEndFrameKey,
				nil],
			nil]];

		[animation setDuration: 0.29];
		[animation setDelegate: self];
		[animation setAnimationCurve: NSAnimationEaseIn];
		[animation startAnimation];
		}*/


#	pragma mark - AudioOutput Protocol


	- (void) submitAudioFrame: (void *) frame
		{
		ring_buffer_write(_audioInputRing, frame);
		}


#	pragma mark - IBAction


	- (IBAction) toggleKeyboard: (id) sender
		{
		}


	- (IBAction) toggleTapeRecorder: (NSMenuItem *) sender
		{
		//_attachInputBuffer = YES;

		if (_tapeRecorderController)
			{
			sender.state = NSOffState;
			[_tapeRecorderController removeOutput: self];
			[_tapeRecorderController release];
			_tapeRecorderController = nil;
			ring_buffer_destroy(_audioInputRing);
			free(_audioInputBuffer);
			_audioInputRing = NULL;
			}

		else	{
			sender.state = NSOnState;
			_audioInputBuffer = malloc(882);
			_audioInputRing = ring_buffer_new(3, 882);
			_tapeRecorderController = [[TapeRecorderController alloc] init];
			[_tapeRecorderController setFrameSize: 882 count: 4];
			[_tapeRecorderController addOutput: self action: @selector(submitAudioFrame:)];
			[_tapeRecorderController.window makeKeyAndOrderFront: self];
			}
		}


	- (IBAction) toggleDebugger: (id) sender
		{
		}


	- (IBAction) toggleVideoRecording: (id) sender
		{
		}


#	pragma mark - Public


	- (void) toggleMachinePower: (NSMenuItem *) sender
		{
		if (sender.state == NSOnState)
			{
			_machineABI->power(_machine, OFF);
			sender.state = NSOffState;
			}

		if (sender.state == NSOffState)
			{
			_machineABI->power(_machine, ON);
			sender.state = NSOnState;
			}
		}


	- (void) start
		{
		}


	- (void) resetMachine: (id) sender
		{
		_machineABI->reset(_machine);
		}


	- (void) toggleMachinePause: (id) sender
		{
		}


	- (void) emulateMachineFrame
		{
		_machineABI->run_one_frame(_machine);
		}


@end
