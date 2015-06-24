/* ACME - ACMEMachine.c
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#include "ACMEMachine.h"
#include "system.h"
#include <Q/functions/buffering/QTripleBuffer.h>
#include <Q/functions/buffering/QRingBuffer.h>
#include <Q/ABIs/QMachineEmulationABI.h>
//#include <Q/ABIs/QCPUEmulationABI.h>
#include <Q/functions/base/block.h>


static void triple_buffer_input(ACMEIOBuffer *object)
	{*(object->frame) = q_triple_buffer_consume(object->buffer);}


static void triple_buffer_output(ACMEIOBuffer *object)
	{*(object->frame) = q_triple_buffer_produce(object->buffer);}


typedef pthread_t QSystemThread;

typedef struct {
	QSystemThread thread;
	volatile qboolean must_stop;
} QMachineEmulator;

void *q_machine_emulator_main(QMachineEmulator *object)
	{

	return NULL;
	}


void *acme_machine_run(ACMEMachine *object)
	{
	quint64       next_frame_tick = q_ticks();
	quint64       frame_ticks     = 1000000000 / object->frames_per_second;
	quint64       delta;
	quint	      loops;
	QDo	      run_one_frame   = object->abi->run_one_frame;
	void*	      machine	      = object->context;
	ACMEIOBuffer* io;

	while (!object->must_stop)
		{
		/*-------------------------------------------------------------------------.
		| Execute frames until idle time is found or maximum frameskip is overrun. |
		'-------------------------------------------------------------------------*/
		loops = 0;
		do run_one_frame(machine);
		while ((next_frame_tick += frame_ticks) < q_ticks() && ++loops < object->maximum_frame_skip);

		/*------------------------------------.
		| Produce outputs and consume inputs. |
		'------------------------------------*/
		for (io = object->outputs; io != NULL; io = io->next) io->step(io);
		for (io = object->inputs;  io != NULL; io = io->next) io->step(io);

		/*-----------------------------------------.
		| Schedule time until next frame and wait. |
		'-----------------------------------------*/
		if ((delta = next_frame_tick - q_ticks()) <= frame_ticks) q_wait(delta);
		}

	return NULL;
	}


void acme_machine_reset(ACMEMachine *object)
	{object->reset(object + 1);}


void acme_machine_set_power(ACMEMachine *object, qboolean state)
	{object->power(object + 1, state);}


/* ACMEMachine.c EOF */
