/* ACME - CoreAudioOutput.m
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#import "CoreAudioOutput.h"
#import <AudioToolbox/AudioToolbox.h>
#import <Q/types/base.h>
#import <Q/functions/buffering/QRingBuffer.h>
#import "system.h"


static OSStatus FillBuffer(
	CoreAudioOutput*	    audioOutput,
	AudioUnitRenderActionFlags* ioActionFlags,
	const AudioTimeStamp*	    inTimeStamp,
	UInt32 			    inBusNumber,
	UInt32 			    inNumberFrames,
	AudioBufferList*	    ioData
)
	{
	if (audioOutput->_buffer.fill_count < 2)
		{
		//printf("m");
		ioData->mBuffers[0].mData = q_ring_buffer_consumption_buffer(&audioOutput->_buffer);
		return noErr;
		}

	while (audioOutput->_buffer.fill_count > 3)
		{
		//printf("M");
		q_ring_buffer_try_consume(&audioOutput->_buffer);
		}

	ioData->mBuffers[0].mData = q_ring_buffer_consumption_buffer(&audioOutput->_buffer);
	q_ring_buffer_try_consume(&audioOutput->_buffer);
	return noErr;
	}


@implementation CoreAudioOutput


	- (id) init
		{
		if ((self = [super init]))
			{
			_sampleRate = 44100;
			_bufferFillCount = 0;
			_bufferFrameCount = 2;

			void *buffer = malloc(sizeof(qint16) * 882 * 4);
			q_ring_buffer_initialize(&_buffer, buffer, sizeof(qint16) * 882, 4);
			memset(buffer, 0, sizeof(qint16) * 882 * 4);

			//---------------------------------------------------------------------------.
			// Configure the search parameters to find the default playback output unit. |
			//---------------------------------------------------------------------------'
			AudioComponentDescription defaultOutputDescription;
			defaultOutputDescription.componentType		= kAudioUnitType_Output;
			defaultOutputDescription.componentSubType	= kAudioUnitSubType_DefaultOutput;
			defaultOutputDescription.componentManufacturer	= kAudioUnitManufacturer_Apple;
			defaultOutputDescription.componentFlags		= 0;
			defaultOutputDescription.componentFlagsMask	= 0;

			//---------------------------------------.
			// Get the default playback output unit. |
			//---------------------------------------'
			AudioComponent defaultOutput = AudioComponentFindNext(NULL, &defaultOutputDescription);
			NSAssert(defaultOutput, @"Can't find default output");

			//------------------------------------------------------------.
			// Create a new unit based on this that we'll use for output. |
			//------------------------------------------------------------'
			OSErr error = AudioComponentInstanceNew(defaultOutput, &_audioUnit);
		//	NSAssert1(_toneUnit, @"Error creating unit: %hd", error);

			// Set our tone rendering function on the unit
			AURenderCallbackStruct input;
			input.inputProc = (void *)FillBuffer;
			input.inputProcRefCon = self;

			error = AudioUnitSetProperty
				(_audioUnit,
				 kAudioUnitProperty_SetRenderCallback, 
				 kAudioUnitScope_Input,
				 0,
				 &input,
				 sizeof(input));

			NSAssert1(error == noErr, @"Error setting callback: %hd", error);

/*			AudioUnitExternalBuffer externalBuffer;
			externalBuffer.buffer = (Byte *)_buffer;
			externalBuffer.size = sizeof(float) * 882;

			error = AudioUnitSetProperty
				(_toneUnit,
				 kAudioUnitProperty_SetExternalBuffer,
				 kAudioUnitScope_Input,
				 0,
				 &externalBuffer,
				 sizeof(AudioUnitExternalBuffer));

			NSAssert1(error == noErr, @"Error setting external buffer: %ld", error);*/

			UInt32 frames = 882;
			// Set the max frames

			error = AudioUnitSetProperty
				(_audioUnit,
				 kAudioUnitProperty_MaximumFramesPerSlice,
				 kAudioUnitScope_Global, 0,
				 &frames, sizeof(frames));

			NSAssert1(error == noErr, @"Error setting maximum frames per slice: %hd", error);

			// Set the buffer size

			error = AudioUnitSetProperty
				(_audioUnit,
				 kAudioDevicePropertyBufferFrameSize,
				 kAudioUnitScope_Global, 0,
				 &frames, sizeof(frames));

			NSAssert1(error == noErr, @"Error setting buffer frame size: %hd", error);

			//----------------------------------------------------.
			// Set the format to 16-bit little-endian lineal PCM. |
			//----------------------------------------------------'
			AudioStreamBasicDescription streamFormat = {
				.mSampleRate       = _sampleRate,
				.mFormatID	   = kAudioFormatLinearPCM,
				.mFormatFlags      = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked,
				.mBytesPerPacket   = 2,
				.mFramesPerPacket  = 1,
				.mBytesPerFrame    = 2,
				.mChannelsPerFrame = 1,
				.mBitsPerChannel   = 16,
				.mReserved	   = 0
			};

			error = AudioUnitSetProperty
				(_audioUnit,
				 kAudioUnitProperty_StreamFormat,
				 kAudioUnitScope_Input,
				 0,
				 &streamFormat,
				 sizeof(AudioStreamBasicDescription));

			NSAssert1(error == noErr, @"Error setting stream format: %hd", error);

			error = AudioUnitInitialize(_audioUnit);
			NSAssert1(error == noErr, @"Error initializing unit: %hd", error);
			}

		return self;
		}



	- (QRingBuffer *) buffer {return &_buffer;}


	- (void) dealloc
		{
		_playing = NO;
		AudioOutputUnitStop(_audioUnit);
		AudioUnitUninitialize(_audioUnit);
		[super dealloc];
		}


	- (void) start
		{
		// Start playback
		_playing = YES;
		OSErr error = AudioOutputUnitStart(_audioUnit);

		NSAssert1(error == noErr, @"Error starting unit: %hd", error);
		//NSLog(@"play: isMainThread => %s", [NSThread isMainThread] ? "YES" : "NO");
		}


	- (void) stop
		{
		AudioOutputUnitStop(_audioUnit);
		}


@end
