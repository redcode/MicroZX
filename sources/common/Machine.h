/*	_________  ___
  _____ \_   /\  \/  /	μZX - common/Machine.h
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#ifndef __mZX_common_Machine_H
#define __mZX_common_Machine_H

#include <Q/types/base.h>
#include <pthread.h>

typedef struct {
	pthread_t thread;
} Machine;


void machine_initialize	    (Machine *object);

void machine_finalize	    (Machine *object);

void machine_run	    (Machine *object);

void machine_run_one_frame  (Machine *object);

void machine_power	    (Machine* object,
			     qboolean state);

void machine_start	    (Machine* object);

void machine_stop	    (Machine* object);

void machine_reset	    (Machine* object);

void machine_keyboard_input (Machine* object,
			     quint16  key_code,
			     qboolean key_state);

#endif /* __mZX_common_Machine_H */
