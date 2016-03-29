/*     _________  ___
 _____ \_   /\  \/  / OS X/CoreAudioOutputPlayer.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#import <Foundation/Foundation.h>
#import <CoreAudio/CoreAudio.h>
#import <AudioToolbox/AudioToolbox.h>
#import <Z/types/buffering.h>

@interface CoreAudioOutputPlayer : NSObject {

	AudioComponentInstance _audioUnit;
	ZRingBuffer	       _buffer;
	zuint		       _bufferFrameCount;
	double		       _sampleRate;
	BOOL		       _playing;
}
	@property (nonatomic, readonly) ZRingBuffer* buffer;

	- (void) start;
	- (void) stop;

@end
