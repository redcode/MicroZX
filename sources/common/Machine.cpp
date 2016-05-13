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

using namespace ZKit;

/*--------------------------------.
| Emulation thread main function. |
'--------------------------------*/
void Machine::main()
	{
	UInt64	frames_per_second = 50;
	UInt64	frame_ticks	  = 1000000000 / frames_per_second;
	UInt64	next_frame_tick   = z_ticks();
	UInt64	delta;
	UInt	maximum_frameskip = 5;
	UInt	loops;
	void*	buffer;
	UInt64* keyboard;

	while (!_must_stop)
		{
		loops = 0;

		do abi->run_1_frame(context);
		while ((next_frame_tick += frame_ticks) < z_ticks() && ++loops < maximum_frameskip);

		//-----------------.
		// Produce output. |
		//-----------------'
		if ((buffer = _audio_output->try_produce()) != NULL)
			context->audio_output_buffer = (Int16 *)buffer;

		context->video_output_buffer = _video_output->produce();

		//----------------.
		// Consume input. |
		//----------------'
		if ((keyboard = (UInt64 *)_keyboard_input->consume()) != NULL)
			{context->state.keyboard.value_uint64 = *keyboard;}

#		if Z_OS != Z_OS_LINUX
		if (_audio_input)
			{
			while (!(buffer = _audio_input->try_consume()))
				{
				//printf("skip");
				next_frame_tick += frame_ticks / 4;
				z_wait(frame_ticks / 4);
				}

			context->audio_input_buffer = (UInt8 *)buffer;
			}
#		endif

		//----------------------------------------.
		// Schedule next iteration time and wait. |
		//----------------------------------------'
		if ((delta = next_frame_tick - z_ticks()) <= frame_ticks) z_wait(delta);
		//else printf("delta => %lu, next => %lu\n", delta, next_frame_tick);
		}
	}


void Machine::start()
	{
	_must_stop = FALSE;
	_thread = std::thread(&Machine::main, this);
	}


void Machine::stop()
	{
	_must_stop = TRUE;
	_thread.join();
	}


Machine::Machine(MachineABI *abi, TripleBuffer *video_output, RingBuffer *audio_output, TripleBuffer *keyboard_input)
: _video_output(video_output), _audio_output(audio_output), abi(abi), _keyboard_input(keyboard_input)
	{
#	if Z_OS != Z_OS_LINUX
	_audio_input = NULL;
#	endif
	flags.power  = OFF;
	flags.pause  = OFF;

	/*--------------------------------------.
	| Create the machine and its components |
	'--------------------------------------*/
	context			     = (ZXSpectrum *)malloc(abi->context_size);
	context->cpu_abi.run	     = (ZEmulatorRun  )z80_run;
	context->cpu_abi.irq	     = (ZContextSwitch)z80_int;
	context->cpu_abi.reset	     = (ZContextDo    )z80_reset;
	context->cpu_abi.power	     = (ZEmulatorPower)z80_power;
	context->cpu		     = (Z80 *)malloc(sizeof(Z80));
	context->cpu_cycles	     = &context->cpu->cycles;
	context->memory		     = (UInt8 *)calloc(1, abi->memory_size);
	context->video_output_buffer = video_output->production_buffer();
	context->audio_output_buffer = (Int16 *)audio_output->production_buffer();
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
			start();
			}

		else	{
			Size rom_count = abi->rom_count, index = 0;
			Size offset = 0;
			ROM *rom;

			if (!flags.pause) stop();
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
			stop();
			}

		else	{
			start();
			flags.pause = OFF;
			}
		}
	}


void Machine::reset()
	{
	if (flags.power)
		{
		if (!flags.pause) stop();
		abi->reset(context);
		start();
		flags.pause = OFF;
		}
	}


void Machine::set_audio_input(RingBuffer *audio_input)
	{
#	if Z_OS != Z_OS_LINUX
	_audio_input = audio_input;
#	endif
	}


void Machine::write_memory(ZKit::UInt16 base_address, void *data, ZKit::Size data_size)
	{memcpy(context->memory + base_address, data, data_size);}


/* Machine.c EOF */
