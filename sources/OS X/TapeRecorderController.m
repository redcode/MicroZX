/*	_________  ___
  _____ \_   /\  \/  /	OS X/TapeRecorderController.m
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#import "TapeRecorderController.h"
#import <Q/formats/storage medium image/audio/TZX.h>
#import <CoreAudio/CoreAudio.h>
#import <time.h>


void tape_recorder_play_frame(TapeRecorder *object)
	{
	//object->output_method(object->output, object->output_selector, object->samples + object->frame_size * object->frame_index);
	object->frame_index++;
	}


void *tape_recorder_play(TapeRecorder *object)
	{
	return NULL;
	}




@implementation TapeRecorderController


#	pragma mark - Helpers


	- (void) loadTapeAtPath: (NSString *) path
		{
		NSFileManager *fileManager = [NSFileManager defaultManager];

		if ([fileManager fileExistsAtPath: path])
			{
			NSData *data = [NSData dataWithContentsOfFile: path];
			_tapeRecorder.sample_count = [data length];
			_tapeRecorder.samples = malloc([data length] * 2);
			memcpy(_tapeRecorder.samples, [data bytes], _tapeRecorder.sample_count);
			memset(_tapeRecorder.samples + _tapeRecorder.sample_count, 0x70, _tapeRecorder.sample_count);
			_filePath = [path retain];
			[tapeView setHidden: NO];
			}
		}

	- (void) playFrame
		{
		((void (*)(id,SEL, quint8 *))_tapeRecorder.output_method)(_tapeRecorder.output, _tapeRecorder.output_selector, _tapeRecorder.samples + _tapeRecorder.frame_size * _tapeRecorder.frame_index);
		_tapeRecorder.frame_index++;
		//NSLog(@"playFrame");
		}


	- (void) startPlayThread
		{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

		_timer = [NSTimer
			scheduledTimerWithTimeInterval: 1.0 / 50.0
			target:				self
			selector:			@selector(playFrame)
			userInfo:			nil
			repeats:			YES];

		[[NSRunLoop currentRunLoop]
			addTimer: _timer
			forMode:  NSDefaultRunLoopMode];

		[[NSRunLoop currentRunLoop] run];

		NSLog(@"runloop stoped");

		[pool drain];
		}


	- (void) stopPlayThread
		{
		[_timer invalidate];
		_timer = nil;
		CFRunLoopStop([[NSRunLoop currentRunLoop] getCFRunLoop]);
		}


#	pragma mark - TapeRecorderWindowDelegate Protocol


	- (void) tapeRecorderWindow:	      (TapeRecorderWindow *) tapeRecorderWindow
		 didReceiveDragForFileAtPath: (NSString *)	     path
		{
		[self loadTapeAtPath: path];
		}


#	pragma mark - NSWindowDelegate


#	pragma mark - Public


	- (id) init
		{
		if ((self = [super init]))
			{
			[NSBundle loadNibNamed: @"TapeRecorder" owner: self];
			NSSize contentSize = contentView.bounds.size;
		//	NSWindow *window = [[DevicePanel alloc] initWithContentRect: NSMakeRect(100.0, 100.0, contentSize.width, contentSize.height)];

		//	window.contentView = contentView;
		//	[window orderFront: self];
			}

		return self;
		}


	- (void) dealloc
		{
/*		[_timer invalidate];
		if (_samples != NULL) free(_samples);
		[_filePath release];*/
		[super dealloc];
		}


	- (void) windowDidLoad
		{
		[super windowDidLoad];
		[self.window registerForDraggedTypes: [NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
		}


	- (void) setFrameSize: (qsize) frameSize
		 count:	       (qsize) frameCount
		{
		if (_tapeRecorder.buffer != NULL) free(_tapeRecorder.buffer);
		_tapeRecorder.buffer = malloc(sizeof(quint8) * frameSize * frameCount);

		_tapeRecorder.frame_count = frameCount;
		_tapeRecorder.frame_size = frameSize;
		}


	- (void) addOutput: (id)  output
		 action:    (SEL) action
		{
		_tapeRecorder.output = output;
		_tapeRecorder.output_method = [output methodForSelector: _tapeRecorder.output_selector = action];
		}


	- (void) removeOutput: (id) output
		{
		}


#	pragma mark - IBAction


	- (IBAction) eject: (id) sender
		{
		if (_tapeRecorder.samples)
			{
			[tapeView setHidden: YES];
			free(_tapeRecorder.samples);
			_tapeRecorder.samples = NULL;
			[_filePath release];
			_filePath = nil;
			}

		else	{
			NSOpenPanel *panel = [NSOpenPanel openPanel];

			panel.allowedFileTypes = [NSArray arrayWithObjects: @"wav", @"raw", @"au", @"voc", nil];

			if ([panel runModal] == NSFileHandlingPanelOKButton)
				[self loadTapeAtPath: panel.URL.path];
			}
		}


	- (IBAction) play: (id) sender
		{
		if (_playing)
			{
			[self performSelector: @selector(stopPlayThread) onThread: _thread withObject: nil waitUntilDone: YES];
			_playing = NO;
			}

		else	{
			_playing = YES;
			_tapeRecorder.frame_index = 0;
			//pthread_create(&_thread, NULL, (void *(*)(void *))tape_recorder_play, &_tapeRecorder);
			[NSThread detachNewThreadSelector: @selector(startPlayThread) toTarget: self withObject: nil];
			}
		}


	- (IBAction) record: (id) sender
		{
		}


	- (IBAction) rewindToPreviousBlock: (id) sender
		{
		}


	- (IBAction) rewind: (id) sender
		{
		}


	- (IBAction) windForward: (id) sender
		{
		}


	- (IBAction) windForwardToNextBlock: (id) sender
		{
		}


@end
