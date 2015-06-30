/*	_________  ___
  _____ \_   /\  \/  /	OS X/TapeRecorderController.h
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import "TapeRecorderWindow.h"
#import "CoreAudioOutput.h"
#import <Q/types/data.h>
#import "RingBuffer.h"
#import <pthread.h>

typedef struct {
} AudioIOCB;

typedef struct {
	qsize	(* test_data)			(void*	    data);

	qsize	(* object_size_for_data)	(void*	    data);

	qsize	(* set_cb)			(void*	    object,
						 AudioIOCB* cb);

	qsize	(* sample_count)		(void*	    object);

	qsize	(* read)			(void*	    object,
						 qsize	    sample_count,
						 void*	    output);

} AudioIOABI;

typedef struct {
	void*	buffer;
	void*	samples;
	qsize	sample_count;
	qsize	frame_size;
	qsize	frame_count;
	qsize	frame_index;
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
	- (void) setFrameSize: (qsize) frameSize
		 count:	       (qsize) count;

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
