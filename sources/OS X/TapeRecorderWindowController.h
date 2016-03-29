/*     _________  ___
 _____ \_   /\  \/  / OS X/TapeRecorderWindowController.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#import "TapeRecorderWindow.h"
#import "CoreAudioOutputPlayer.h"
#import <Z/types/base.h>
#import <pthread.h>

typedef struct {
} AudioIOCB;

typedef struct {
	zsize	(* test_data)			(void*	    data);

	zsize	(* object_size_for_data)	(void*	    data);

	zsize	(* set_cb)			(void*	    object,
						 AudioIOCB* cb);

	zsize	(* sample_count)		(void*	    object);

	zsize	(* read)			(void*	    object,
						 zsize	    sample_count,
						 void*	    output);

} AudioIOABI;

typedef struct {
	void*	buffer;
	void*	samples;
	zsize	sample_count;
	zsize	frame_size;
	zsize	frame_count;
	zsize	frame_index;
	IMP	output_method;
	SEL	output_selector;
	id	output;
} TapeRecorder;

@interface TapeRecorderWindowController : NSWindowController <TapeRecorderWindowDelegate> {
	IBOutlet NSView*      contentView;
	IBOutlet NSImageView* tapeView;

	TapeRecorder _tapeRecorder;
	NSString*    _filePath;
	NSTimer*     _timer;
	BOOL	     _playing;
	pthread_t    _thread;
}
	- (void) setFrameSize: (zsize) frameSize
		 count:	       (zsize) count;

	- (void) addOutput: (id)  output
		 action:    (SEL) action;

	- (void) removeOutput: (id) output;

	- (IBAction) eject:		     (id) sender;
	- (IBAction) play:		     (id) sender;
	- (IBAction) record:		     (id) sender;
	- (IBAction) rewindToPreviousBlock:  (id) sender;
	- (IBAction) rewind:		     (id) sender;
	- (IBAction) windForward:	     (id) sender;
	- (IBAction) windForwardToNextBlock: (id) sender;
@end
