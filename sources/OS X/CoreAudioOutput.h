/*     _________  ___
 _____ \_   /\  \/  / OS X/CoreAudioOutput.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#import <Foundation/Foundation.h>
#import <CoreAudio/CoreAudio.h>
#import <AudioToolbox/AudioToolbox.h>
#import <Q/types/buffering.h>
#import "RingBuffer.h"

@interface CoreAudioOutput : NSObject {

	AudioComponentInstance	_audioUnit;

	@public
	QRingBuffer		_buffer;
	void*			_silence;
	quint			_bufferFrameCount;
	quint			_bufferFillCount;
	double			_sampleRate;
	BOOL			_playing;
}

	@property (nonatomic, readonly) QRingBuffer* buffer;

	- (void) start;
	- (void) stop;

@end
