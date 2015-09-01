/*     _________  ___
 _____ \_   /\  \/  / common/Machine.h
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef __mZX_common_Machine_H
#define __mZX_common_Machine_H

#include <Z/types/base.h>
#include <Z/types/buffering.h>
#include <pthread.h>
#include "ZX Spectrum.h"
#include "MachineABI.h"

#include <Z/inspection/OS.h>

typedef struct {
	pthread_t      thread;
	ZXSpectrum*    context;
	MachineABI*    abi;
	ZTripleBuffer* video_output;
	ZRingBuffer*   audio_output;

#	if Q_OS == Q_OS_MAC_OS_X
		ZRingBuffer* audio_input;
#	endif

	zuint8		  audio_frame[882];
	ZTripleBuffer*	  keyboard_input;
	volatile zboolean must_stop;

	struct {zuint8 power :1;
		zuint8 pause :1;
	} flags;
} Machine;

Z_C_SYMBOLS_BEGIN

void machine_initialize		    (Machine*	    object,
				     MachineABI*    abi,
				     ZTripleBuffer* video_output,
				     ZRingBuffer*   audio_output);

void machine_finalize		    (Machine*	    object);

void machine_run_one_frame	    (Machine*	    object);

void machine_power		    (Machine*	    object,
				     zboolean	    state);

void machine_pause		    (Machine*	    object,
				     zboolean	    state);

void machine_reset		    (Machine*	    object);

void machine_keyboard_input	    (Machine*	    object,
				     zuint16	    key_code,
				     zboolean	    key_state);

void machine_set_audio_input_buffer (Machine*	    object,
				     ZRingBuffer*   audio_input);

Z_C_SYMBOLS_END

#endif /* __mZX_common_Machine_H */
