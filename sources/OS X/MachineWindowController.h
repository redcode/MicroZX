/*     _________  ___
 _____ \_   /\  \/  / OS X/MachineWindowController.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#import <Foundation/Foundation.h>
#import "Machine.hpp"
#import "GLVideoOutputView.h"
#import "CoreAudioOutputPlayer.h"
#import "TapeRecorderWindowController.h"
#import "ZX Spectrum.h" // Eliminar
#import "SNA.h"
#import <Z/types/buffering.h>
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
	Machine*		_machine;

	//----------.
	// Geometry |
	//----------'
	ZKit::Value2D<ZKit::Real> _minimumWindowSize;

	//-------------.
	// I/O Buffers |
	//-------------'
	//QTripleBuffer*	_videoOutputBuffer;
	//QRingBuffer*		_audioOutputBuffer;
	ZKit::TripleBuffer*	_keyboardBuffer;
	Z64Bit*			_keyboard;
	Z64Bit			_keyboardState;

	BOOL			_smooth;
	GLVideoOutputView*	_videoOutputView;
	CoreAudioOutputPlayer*	_audioOutputPlayer;
	BOOL			_keyboardInput;
	ZRingBuffer		_audioInputBuffer;

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
	ZKit::Size   _tapeSampleCount;
	ZKit::UInt8* _tapeSamples;
	BOOL	     _attachInputBuffer;
}
	- (id) initWithMachineABI: (MachineABI *) machineABI;

	//--------------------.
	// Main Menu: Machine |
	//--------------------'
	- (IBAction) togglePower:  (id) sender;
	- (IBAction) togglePause:  (id) sender;
	- (IBAction) reset:	   (id) sender;
	- (IBAction) saveState:	   (id) sender;
	- (IBAction) restoreState: (id) sender;
	- (IBAction) showStates:   (id) sender;
	- (IBAction) toggleVSync:  (id) sender;
	- (IBAction) setFrameSkip: (id) sender;

	//-----------------.
	// Main Menu: Edit |
	//-----------------'
	- (IBAction) copy: (id) sender;

	//-----------------.
	// Main Menu: View |
	//-----------------'
	- (IBAction) zoomIn:		      (id) sender;
	- (IBAction) zoomOut:		      (id) sender;
	- (IBAction) zoomToFit:		      (id) sender;
	- (IBAction) zoomTo1x:		      (id) sender;
	- (IBAction) zoomTo2x:		      (id) sender;
	- (IBAction) zoomTo3x:		      (id) sender;
	- (IBAction) toggleSmooth:	      (id) sender;
	- (IBAction) toggleKeyboardShown:     (id) sender;
	- (IBAction) toggleTapeRecorderShown: (id) sender;
	- (IBAction) toggleDebuggerShown:     (id) sender;
	- (IBAction) toggleCPUShown:	      (id) sender;
	- (IBAction) toggleULAShown:	      (id) sender;
	- (IBAction) togglePSGShown:	      (id) sender;

	//-------------------.
	// Main Menu: Window |
	//-------------------'
	- (IBAction) editWindowTitle: (id) sender;
@end
