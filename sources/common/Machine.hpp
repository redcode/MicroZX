/*     _________  ___
 _____ \_   /\  \/  / common/Machine.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef __mZX_common_Machine_H
#define __mZX_common_Machine_H

#include <Z/classes/buffering/TripleBuffer.hpp>
#include <pthread.h>
#include "ZX Spectrum.h"
#include "MachineABI.h"
#include <Z/inspection/OS.h>

class Machine {

	public:
	pthread_t	    thread;
	ZXSpectrum*	    context;
	MachineABI*	    abi;
	ZKit::TripleBuffer* video_output;
	ZRingBuffer*	    audio_output;

#	if Z_OS == Z_OS_MAC_OS_X
		ZRingBuffer* audio_input;
#	endif

	ZKit::UInt8	       audio_frame[882];
	ZKit::TripleBuffer*    keyboard_input;
	volatile ZKit::Boolean must_stop;

	struct {ZKit::Boolean power :1;
		ZKit::Boolean pause :1;
	} flags;

	Machine(MachineABI*	    abi,
		ZKit::TripleBuffer* video_output,
		ZRingBuffer*	    audio_output);

	~Machine();

	void run_one_frame();
	void power(ZKit::Boolean  state);
	void pause(ZKit::Boolean  state);
	void reset();
	void machine_set_audio_input_buffer(ZRingBuffer* audio_input);
};

#endif /* __mZX_common_Machine_H */
