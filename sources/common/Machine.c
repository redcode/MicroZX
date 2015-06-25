/*	_________  ___
  _____ \_   /\  \/  /	common/Machine.c
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#include "Machine.h"
#include "system.h"
#include "Z80.h"
#include <Q/functions/buffering/QTripleBuffer.h>
#include <Q/functions/buffering/QRingBuffer.h>
#include <stdlib.h>


/*---------------------.
| Emulation main loop. |
'---------------------*/
static void *emulate(Machine *object)
	{
	quint64 frames_per_second = 50;
	quint64 frame_ticks	  = 1000000000 / frames_per_second;
	quint64 next_frame_tick   = q_ticks();
	quint64 delta;
	quint	maximum_frameskip = 5;
	quint	loops;
	void*	audio_output_buffer;
	quint64* keyboard;

	while (!object->must_stop)
		{
		loops = 0;

		do object->abi->run_one_frame(object->context);
		while ((next_frame_tick += frame_ticks) < q_ticks() && ++loops < maximum_frameskip);

		if ((audio_output_buffer = q_ring_buffer_try_produce(object->audio_output_buffer)) != NULL)
			object->context->audio_output_buffer = audio_output_buffer;

		object->context->video_output_buffer = q_triple_buffer_produce(object->video_output_buffer);

		//----------------.
		// Consume input. |
		//----------------'
		//if ((keyboard = q_triple_buffer_consume(controller->_keyboardBuffer)) != NULL)
		//	{controller->_machine->state.keyboard.value_uint64 = *keyboard;}

		/*if (object->audio_input_buffer != NULL)
			{
			object->context->audio_input_buffer = ring_buffer_try_read(controller->_audioInputRing, controller->_audioInputBuffer)
				? controller->_audioInputBuffer : NULL;
			}*/

		//----------------------------------------.
		// Schedule next iteration time and wait. |
		//----------------------------------------'
		if ((delta = next_frame_tick - q_ticks()) <= frame_ticks) q_wait(delta);
		//else printf("delta => %lu, next => %lu\n", delta, next_frame_tick);
		}

	return NULL;
	}


void machine_initialize(
	Machine*       object,
	MachineABI*    abi,
	QTripleBuffer* video_output_buffer,
	QRingBuffer*   audio_output_buffer
)
	{
	object->abi		    = abi;
	object->video_output_buffer = video_output_buffer;
	object->audio_output_buffer = audio_output_buffer;

	/*--------------------------------------.
	| Create the machine and its components |
	'--------------------------------------*/
	ZXSpectrum *context = object->context = malloc(abi->context_size);

	context->cpu_abi.run   = (void *)z80_run;
	context->cpu_abi.irq   = (void *)z80_irq;
	context->cpu_abi.reset = (void *)z80_reset;
	context->cpu_abi.power = (void *)z80_power;
	context->cpu	       = malloc(sizeof(Z80));
	context->cpu_cycles    = &context->cpu->cycles;
	context->memory	       = calloc(1, abi->memory_size);

	context->video_output_buffer = q_triple_buffer_production_buffer(video_output_buffer);
	context->audio_output_buffer = q_ring_buffer_production_buffer	(audio_output_buffer);
	abi->initialize(context);
	}


void machine_finalize(Machine *object)
	{
	free(object->context->memory);
	free(object->context->cpu);
	}


void machine_run(Machine *object)
	{
	}


void machine_run_one_frame(Machine *object)
	{
	}


void machine_power(Machine *object, qboolean state)
	{
	}


void machine_start(Machine *object)
	{
	if (!object->flags.running)
		{
		object->flags.running = TRUE;
		object->must_stop = FALSE;

		pthread_attr_t attributes;
		pthread_attr_init(&attributes);
		pthread_create(&object->thread, &attributes, (void *(*)(void *))emulate, object);
		pthread_attr_destroy(&attributes);
		}
	}


void machine_stop(Machine *object)
	{
	if (object->flags.running)
		{
		object->must_stop = TRUE;
		pthread_join(object->thread, NULL);
		object->flags.running = FALSE;
		}
	}


void machine_reset(Machine *object)
	{
	}


void machine_keyboard_input(Machine *object, quint16  key_code, qboolean key_state)
	{
	}


/* Machine.c EOF */
