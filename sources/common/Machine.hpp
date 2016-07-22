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
	volatile Zeta::Boolean _must_stop;
	Zeta::TripleBuffer*    _video_output;
	Zeta::RingBuffer*      _audio_output;
	Zeta::TripleBuffer*    _keyboard_input;

#	if Z_OS != Z_OS_LINUX
		Zeta::RingBuffer* _audio_input;
#	endif

	public:
	ZXSpectrum*	    context;
	MachineABI*	    abi;

	struct {Zeta::Boolean power :1;
		Zeta::Boolean pause :1;
	} flags;

	Machine(MachineABI*	    abi,
		Zeta::TripleBuffer* video_output,
		Zeta::RingBuffer*   audio_output,
		Zeta::TripleBuffer* keyboard_input);

	~Machine();

	void run_one_frame();
	void power(Zeta::Boolean state);
	void pause(Zeta::Boolean state);
	void reset();
	void set_audio_input(Zeta::RingBuffer *audio_input);
	void write_memory(Zeta::UInt16 base_address, void *data, Zeta::Size data_size);

	private:
	void main();
	void start();
	void stop();
};

#endif /* __mZX_common_Machine_H */
