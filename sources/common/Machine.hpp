/*     _________  ___
 _____ \_   /\  \/  / common/Machine.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef __mZX_common_Machine_H
#define __mZX_common_Machine_H

#include <Z/classes/buffering/TripleBuffer.hpp>
#include <Z/classes/buffering/RingBuffer.hpp>
#include "ZX Spectrum.h"
#include "MachineABI.h"
#include <Z/inspection/OS.h>
#include <thread>

class Machine {
	private:
	std::thread	       _thread;
	volatile ZKit::Boolean _must_stop;
	ZKit::TripleBuffer*    _video_output;
	ZKit::RingBuffer*      _audio_output;
	ZKit::TripleBuffer*    _keyboard_input;

#	if Z_OS == Z_OS_MAC_OS_X
		ZKit::RingBuffer* _audio_input;
#	endif

	public:
	ZXSpectrum*	    context;
	MachineABI*	    abi;

	struct {ZKit::Boolean power :1;
		ZKit::Boolean pause :1;
	} flags;

	Machine(MachineABI*	    abi,
		ZKit::TripleBuffer* video_output,
		ZKit::RingBuffer*   audio_output,
		ZKit::TripleBuffer* keyboard_input);

	~Machine();

	void run_one_frame();
	void power(ZKit::Boolean state);
	void pause(ZKit::Boolean state);
	void reset();
	void set_audio_input(ZKit::RingBuffer *audio_input);
	void write_memory(ZKit::UInt16 base_address, void *data, ZKit::Size data_size);

	private:
	void main();
	void start();
	void stop();
};

#endif /* __mZX_common_Machine_H */
