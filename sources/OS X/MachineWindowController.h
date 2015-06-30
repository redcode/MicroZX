/*	_________  ___
  _____ \_   /\  \/  /	OS X/MachineWindowController.h
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import <Foundation/Foundation.h>
#import "Machine.h"
#import "GLOutputView.h"
#import "CoreAudioOutput.h"
#import "TapeRecorderWindowController.h"
#import "ZX Spectrum.h" // Eliminar
#import "SNA.h"
#import "RingBuffer.h"
#import <Q/types/buffering.h>
#import <pthread.h>

@interface MachineWindowController : NSWindowController <NSWindowDelegate, NSAnimationDelegate> {

	//-------------.
	// XIB objects |
	//-------------'
	IBOutlet NSWindow*    titleWindow;
	IBOutlet NSTextField* titleTextField;

	@public

	//-----------.
	// Emulation |
	//-----------'
	Machine			_machine;

	//ZXSpectrum*		_machine;
	//MachineABI*		_machineABI;
	//void*			_memory;
	//pthread_t		_thread;

	//----------.
	// Geometry |
	//----------'
	Q2D			_minimumWindowSize;

	//-------------.
	// I/O Buffers |
	//-------------'
	//QTripleBuffer*		_videoOutputBuffer;
	//QRingBuffer*		_audioOutputBuffer;
	QTripleBuffer*		_keyboardBuffer;
	Q64Bit*			_keyboard;
	Q64Bit			_keyboardState;

	BOOL			_smooth;
	GLOutputView*		_videoOutput;
	CoreAudioOutput*	_audioOutput;
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
	//KeyboardWindowController*     _keyboardWindowController;
	//DebuggerWindowController*     _debuggerWindowController;
	TapeRecorderWindowController* _tapeRecorderWindowController;

	volatile BOOL _mustStop;

	struct {BOOL isFullScreen	 :1;
		BOOL ignoreKeyboardInput :1;
	} _flags;

	// Temporal
	qsize	_tapeSampleCount;
	quint8*	_tapeSamples;
	BOOL	_attachInputBuffer;
}
	- (id) initWithMachineABI: (MachineABI *) machineABI;

	- (void) start;
	- (void) stop;

	//--------------------.
	// Main Menu: Machine |
	//--------------------'
	- (IBAction) power: (id) sender;
	- (IBAction) pause: (id) sender;
	- (IBAction) reset: (id) sender;

	//-----------------.
	// Main Menu: Edit |
	//-----------------'
	- (IBAction) copy: (id) sender;

	//-----------------.
	// Main Menu: View |
	//-----------------'
	- (IBAction) zoomIn:	   (id) sender;
	- (IBAction) zoomOut:	   (id) sender;
	- (IBAction) zoomToFit:	   (id) sender;
	- (IBAction) zoomTo1x:	   (id) sender;
	- (IBAction) zoomTo2x:	   (id) sender;
	- (IBAction) zoomTo3x:	   (id) sender;
	- (IBAction) smooth:	   (id) sender;
	- (IBAction) keyboard:	   (id) sender;
	- (IBAction) tapeRecorder: (id) sender;
	- (IBAction) debugger:	   (id) sender;
	- (IBAction) CPU:	   (id) sender;
	- (IBAction) ULA:	   (id) sender;
	- (IBAction) PSG:	   (id) sender;

	//-------------------.
	// Main Menu: Window |
	//-------------------'
	- (IBAction) editWindowTitle: (id) sender;
@end
