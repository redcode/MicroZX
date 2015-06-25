/*	_________  ___
  _____ \_   /\  \/  /	μZX - common/MainController.h
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#include "Machine.h"
#include <Q/functions/buffering/QTripleBuffer.h>
#include <Q/functions/buffering/QRingBuffer.h>
#include "system.h"
#include "Z80.h"


static void emulate(Machine *object)
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
		if ((delta = next_frame_tick - q_ticks()) <= frame_ticks)
			q_wait(delta);

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

	object->abi	       = abi;


	/*-----------------------------------.
	| Create needed Input/Output objects |
	'-----------------------------------*/
	_videoOutput = [[GLOutputView alloc] initWithFrame:
		NSMakeRect(0.0, 0.0, (CGFloat)Q_ZX_SPECTRUM_SCREEN_WIDTH, (CGFloat)Q_ZX_SPECTRUM_SCREEN_HEIGHT)];

	[_videoOutput
		setResolution: q_2d_value(SIZE)(Q_ZX_SPECTRUM_SCREEN_WIDTH, Q_ZX_SPECTRUM_SCREEN_HEIGHT)
		format:	       0];

			_videoOutput.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
			_audioOutput = [[CoreAudioOutput alloc] init];
			//_audioOutput = [[ALOutputPlayer alloc] init];

			_videoOutputBuffer = _videoOutput.buffer;
			_audioOutputBuffer = _audioOutput.buffer;

			_machine->video_output_buffer = q_triple_buffer_production_buffer(_videoOutputBuffer);
			_machine->audio_output_buffer = q_ring_buffer_production_buffer(_audioOutputBuffer);

			_keyboardBuffer = malloc(sizeof(QTripleBuffer));
			q_triple_buffer_initialize(_keyboardBuffer, malloc(sizeof(quint64) * 3), sizeof(quint64));
			_keyboard = q_triple_buffer_production_buffer(_keyboardBuffer);
			memset(_keyboardBuffer->buffers[0], 0xFF, sizeof(quint64) * 3);

			/*-----------------.
			| Load needed ROMs |
			'-----------------*/
			qsize index = machineABI->rom_count;
			NSBundle *bundle = [NSBundle mainBundle];
			ROM *rom;

			while (index)
				{
				rom = &_machineABI->roms[--index];

				NSData *ROM = [NSData dataWithContentsOfFile: [bundle
					pathForResource: [NSString stringWithUTF8String: rom->file_name]
					ofType:		 @"rom"
					inDirectory:	 @"ROMs"]];

				memcpy(_machine->memory + rom->base_address, [ROM bytes], rom->size);
				}

			machineABI->initialize(_machine);

			_keyboardState.value_uint64 = Q_UINT64(0xFFFFFFFFFFFFFFFF);

			/*NSData *data = [NSData dataWithContentsOfFile: @"/Users/manuel/Desktop/Batman.sna"];
			QSNAv48K *sna = (QSNAv48K *)[data bytes];

			sna_v48k_decode(sna, &_machine->state, &_machine->cpu->state, _machine->memory);*/
			//_machine->border_color = Q_RGBA32(FF, 00, 00, 00);*/

			_attachInputBuffer = NO;

			[_audioOutput start];

			pthread_attr_t threadAttributes;
			pthread_attr_init(&threadAttributes);
			pthread_create(&_thread, &threadAttributes, (void *(*)(void *))EmulationMain, self);
			pthread_attr_destroy(&threadAttributes);
	}


void machine_finalize(Machine *object)
	{
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
	}


void machine_stop(Machine *object)
	{
	}


void machine_reset(Machine *object)
	{
	}


void machine_keyboard_input(Machine *object, quint16  key_code, qboolean key_state)
	{
	}


/* Machine.c EOF */
