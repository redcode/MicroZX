/*     _________  ___
 _____ \_   /\  \/  / OS X/MachineWindowController.mm
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#import "MachineWindowController.h"
#import "KeyCodes.h"
#import <Z/hardware/machine/platform/computer/ZX Spectrum.h>
#import <Z/hardware/machine/model/computer/ZX Spectrum/ZX Spectrum.h>
#import "NSWindow+RedCode.h"
#import "NSView+RedCode.h"

#define kZoomIncrement	0.5
#define SCREEN_SIZE_X	Z_JOIN_2(Z_ZX_SPECTRUM_SCREEN_WIDTH,  .0)
#define SCREEN_SIZE_Y	Z_JOIN_2(Z_ZX_SPECTRUM_SCREEN_HEIGHT, .0)
#define SCREEN_SIZE	Value2D<Real>(Z_ZX_SPECTRUM_SCREEN_WIDTH, Z_ZX_SPECTRUM_SCREEN_HEIGHT)
#define NS_SCREEN_SIZE	NSMakeSize   (Z_ZX_SPECTRUM_SCREEN_WIDTH, Z_ZX_SPECTRUM_SCREEN_HEIGHT)

using namespace Zeta;

typedef struct {
	struct {zuint8 row  :3;
		zuint8 mask :5;
	} modifier_key;

	struct {zuint8 row  :3;
		zuint8 mask :5;
	} key;
} KeyCode;

#define KEY_1(KEY_NAME)					\
	{{0, 0},					\
	 {Z_JOIN_2(Z_ZX_SPECTRUM_KEY_ROW_,  KEY_NAME),	\
	  Z_JOIN_2(Z_ZX_SPECTRUM_KEY_MASK_, KEY_NAME)}}

#define KEY_2(MODIFIER_KEY_NAME, KEY_NAME)			 \
	{{Z_JOIN_2(Z_ZX_SPECTRUM_KEY_ROW_,  MODIFIER_KEY_NAME),	 \
	  Z_JOIN_2(Z_ZX_SPECTRUM_KEY_MASK_, MODIFIER_KEY_NAME)}, \
	 {Z_JOIN_2(Z_ZX_SPECTRUM_KEY_ROW_,  KEY_NAME),		 \
	  Z_JOIN_2(Z_ZX_SPECTRUM_KEY_MASK_, KEY_NAME)}}


Z_INLINE Real step_down(Real n, Real step_size)
	{
	Real factor = n / step_size;
	Real step = ceil(factor);

	return step < factor ? step * step_size : step * step_size - step_size;
	}


Z_INLINE Real step_up(Real n, Real step_size)
	{return floor(n / step_size) * step_size + step_size;}


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
		[pasteboard setData: [[_videoOutputView imageCapture] TIFFRepresentation] forType: NSTIFFPboardType];
		}


	- (CGFloat) zoom
		{
		return _flags.isFullScreen
			? _videoOutputView.contentSize.x / SCREEN_SIZE_X
			: ((NSView *)self.window.contentView).bounds.size.width / SCREEN_SIZE_X;
		}


	- (void) setZoom: (zreal) zoom
		{
		if (_flags.isFullScreen)
			{
			Value2D<Real> zoomedSize = SCREEN_SIZE * zoom;
			Value2D<Real> boundsSize(_videoOutputView.bounds.size);

			_videoOutputView.contentSize = boundsSize.contains(zoomedSize)
				? zoomedSize
				: SCREEN_SIZE.fit(boundsSize);
			}

		else	{
			NSWindow *window = self.window;

			Rectangle<Real> screenFrame(window.screen.visibleFrame);
			Value2D	 <Real> borderSize(window.borderSize);
			Value2D	 <Real> newSize = borderSize + SCREEN_SIZE * zoom;

			[window animateIntoScreenFrame: screenFrame
				fromTopCenterToSize:	screenFrame.size.contains(newSize)
					? newSize
					: SCREEN_SIZE.fit(screenFrame.size - borderSize) + borderSize];
			}
		}


	- (BOOL) saveSnapshotAtPath: (NSString *) path
		 error:		     (NSError **) error
		{
		return YES;
		}


#	pragma mark - Life cycle


	- (id) initWithMachineABI: (MachineABI *) machineABI
		{
		if ((self = [super initWithWindowNibName: @"MachineWindow"]))
			{
			_smooth = YES;

			/*--------------------------.
			| Create auxiliary controls |
			'--------------------------*/
			[NSBundle loadNibNamed: @"Title Sheet" owner: self];

			/*----------------------------.
			| Create video output object. |
			'----------------------------*/
			_videoOutputView = [[GLVideoView alloc] initWithFrame:
				NSMakeRect(0.0, 0.0, SCREEN_SIZE_X, SCREEN_SIZE_Y)];

			[_videoOutputView
				setResolution: Value2D<Zeta::Size>(SCREEN_SIZE_X, SCREEN_SIZE_Y)
				format:	       0];

			_videoOutputView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

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

			//machineABI->initialize(_machine);

			_keyboardState.value_uint64 = Type<Zeta::UInt64>::maximum;

			/*NSData *data = [NSData dataWithContentsOfFile: @"/Users/manuel/Desktop/Batman.sna"];
			ZSNAv48K *sna = (ZSNAv48K *)[data bytes];

			sna_v48k_decode(sna, &_machine->state, &_machine->cpu->state, _machine->memory);*/
			//_machine->border_color = Z_RGBA32(FF, 00, 00, 00);*/

			_attachInputBuffer = NO;


			//machine_start(&_machine);
			}

		return self;
		}


#	pragma mark - Overwritten


	- (void) dealloc
		{
		[_videoOutputView stop];
		_audioOutputPlayer->stop();
		_machine->power(OFF);
		[titleWindow release];
		[_tapeRecorderWindowController release];
		[_pointerVisibilityTimer invalidate];
		[_videoOutputView release];
		delete _audioOutputPlayer;
		delete _machine;
		free(_keyboardBuffer->buffers[0]);
		free(_keyboardBuffer);
		[super dealloc];
		}


	- (void) windowDidLoad
		{
		[super windowDidLoad];
		//[[self window] disableScreenUpdatesUntilFlush];
		NSWindow *window = self.window;
		NSSize contentSize = NS_SCREEN_SIZE;

		if (	[window respondsToSelector: @selector(setAnimationBehavior:) ] &&
			[window respondsToSelector: @selector(setCollectionBehavior:)]
		)
			[window setCollectionBehavior: [window collectionBehavior] | NSWindowCollectionBehaviorFullScreenPrimary];

		_minimumWindowSize = SCREEN_SIZE + window.borderSize;
		window.title = [NSString stringWithUTF8String: _machine->abi->model_name];
		[window.contentView addSubview: _videoOutputView];
		[window setContentAspectRatio: contentSize];
		[window setContentMinSize: contentSize];

		[_videoOutputView start];
		_audioOutputPlayer->start();
		_machine->power(ON);
		}


	- (BOOL) validateMenuItem: (NSMenuItem *) menuItem
		{
		SEL action = menuItem.action;

		if ([self respondsToSelector: action])
			{
			if (action == @selector(togglePower:))
				menuItem.state = _machine->flags.power ? NSOnState : NSOffState;

			else if (action == @selector(togglePause:))
				{
				if (!_machine->flags.power)
					{
					menuItem.state = NSOffState;
					return NO;
					}

				menuItem.state = _machine->flags.pause ? NSOnState : NSOffState;
				}

			else if (action == @selector(reset:) || action == @selector(saveState:))
				{
				if (!_machine->flags.power) return NO;
				}

			return YES;
			}

		return NO;
		}


#	pragma mark - Overwritten: Input Control


#	define KEY_DOWN(line, bit) _keyboardState.array_uint8[line] &= ~(1 << bit)
#	define KEY_UP(	line, bit) _keyboardState.array_uint8[line] |=  (1 << bit)

	- (void) keyDown: (NSEvent *) event
		{
		//NSLog(@"keyDown: %X, %@, %lu", [event keyCode], [event characters], strlen([[event characters] UTF8String]));

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
		_keyboard = (Z64Bit *)_keyboardBuffer->produce();
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
		_keyboard = (Z64Bit *)_keyboardBuffer->produce();
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
		_keyboard = (Z64Bit *)_keyboardBuffer->produce();
		}


#	pragma mark - Overwritten: Full Screen Tracking Area Listeners


	- (void) mouseUp: (NSEvent *) event
		{if (event.clickCount == 2) [self.window toggleFullScreen: self];}


	- (void) mouseEntered: (NSEvent *) event
		{[self startHidePointerTimer];}


	- (void) mouseExited: (NSEvent *) event
		{
		[_pointerVisibilityTimer invalidate];
		_pointerVisibilityTimer = nil;
		}


	- (void) mouseMoved: (NSEvent *) event
		{[self startHidePointerTimer];}


#	pragma mark - NSWindowDelegate


	- (NSSize) window:			 (NSWindow *) window
		   willUseFullScreenContentSize: (NSSize    ) proposedSize
		{
		[window setContentAspectRatio: NSZeroSize];

		_videoOutputView.scaling = Z_SCALING_FIT;
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

		_videoOutputView.scaling = Z_SCALING_NONE;

		_trackingArea = [[NSTrackingArea alloc]
			initWithRect: _videoOutputView.bounds
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

		[window setContentAspectRatio: NS_SCREEN_SIZE];
		[window.contentView removeTrackingArea: _trackingArea];
		[_pointerVisibilityTimer invalidate];
		_pointerVisibilityTimer = nil;
		_trackingArea = nil;
		_flags.isFullScreen = NO;
		_videoOutputView.scaling = Z_SCALING_EXPAND;
		[NSCursor unhide];
		}


#	pragma mark - AudioOutput Protocol


	- (void) submitAudioFrame: (void *) frame
		{
		//void *buffer = z_ring_buffer_production_buffer(_machine->_audio_input);

		//memcpy(buffer, frame, _machine->_audio_input->buffer_size);
		//z_ring_buffer_produce(_machine->_audio_input);
		}


#	pragma mark - IBAction: Main Menu - File


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


#	pragma mark - IBAction: Main Menu - Edit


	- (IBAction) copy: (id) sender
		{
		}


#	pragma mark - IBAction: Main Menu - Machine


	- (IBAction) togglePower: (NSMenuItem *) sender
		{
		zboolean state = !_machine->flags.power;

		_machine->power(state);

		if (state)
			{
			[_videoOutputView start];
			_audioOutputPlayer->start();
			}

		else	{
			_audioOutputPlayer->stop();
			[_videoOutputView stop];
			[_videoOutputView blank];
			}
		}


	- (IBAction) togglePause: (id) sender
		{
		zboolean state = !_machine->flags.pause;

		_machine->pause(state);

		if (state)
			{
			_audioOutputPlayer->stop();
			[_videoOutputView stop];
			}

		else	{
			[_videoOutputView start];
			_audioOutputPlayer->start();
			}
		}


	- (IBAction) reset: (id) sender
		{
		zboolean pause = _machine->flags.pause;

		_machine->reset();

		if (pause)
			{
			[_videoOutputView start];
			_audioOutputPlayer->start();
			}
		}



	- (IBAction) saveState: (id) sender
		{
		}


	- (IBAction) restoreState: (id) sender
		{
		}


	- (IBAction) showStates: (id) sender
		{
		}


	- (IBAction) toggleVSync: (id) sender
		{
		}


	- (IBAction) setFrameSkip: (id) sender
		{
		}


#	pragma mark - IBAction: Main Menu - View


	- (IBAction) zoomIn: (id) sender
		{self.zoom = step_up(self.zoom, kZoomIncrement);}


	- (IBAction) zoomOut: (id) sender
		{
		zreal zoom = step_down(self.zoom, kZoomIncrement);

		self.zoom = zoom <= 1.0 ? 1.0 : zoom;
		}


	- (IBAction) zoomToFit: (id) sender
		{[self setZoom: self.window.screen.frame.size.height / SCREEN_SIZE_Y];}


	- (IBAction) zoomTo1x: (id) sender {[self setZoom: 1.0];}
	- (IBAction) zoomTo2x: (id) sender {[self setZoom: 2.0];}
	- (IBAction) zoomTo3x: (id) sender {[self setZoom: 3.0];}


	- (IBAction) toggleSmooth: (NSMenuItem *) sender;
		{
		BOOL enable = sender.state == NSOnState ? NSOffState : NSOnState;

		sender.state = enable;
		[_videoOutputView setLinearInterpolation: enable];
		}


	- (IBAction) toggleKeyboardShown: (id) sender
		{
		}


	- (IBAction) toggleTapeRecorderShown: (NSMenuItem *) sender
		{
		if (_tapeRecorderWindowController)
			{
			sender.state = NSOffState;
			[_tapeRecorderWindowController removeOutput: self];
			[_tapeRecorderWindowController release];
			_tapeRecorderWindowController = nil;
		//	ring_buffer_destroy(_machine.audio_input_buffer);
		//	_machine->_audio_input = NULL;
			}

		else	{
			sender.state = NSOnState;
			void *buffer = calloc(3, 882);
			_audioInputBuffer.initialize(buffer, 882, 3);
			_machine->set_audio_input(&_audioInputBuffer);
			_tapeRecorderWindowController = [[TapeRecorderWindowController alloc] init];
			[_tapeRecorderWindowController setFrameSize: 882 count: 4];
			[_tapeRecorderWindowController addOutput: self action: @selector(submitAudioFrame:)];
			[_tapeRecorderWindowController.window makeKeyAndOrderFront: self];
			}
		}


	- (IBAction) toggleDebuggerShown: (id) sender
		{
		}


	- (IBAction) toggleCPUShown: (id) sender
		{
		}


	- (IBAction) toggleULAShown: (id) sender
		{
		}


	- (IBAction) togglePSGShown: (id) sender
		{
		}


#	pragma mark - IBAction: Main Menu - Window


	- (IBAction) editWindowTitle: (id) sender
		{
		NSString *currentTitle = self.window.title;
		NSString *placeHolder = [NSString stringWithUTF8String: _machine->abi->model_name];

		[titleTextField.cell setPlaceholderString: placeHolder];
		[titleTextField setStringValue: [currentTitle isEqualToString: placeHolder] ? @"" : currentTitle];

		[NSApp	beginSheet:	titleWindow
			modalForWindow: [self window]
			modalDelegate:	self
			didEndSelector:	nil
			contextInfo:	nil];
		}


#	pragma mark - IBAction: Window Title Editor


	- (IBAction) editWindowTitleOK: (id) sender
		{
		NSString *title = [[titleTextField stringValue]
			stringByTrimmingCharactersInSet: [NSCharacterSet whitespaceAndNewlineCharacterSet]];

		self.window.title = (title && ![title isEqualToString: @""])
			? title
			: [NSString stringWithUTF8String: _machine->abi->model_name];

		[NSApp endSheet: titleWindow];
		[titleWindow orderOut: self];
		}


	- (IBAction) editWindowTitleCancel: (id) sender
		{
		[NSApp endSheet: titleWindow];
		[titleWindow orderOut: self];
		}


@end
