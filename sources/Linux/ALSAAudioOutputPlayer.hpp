/*     _________  ___
 _____ \_   /\  \/  / Linux/ALSAAudioOutputPlayer.hpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef __mZX_Linux_ALSAAudioOutputPlayer_HPP
#define __mZX_Linux_ALSAAudioOutputPlayer_HPP

#include <Z/classes/buffering/RingBuffer.hpp>
#include <alsa/asoundlib.h>
#include <thread>

#if Z_CPP < Z_CPP11
#	error "C++11 is needed."
#endif

class ALSAAudioOutputPlayer {
	private:
	std::thread	  _thread;
	snd_pcm_t*	  _device;
	ZKit::RingBuffer  _buffer;

	public:
	volatile zboolean must_stop;
	zboolean	  playing;

	ALSAAudioOutputPlayer();
	~ALSAAudioOutputPlayer();
	ZKit::RingBuffer *buffer() {return &_buffer;}
	void start();
	void stop();

	private:
	void main();
};

#endif // __mZX_Linux_ALSAAudioOutputPlayer_HPP
