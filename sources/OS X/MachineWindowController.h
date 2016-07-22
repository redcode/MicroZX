/*     _________  ___
 _____ \_   /\  \/  / OS X/MachineWindowController.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#import "Machine.hpp"
#import "GLVideoView.h"
#import "CoreAudioOutputPlayer.hpp"
#import "TapeRecorderWindowController.h"
#import "SNA.h"

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
	Machine* _machine;

	//----------.
	// Geometry |
	//----------'
	Zeta::Value2D<Zeta::Real> _minimumWindowSize;

	//-------------.
	// I/O Buffers |
	//-------------'
	Zeta::TripleBuffer* _keyboardBuffer;
	Z64Bit*		    _keyboard;
	Z64Bit		    _keyboardState;

	BOOL		       _smooth;
	GLVideoView*	       _videoOutputView;
	CoreAudioOutputPlayer* _audioOutputPlayer;
	BOOL			_keyboardInput;
	Zeta::RingBuffer	_audioInputBuffer;

	NSTimer*		_pointerVisibilityTimer;
	NSTrackingArea*		_trackingArea;

	//--------.
	// Panels |
	//--------'
	//KeyboardWindowController*     _keyboardWindowController;
	//DebuggerWindowController*     _debuggerWindowController;
	TapeRecorderWindowController* _tapeRecorderWindowController;

	struct {BOOL isFullScreen	 :1;
		BOOL ignoreKeyboardInput :1;
	} _flags;

	// Temporal
	Zeta::Size   _tapeSampleCount;
	Zeta::UInt8* _tapeSamples;
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
