/*     _________  ___
 _____ \_   /\  \/  / OS X/CoreAudioOutputPlayer.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#import <AudioToolbox/AudioToolbox.h>
#import <Z/classes/buffering/RingBuffer.hpp>

class CoreAudioOutputPlayer {
	private:
	AudioComponentInstance _audio_unit;
	Zeta::RingBuffer       _buffer;
	zuint		       _buffer_frame_count;
	double		       _sample_rate;

	public:
	Zeta::Boolean	       playing;

	CoreAudioOutputPlayer();
	~CoreAudioOutputPlayer();
	Zeta::RingBuffer *buffer() {return &_buffer;}
	void start();
	void stop();
};
