/* ACME - CoreAudioOutput.h
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

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
