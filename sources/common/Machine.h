/*	_________  ___
  _____ \_   /\  \/  /	μZX - common/Machine.h
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#ifndef __mZX_common_Machine_H
#define __mZX_common_Machine_H

#include <Q/types/base.h>
#include <Q/types/buffering.h>
#include <pthread.h>
#include "ZX Spectrum.h"
#include "MachineABI.h"

typedef struct {
	pthread_t      thread;
	ZXSpectrum*    context;
	MachineABI*    abi;
	QTripleBuffer* video_output_buffer;
	QRingBuffer*   audio_output_buffer;
	QRingBuffer*   audio_input_buffer;
	qboolean       must_stop;

	struct {quint8 running :1;
	} flags;
} Machine;

void machine_initialize		    (Machine*	    object,
				     MachineABI*    abi,
				     QTripleBuffer* video_output_buffer,
				     QRingBuffer*   audio_output_buffer);

void machine_finalize		    (Machine*	    object);

void machine_run		    (Machine*	    object);

void machine_run_one_frame	    (Machine*	    object);

void machine_power		    (Machine*	    object,
				     qboolean	    state);

void machine_start		    (Machine*	    object);

void machine_stop		    (Machine*	    object);

void machine_reset		    (Machine*	    object);

void machine_keyboard_input	    (Machine*	    object,
				     quint16	    key_code,
				     qboolean	    key_state);

void machine_set_audio_input_buffer (Machine*	    object,
				     QRingBuffer*   audio_input_buffer);

#endif /* __mZX_common_Machine_H */
