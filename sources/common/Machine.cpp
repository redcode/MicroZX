/*     _________  ___
 _____ \_   /\  \/  / common/Machine.c
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Machine.hpp"
#include "system.h"
#include "Z80.h"
#include <Z/functions/buffering/ZTripleBuffer.h>
#include <Z/functions/buffering/ZRingBuffer.h>
#include <Z/types/base.hpp>

using namespace ZKit;

/*--------------------------------.
| Emulation thread main function. |
'--------------------------------*/
Z_PRIVATE void *emulate(Machine *object)
	{
	UInt64	frames_per_second = 50;
	UInt64	frame_ticks	  = 1000000000 / frames_per_second;
	UInt64	next_frame_tick   = z_ticks();
	UInt64	delta;
	UInt	maximum_frameskip = 5;
	UInt	loops;
	void*	buffer;
	UInt64* keyboard;

	while (!object->must_stop)
		{
		loops = 0;

		do object->abi->run_one_frame(object->context);
		while ((next_frame_tick += frame_ticks) < z_ticks() && ++loops < maximum_frameskip);

		//-----------------.
		// Produce output. |
		//-----------------'
		if ((buffer = z_ring_buffer_try_produce(object->audio_output)) != NULL)
			object->context->audio_output_buffer = (Int16 *)buffer;

		object->context->video_output_buffer = object->video_output->produce();

		//----------------.
		// Consume input. |
		//----------------'
		if ((keyboard = (UInt64 *)object->keyboard_input->consume()) != NULL)
			{object->context->state.keyboard.value_uint64 = *keyboard;}

#		if Z_OS != Z_OS_LINUX
		if (object->audio_input != NULL)
			{
			while ((buffer = z_ring_buffer_try_consume(object->audio_input)) == NULL)
				{
				//printf("skip");
				next_frame_tick += frame_ticks / 4;
				z_wait(frame_ticks / 4);
				}

			object->context->audio_input_buffer = (UInt8 *)buffer;
			}
#		endif

		//----------------------------------------.
		// Schedule next iteration time and wait. |
		//----------------------------------------'
		if ((delta = next_frame_tick - z_ticks()) <= frame_ticks) z_wait(delta);
		//else printf("delta => %lu, next => %lu\n", delta, next_frame_tick);
		}

	return NULL;
	}


Z_PRIVATE void start(Machine *object)
	{
	object->must_stop = FALSE;
	pthread_attr_t attributes;
	pthread_attr_init(&attributes);
	pthread_create(&object->thread, &attributes, (void *(*)(void *))emulate, object);
	pthread_attr_destroy(&attributes);
	}


Z_PRIVATE void stop(Machine *object)
	{
	object->must_stop = TRUE;
	pthread_join(object->thread, NULL);
	}


Machine::Machine(MachineABI *abi, ZKit::TripleBuffer *video_output, ZRingBuffer *audio_output)
	{
	this->abi	   = abi;
	this->video_output = video_output;
	this->audio_output = audio_output;
#	if Z_OS != Z_OS_LINUX
	this->audio_input  = NULL;
#	endif
	flags.power  = OFF;
	flags.pause  = OFF;

	/*--------------------------------------.
	| Create the machine and its components |
	'--------------------------------------*/
	context			     = (ZXSpectrum *)malloc(abi->context_size);
	context->cpu_abi.run	     = (ZEmulatorRun  )z80_run;
	context->cpu_abi.irq	     = (ZContextSwitch)z80_irq;
	context->cpu_abi.reset	     = (ZContextDo    )z80_reset;
	context->cpu_abi.power	     = (ZEmulatorPower)z80_power;
	context->cpu		     = (Z80 *)malloc(sizeof(Z80));
	context->cpu_cycles	     = &context->cpu->cycles;
	context->memory		     = (UInt8 *)calloc(1, abi->memory_size);
	context->video_output_buffer = video_output->production_buffer();
	context->audio_output_buffer = (Int16 *)z_ring_buffer_production_buffer(audio_output);
	abi->initialize(context);
	}


Machine::~Machine()
	{
	free(context->memory);
	free(context->cpu);
	}


void machine_run_one_frame(Machine *object)
	{
	}


void Machine::power(Boolean state)
	{
	if (state != flags.power)
		{
		if (state)
			{
			flags.power = ON;
			flags.pause = OFF;
			abi->power(context, ON);
			start(this);
			}

		else	{
			Size rom_count = abi->rom_count, index = 0;
			Size offset = 0;
			ROM *rom;

			if (!flags.pause) stop(this);
			abi->power(context, OFF);

			for (; index < rom_count; index++)
				{
				rom = &abi->roms[index];
				memset(context->memory + offset, 0, rom->base_address - offset);
				offset = rom->base_address + rom->size;
				}

			memset(context->memory + offset, 0, abi->memory_size - offset);
			flags.power = OFF;
			flags.pause = OFF;
			}
		}
	}


void Machine::pause(Boolean state)
	{
	if (flags.power && state != flags.pause)
		{
		if (state)
			{
			flags.pause = ON;
			stop(this);
			}

		else	{
			start(this);
			flags.pause = OFF;
			}
		}
	}


void Machine::reset()
	{
	if (flags.power)
		{
		if (!flags.pause) stop(this);
		abi->reset(context);
		start(this);
		flags.pause = OFF;
		}
	}


/* Machine.c EOF */
