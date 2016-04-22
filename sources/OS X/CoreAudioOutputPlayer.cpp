/*     _________  ___
 _____ \_   /\  \/  / OS X/CoreAudioOutputPlayer.m
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#import "CoreAudioOutputPlayer.hpp"
#import <AudioToolbox/AudioToolbox.h>
#import <Z/types/base.h>
#import "system.h"

using namespace ZKit;


static OSStatus FillBuffer(
	RingBuffer*		    buffer,
	AudioUnitRenderActionFlags* ioActionFlags,
	const AudioTimeStamp*	    inTimeStamp,
	UInt32 			    inBusNumber,
	UInt32 			    inNumberFrames,
	AudioBufferList*	    ioData
)
	{
	if (buffer->fill_count < 2)
		{
		ioData->mBuffers[0].mData = buffer->consumption_buffer();
		return noErr;
		}

	while (buffer->fill_count > 3) buffer->try_consume();
	ioData->mBuffers[0].mData = buffer->consumption_buffer();
	buffer->try_consume();
	return noErr;
	}


CoreAudioOutputPlayer::CoreAudioOutputPlayer()
: _sample_rate(44100), _buffer_frame_count(2)
	{
	void *buffer = calloc(1, Z_INT16_SIZE * 882 * 4);
	_buffer.initialize(buffer, Z_INT16_SIZE * 882, 4);

	//---------------------------------------------------------------------------.
	// Configure the search parameters to find the default playback output unit. |
	//---------------------------------------------------------------------------'
	AudioComponentDescription output_description = {
		.componentType		= kAudioUnitType_Output,
		.componentSubType	= kAudioUnitSubType_DefaultOutput,
		.componentManufacturer	= kAudioUnitManufacturer_Apple,
		.componentFlags		= 0,
		.componentFlagsMask	= 0
	};

	//---------------------------------------.
	// Get the default playback output unit. |
	//---------------------------------------'
	AudioComponent output = AudioComponentFindNext(NULL, &output_description);
	//NSAssert(output, @"Can't find default output");

	//------------------------------------------------------------.
	// Create a new unit based on this that we'll use for output. |
	//------------------------------------------------------------'
	OSErr error = AudioComponentInstanceNew(output, &_audio_unit);
	//NSAssert1(_toneUnit, @"Error creating unit: %hd", error);

	// Set our tone rendering function on the unit
	AURenderCallbackStruct input;
	input.inputProc = (decltype(input.inputProc))FillBuffer;
	input.inputProcRefCon = &_buffer;

	error = AudioUnitSetProperty
		(_audio_unit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input,
		 0, &input, sizeof(input));

	//NSAssert1(error == noErr, @"Error setting callback: %hd", error);

	UInt32 frames = 882;
	// Set the max frames

	error = AudioUnitSetProperty
		(_audio_unit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global,
		 0, &frames, sizeof(frames));

	//NSAssert1(error == noErr, @"Error setting maximum frames per slice: %hd", error);

	// Set the buffer size

	error = AudioUnitSetProperty
		(_audio_unit, kAudioDevicePropertyBufferFrameSize, kAudioUnitScope_Global,
		 0, &frames, sizeof(frames));

	//NSAssert1(error == noErr, @"Error setting buffer frame size: %hd", error);

	//----------------------------------------------------.
	// Set the format to 16-bit little-endian lineal PCM. |
	//----------------------------------------------------'
	AudioStreamBasicDescription streamFormat = {
		.mSampleRate       = _sample_rate,
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
		(_audio_unit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input,
		 0, &streamFormat, sizeof(AudioStreamBasicDescription));

	//NSAssert1(error == noErr, @"Error setting stream format: %hd", error);

	error = AudioUnitInitialize(_audio_unit);
	//NSAssert1(error == noErr, @"Error initializing unit: %hd", error);
	}


CoreAudioOutputPlayer::~CoreAudioOutputPlayer()
	{
	playing = false;
	AudioOutputUnitStop(_audio_unit);
	AudioUnitUninitialize(_audio_unit);
	}


void CoreAudioOutputPlayer::start()
	{
	playing = true;
	OSErr error = AudioOutputUnitStart(_audio_unit);

	//NSAssert1(error == noErr, @"Error starting unit: %hd", error);
	}


void CoreAudioOutputPlayer::stop()
	{
	AudioOutputUnitStop(_audio_unit);
	}
