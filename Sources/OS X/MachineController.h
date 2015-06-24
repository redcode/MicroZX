/*	_________  ___
  _____ \_   /\  \/  /	μZX - OS X/MachineController.h
 |  |  |_/  /__>    <	Copyright © 2013 RedCode Software.
 |   ____________/\__\	All rights reserved.
 |_*/

#import <Foundation/Foundation.h>
#import "MachineABI.h"
#import "GLOutputView.h"
#import "CoreAudioOutput.h"
#import "TapeRecorderController.h"
#import "ZX Spectrum.h" // Eliminar
#import "SNA.h"
#import "RingBuffer.h"
#import <Q/types/buffering.h>
#import <pthread.h>
#import "ALOutputPlayer.h"

@interface MachineController : NSWindowController <NSWindowDelegate, NSAnimationDelegate> {

	//-------------.
	// XIB objects |
	//-------------'
	IBOutlet NSWindow*	titleWindow;
	IBOutlet NSTextField*	titleTextField;
	IBOutlet NSArray*	customMenus;

	@public

	//-----------.
	// Emulation |
	//-----------'
	ZXSpectrum*		_machine;
	MachineABI*		_machineABI;
	void*			_memory;
	pthread_t		_thread;

	//----------.
	// Geometry |
	//----------'
	Q2D			_minimumWindowSize;

	//-------------.
	// I/O Buffers |
	//-------------'
	QTripleBuffer*		_videoOutputBuffer;
	QRingBuffer*		_audioOutputBuffer;
	QTripleBuffer*		_keyboardBuffer;
	Q64Bit*			_keyboard;
	Q64Bit			_keyboardState;

	BOOL			_smooth;
	GLOutputView*		_videoOutput;
	CoreAudioOutput*	_audioOutput;
	//ALOutputPlayer*		_audioOutput;
	BOOL			_keyboardInput;
	RingBuffer*		_audioInputRing;
	quint8*			_audioInputBuffer;

	NSTimer*		_pointerVisibilityTimer;
	NSTrackingArea*		_trackingArea;

	//----------------------.
	// Pre-Lion Full Screen |
	//----------------------'
	NSWindow*		_fullScreenWindow;
	NSImageView*		_fullScreenTransitionView;

	//--------.
	// Panels |
	//--------'
	//KeyboardController*	_keyboardController;
	//DebuggerController*	_debuggerController;
	TapeRecorderController*	_tapeRecorderController;


	volatile BOOL _mustStop;

	struct {BOOL isFullScreen	 :1;
		BOOL ignoreKeyboardInput :1;
	} _flags;


	// Temporal
	qsize	_tapeSampleCount;
	quint8*	_tapeSamples;
	BOOL	_attachInputBuffer;
}

	@property (nonatomic, readonly) NSArray *customMenus;

	//@property (nonatomic, readonly) NSMenu* menu;

	- (id) initWithMachineABI: (MachineABI *) machineABI;

	- (void) start;
	- (void) stop;

	//---------------.
	// Menu: Machine |
	//---------------'
	- (IBAction) toggleMachinePower:	(id) sender;
	- (IBAction) resetMachine:		(id) sender;
	- (IBAction) toggleMachinePause:	(id) sender;
	- (IBAction) toggleKeyboard:		(id) sender;
	- (IBAction) toggleTapeRecorder:	(id) sender;
	- (IBAction) toggleDebugger:		(id) sender;
	- (IBAction) toggleVideoRecording:	(id) sender;

@end
