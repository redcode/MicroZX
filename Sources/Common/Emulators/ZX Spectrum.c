/* ZX Spectrum Emulator v1.0
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#include <Q/hardware/machine/platform/computer/ZX Spectrum.h>
#include <Q/hardware/machine/model/computer/ZX Spectrum/ZX Spectrum.h>
#include <Q/hardware/machine/model/computer/ZX Spectrum/ZX Spectrum +.h>
#include <Q/hardware/machine/model/computer/ZX Spectrum/ZX Spectrum + 128K.h>
#include <Q/hardware/machine/model/computer/ZX Spectrum/ZX Spectrum +2.h>
#include <Q/hardware/machine/model/computer/ZX Spectrum/ZX Spectrum +2A.h>
#include <Q/hardware/machine/model/computer/ZX Spectrum/ZX Spectrum +3.h>
#include <Q/hardware/machine/model/computer/ZX Spectrum/Inves Spectrum +.h>
#include <ACME/types/basics.h>

#define KB(amount) (1024 * amount)

/* MARK: - Types */

typedef struct {
	qsize top;
	qsize bottom;
} ScreenBorder;

typedef struct {
	qsize per_int;
	qsize per_scanline;
	qsize per_frame;
	qsize at_int;
	qsize at_visible_top_border;
	qsize at_paper_region;
	qsize at_bottom_border;
} Cycles;

typedef struct {
} Contention;

/* MARK: - Constants */

static const ScreenBorder zx_spectrum_screen_border = {
	Q_ZX_SPECTRUM_SCREEN_VISIBLE_TOP_BORDER_HEIGHT,
	Q_ZX_SPECTRUM_SCREEN_BOTTOM_BORDER_HEIGHT
};

static const Cycles zx_spectrum_cycles = {
	Q_ZX_SPECTRUM_CYCLES_PER_INT,
	Q_ZX_SPECTRUM_CYCLES_PER_SCANLINE,
	Q_ZX_SPECTRUM_CYCLES_PER_FRAME,
	Q_ZX_SPECTRUM_CYCLES_AT_INT,
	Q_ZX_SPECTRUM_CYCLES_AT_VISIBLE_TOP_BORDER,
	Q_ZX_SPECTRUM_CYCLES_AT_PAPER_REGION,
	Q_ZX_SPECTRUM_CYCLES_AT_BOTTOM_BORDER
};

static const Cycles zx_spectrum_plus_128k_cycles = {
	Q_ZX_SPECTRUM_PLUS_128K_CYCLES_PER_INT,
	Q_ZX_SPECTRUM_PLUS_128K_CYCLES_PER_SCANLINE,
	Q_ZX_SPECTRUM_PLUS_128K_CYCLES_PER_FRAME,
	Q_ZX_SPECTRUM_PLUS_128K_CYCLES_AT_INT,
	Q_ZX_SPECTRUM_PLUS_128K_CYCLES_AT_VISIBLE_TOP_BORDER,
	Q_ZX_SPECTRUM_PLUS_128K_CYCLES_AT_PAPER_REGION,
	Q_ZX_SPECTRUM_PLUS_128K_CYCLES_AT_BOTTOM_BORDER
};

static const Cycles zx_spectrum_plus_2a_cycles = {
};

static const Cycles pentagon_cycles = {
};

static const Cycles scorpion_cycles = {
};

static const Contention zx_spectrum_contention = {
};

static const Contention zx_spectrum_plus_128k_contention = {
};




#define ZX_SPECTRUM_VALUES				\
	quint8*			memory;			\
	void*			cpu;			\
	qsize*			cpu_cycles;		\
	void*			video_output_buffer;	\
	quint8*			audio_input_buffer;	\
	qint16*			audio_output_buffer;	\
							\
	struct {ACMERunCycles	run;			\
		ACMESwitch	power;			\
		ACMEPulse	reset;			\
		ACMESwitch	irq;			\
	} cpu_abi;					\
							\
	quint8			keyboard[8];		\
	quint32			border_color;		\
	qsize			frame_cycles;		\
	qsize			frames_since_flash;	\
	qint16			current_audio_sample;	\
	qsize			audio_sample_index;	\
	qsize			audio_input_base_index;	\
	qboolean		accurate;		\
	QZXSpectrumState	state;			\
	const ScreenBorder*	screen_border;		\
	const Cycles*		cycles;			\
	const Contention*	contention;		\
	quint8			port_fe;		\
	quint8			port_fe_update_cycle;	\
	quint8*			vram;			\

typedef struct {
	ZX_SPECTRUM_VALUES
} ZXSpectrum;

typedef struct {
	ZX_SPECTRUM_VALUES
	quint8*		memory_pages[4];
	quint8		port_7ffd;
	qboolean	disable_bank_switching;
	void*		psg;

	struct {
	} psg_abi;
} ZXSpectrum128K;

#define RAM_BANK(number) (object->memory + (1024 * 16 * 2) + (1024 * 16 * (number)))
#define ROM_BANK(number) (object->memory + (1024 * 16 * (number)))

#include <string.h>
#include <Q/macros/color.h>
#include <stdlib.h>

#define CPU_RUN(cycles)		object->cpu_abi.run(object->cpu, cycles)
#define CPU_INT(state)		object->cpu_abi.irq(object->cpu, state)
#define CPU_RESET		object->cpu_abi.reset(object->cpu)
#define CPU_POWER(state)	object->cpu_abi.power(object->cpu, state)
#define RGBA			Q_RGBA32
#define WAVE_HIGH		6550
#define WAVE_LOW		-6550

static const quint32 bright_colors[8] = {
	RGBA(00, 00, 00, 00),	/* Bright Black	  */
	RGBA(00, 00, FF, 00),	/* Bright Blue	  */
	RGBA(FF, 00, 00, 00),	/* Bright Red	  */
	RGBA(FF, 00, FF, 00),	/* Bright Magenta */
	RGBA(00, FF, 00, 00),	/* Bright Green	  */
	RGBA(00, FF, FF, 00),	/* Bright Cyan	  */
	RGBA(FF, FF, 00, 00),	/* Bright Yellow  */
	RGBA(FF, FF, FF, 00)	/* Bright White	  */
};

static const quint32 basic_colors[8] = {
	RGBA(00, 00, 00, 00),	/* Black	 */
	RGBA(00, 00, C0, 00),	/* Basic Blue	 */
	RGBA(C0, 00, 00, 00),	/* Basic Red	 */
	RGBA(C0, 00, C0, 00),	/* Basic Magenta */
	RGBA(00, C0, 00, 00),	/* Basic Green	 */
	RGBA(00, C0, C0, 00),	/* Basic Cyan	 */
	RGBA(C0, C0, 00, 00),	/* Basic Yellow	 */
	RGBA(C0, C0, C0, 00),	/* Basic White	 */
};

static const quint32 palette[] = {
	RGBA(00, 00, 00, 00), RGBA(00, 00, C0, 00),
	RGBA(C0, 00, 00, 00), RGBA(C0, 00, C0, 00),
	RGBA(00, C0, 00, 00), RGBA(00, C0, C0, 00),
	RGBA(C0, C0, 00, 00), RGBA(C0, C0, C0, 00),
	RGBA(00, 00, 00, 00), RGBA(00, 00, FF, 00),
	RGBA(FF, 00, 00, 00), RGBA(FF, 00, FF, 00),
	RGBA(00, FF, 00, 00), RGBA(00, FF, FF, 00),
	RGBA(FF, FF, 00, 00), RGBA(FF, FF, FF, 00)
};


#import "Z80.h"


/* MARK: - Helpers */


static void update_audio_output(ZXSpectrum *object, qsize new_index)
	{
	qint16 sample = object->current_audio_sample;
	qint16 *p = &object->audio_output_buffer[object->audio_sample_index];
	qint16 *e = &object->audio_output_buffer[new_index > 882 ? 882 : new_index];

	for (; p != e; p++) *p = sample;
	object->audio_sample_index = new_index;
	}


static void draw_character_row(quint8 *vram, qsize x, qsize y, qsize row, qboolean flash, quint32 *output)
	{
	quint8 character = *(vram + 2048 * (y / 8) + 32 * (y - 8 * (y / 8)) + 256 * row + x);
	quint8 attribute = *(vram + Q_ZX_SPECTRUM_VIDEO_CHARACTER_RAM_SIZE + 32 * y + x);
	quint32	f, b;
	qsize cx;

	if (flash && (attribute & 128))
		{
		if (attribute & 64)
			{
			b = bright_colors[ attribute	   & 7];
			f = bright_colors[(attribute >> 3) & 7];
			}

		else	{
			b = basic_colors[ attribute	  & 7];
			f = basic_colors[(attribute >> 3) & 7];
			}
		}

	else	{
		if (attribute & 64)
			{
			f = bright_colors[ attribute	   & 7];
			b = bright_colors[(attribute >> 3) & 7];
			}

		else	{
			f = basic_colors[ attribute	  & 7];
			b = basic_colors[(attribute >> 3) & 7];
			}
		}

	for (cx = 0; cx < 8; cx++, output++)
		*output = (character & (128 >> cx)) ? f : b;
	}


/* MARK: - CPU Callbacks: Memory Access */


static quint8 zx_spectrum_16k_cpu_read(ZXSpectrum *object, quint16 address)
	{return address < 0x8000 ? object->memory[address] : 0;}


static void zx_spectrum_16k_cpu_write(
	ZXSpectrum*	object,
	quint16		address,
	quint8		value
)
	{if (address > 0x3FFF && address < 0x8000) object->memory[address] = value;}


static quint8 zx_spectrum_48k_cpu_read(ZXSpectrum *object, quint16 address)
	{return object->memory[address];}


static void zx_spectrum_48k_cpu_write(
	ZXSpectrum*	object,
	quint16		address,
	quint8		value
)
	{if (address > 0x3FFF) object->memory[address] = value;}


static quint8 zx_spectrum_plus_128k_cpu_read(ZXSpectrum128K *object, quint16 address)
	{return object->memory_pages[address / KB(16)][address % KB(16)];}


static void zx_spectrum_plus_128k_cpu_write(
	ZXSpectrum128K*	object,
	quint16		address,
	quint8		value
)
	{
	if (address > 0x3FFF)
		object->memory_pages[address / KB(16)][address % KB(16)] = value;
	}


/* MARK: - CPU Callbacks: I/O */


static quint8 zx_spectrum_cpu_in(ZXSpectrum *object, quint16 port)
	{
	quint8 value;

	if (port & 1) switch (port & 0xFF)
		{
		/*----.
		| ULA |
		'----*/
		case Q_ZX_SPECTRUM_IO_PORT_ULA:
		return 0;

		/*------------------.
		| Kempston Joystick |
		'------------------*/
		case Q_ZX_SPECTRUM_IO_PORT_KEMPSTON_JOYSTICK:
		/* treat this as attached but unused
		 (for the benefit of Manic Miner) */
		return 0xFF;

		/*----------------.
		| Fuller Joystick |
		'----------------*/
		case Q_ZX_SPECTRUM_IO_PORT_FULLER_JOYSTICK:
		return 0xFF;

		/*------------------.
		| Mikrogen Joystick |
		'------------------*/
		case Q_ZX_SPECTRUM_IO_PORT_MIKROGEN_JOYSTICK:
		return 0xFF;

		/*--------------------.
		| Unassigned I/O port |
		'--------------------*/
		default: return Q_ZX_SPECTRUM_UNASSIGNED_IO_PORT_INPUT;
		}

	else	{
		/*---------.
		| Keyboard |
		'---------*/
		value = 0xBF;
		if (!(port & (1 <<  8))) value &= object->state.keyboard.array_uint8[0];
		if (!(port & (1 <<  9))) value &= object->state.keyboard.array_uint8[1];
		if (!(port & (1 << 10))) value &= object->state.keyboard.array_uint8[2];
		if (!(port & (1 << 11))) value &= object->state.keyboard.array_uint8[3];
		if (!(port & (1 << 12))) value &= object->state.keyboard.array_uint8[4];
		if (!(port & (1 << 13))) value &= object->state.keyboard.array_uint8[5];
		if (!(port & (1 << 14))) value &= object->state.keyboard.array_uint8[6];
		if (!(port & (1 << 15))) value &= object->state.keyboard.array_uint8[7];

		if (object->audio_input_buffer)
			{
			qsize index = ((*object->cpu_cycles + object->frame_cycles) * 882) / object->cycles->per_frame;

			if (object->audio_input_buffer[object->audio_input_base_index + index] == 0x90)
				value |= 0x40;

			//else printf("0\n");
			}
		}

	return value;
	}


static quint8 zx_spectrum_plus_128k_cpu_in(ZXSpectrum128K *object, quint16 port)
	{
	if (port == 0xFFFD)
		{
		return 0;
		}

	else return zx_spectrum_cpu_in((ZXSpectrum *)object, port);
	}


static void zx_spectrum_cpu_out(ZXSpectrum *object, quint16 port, quint8 value)
	{
	if ((port & 1) == 0)
		{
		/*-------------.
		| Border Color |
		'-------------*/
		object->border_color = palette[value & 0x07];

		/*----------.
		| MIC - EAR |
		'----------*/
		if ((object->port_fe ^ value) & 24)
			{
			qsize new_index = ((*object->cpu_cycles + object->frame_cycles) * 882) / object->cycles->per_frame;
			update_audio_output(object, new_index);
			object->current_audio_sample = (value & 0x10) ? WAVE_HIGH : WAVE_LOW;
			}

		object->port_fe = value;
		object->port_fe_update_cycle = ((Z80 *)object->cpu)->cycles;
	//	printf("sending something to ULA => %hhX\n", value);
		}
	}


static void zx_spectrum_plus_128k_cpu_out(ZXSpectrum128K *object, quint16 port, quint8 value)
	{
	if (port == 0x7FFD)
		{
		if (!object->disable_bank_switching)
			{
			object->memory_pages[0] = ROM_BANK(!!(value & 16));
			object->memory_pages[1] = object->vram = RAM_BANK(value &  8 ? 5 : 7);
			object->memory_pages[3] = RAM_BANK(value &  7);
			object->disable_bank_switching = !!(value & 32);
			object->memory_pages[1][0x5B5C - 0x4000] = value;
			}
		}

	else if (port == 0xFFFD)
		{
		}

	else if (port == 0xBFFD)
		{
		}

	else zx_spectrum_cpu_out((ZXSpectrum *)object, port, value);
	}


static quint32 cpu_int_data(ZXSpectrum *object)
	{
	return 00;
	}


static void cpu_halt(ZXSpectrum *object)
	{
	}





#include "Z80.h"

#define  CPU(object) ((Z80 *)(object))

static void zx_spectrum_initialize(ZXSpectrum *object)
	{
	object->frames_since_flash = 0;
	CPU(object->cpu)->cb.read	= (void *)zx_spectrum_48k_cpu_read;
	CPU(object->cpu)->cb.write	= (void *)zx_spectrum_48k_cpu_write;
	CPU(object->cpu)->cb.in		= (void *)zx_spectrum_cpu_in;
	CPU(object->cpu)->cb.out	= (void *)zx_spectrum_cpu_out;
	CPU(object->cpu)->cb.int_data	= (void *)cpu_int_data;
	CPU(object->cpu)->cb.halt	= (void *)cpu_halt;
	CPU(object->cpu)->cb_context	= object;

	object->screen_border = &zx_spectrum_screen_border;
	object->cycles = &zx_spectrum_cycles;
	object->state.keyboard.value_uint64 = 0xFFFFFFFFFFFFFFFF;
	object->state.ula_io.value = 0;
	object->state.flash = FALSE;
	object->current_audio_sample = WAVE_LOW;
	object->border_color = palette[0];
	object->frame_cycles = 0;
	object->audio_input_buffer = NULL;
	object->audio_input_base_index = 0;
	object->vram = object->memory + Q_ZX_SPECTRUM_ADDRESS_VIDEO_CHARACTER_RAM;
	}


static void zx_spectrum_plus_128k_initialize(ZXSpectrum128K *object)
	{
	object->frames_since_flash = 0;
	CPU(object->cpu)->cb.read	= (void *)zx_spectrum_plus_128k_cpu_read;
	CPU(object->cpu)->cb.write	= (void *)zx_spectrum_plus_128k_cpu_write;
	CPU(object->cpu)->cb.in		= (void *)zx_spectrum_plus_128k_cpu_in;
	CPU(object->cpu)->cb.out	= (void *)zx_spectrum_plus_128k_cpu_out;
	CPU(object->cpu)->cb.int_data	= (void *)cpu_int_data;
	CPU(object->cpu)->cb.halt	= (void *)cpu_halt;
	CPU(object->cpu)->cb_context	= object;

	object->screen_border = &zx_spectrum_screen_border;
	object->cycles = &zx_spectrum_plus_128k_cycles;
	object->state.keyboard.value_uint64 = 0xFFFFFFFFFFFFFFFF;
	object->state.ula_io.value = 0;
	object->state.flash = FALSE;
	object->current_audio_sample = WAVE_LOW;
	object->border_color = palette[0];
	object->frame_cycles = 0;
	object->audio_input_buffer = NULL;
	object->audio_input_base_index = 0;
	object->disable_bank_switching = FALSE;

	object->port_7ffd = 0;
	object->memory_pages[0] = object->memory;
	object->memory_pages[1] = object->vram = RAM_BANK(5);
	object->memory_pages[2] = RAM_BANK(2);
	object->memory_pages[3] = RAM_BANK(0);
	}


static void zx_spectrum_power(ZXSpectrum *object, qboolean state)
	{CPU_POWER(state);}


static void zx_spectrum_reset(ZXSpectrum *object)
	{CPU_RESET;}


#define CYCLES_AT_LINE(region, scanline_index) \
	(cycles.at_##region + cycles.per_scanline * (scanline_index))

static void zx_spectrum_run_one_frame(ZXSpectrum *object)
	{
	qsize i;
	qsize cx, cy, row;
	quint32 *p = object->video_output_buffer, *e;
	quint32 border_color;
	Cycles cycles = *object->cycles;
	ScreenBorder screen_border = *object->screen_border;

	object->audio_sample_index = 0;

	if (object->frames_since_flash == 16)
		{
		object->frames_since_flash = 0;
		object->state.flash = !object->state.flash;
		}

	/*------------------.
	| VBLANK before INT |
	'------------------*/
	object->frame_cycles += CPU_RUN(cycles.at_int - object->frame_cycles);

	/*------------------.
	| VBLANK during INT |
	'------------------*/
	CPU_INT(ON);
	object->frame_cycles += CPU_RUN(cycles.per_int);
	CPU_INT(OFF);

	/*---------------------------------------------------.
	| VBLANK after INT and invisble top border scanlines |
	'---------------------------------------------------*/
	object->frame_cycles += CPU_RUN(cycles.at_visible_top_border - object->frame_cycles);

	/*-----------------------------.
	| Visible top border scanlines |
	'-----------------------------*/
	for (i = 0; i < screen_border.top; i++)
		{
		object->frame_cycles += CPU_RUN
			(cycles.per_scanline - (object->frame_cycles - CYCLES_AT_LINE(visible_top_border, i)));

		border_color = object->border_color;

		for (e = p + Q_ZX_SPECTRUM_SCREEN_WIDTH; p != e; p++)
			*p = border_color;
		}

	/*----------------.
	| Video scanlines |
	'----------------*/
	for (i = 0; i < Q_ZX_SPECTRUM_SCREEN_PAPER_HEIGHT; i++)
		{
		object->frame_cycles += CPU_RUN
			(cycles.per_scanline - (object->frame_cycles - CYCLES_AT_LINE(paper_region, i)));

		border_color = object->border_color;

		for (e = p + Q_ZX_SPECTRUM_SCREEN_LEFT_BORDER_WIDTH; p != e; p++)
			*p = border_color;

		for (cx = 0, cy = i / 8, row = i % 8; cx < Q_ZX_SPECTRUM_SCREEN_PAPER_WIDTH / 8; cx++, p += 8)
			draw_character_row(object->vram, cx, cy, row, object->state.flash, p);

		for (e = p + Q_ZX_SPECTRUM_SCREEN_RIGHT_BORDER_WIDTH; p != e; p++)
			*p = border_color;
		}

	/*------------------------.
	| Bottom border scanlines |
	'------------------------*/
	for (i = 0; i < screen_border.bottom; i++)
		{
		object->frame_cycles += CPU_RUN
			(cycles.per_scanline - (object->frame_cycles - CYCLES_AT_LINE(bottom_border, i)));

		border_color = object->border_color;

		for (e = p + Q_ZX_SPECTRUM_SCREEN_WIDTH; p != e; p++)
			*p = border_color;
		}

	update_audio_output(object, 882);
	object->frames_since_flash++;
	object->frame_cycles -= cycles.per_frame;

	if (object->audio_input_buffer != NULL)
		{
		quint8 *input = object->audio_input_buffer + 882;
		quint index = 882;

		while (index)
			object->audio_output_buffer[--index] = *--input == 0x90 ? WAVE_HIGH : WAVE_LOW;
		}

	i = 0;
	}


static void zx_spectrum_run_one_scanline(ZXSpectrum *object)
	{
	}


#include "MachineABI.h"


static const ROM zx_spectrum_rom	 = {"ZX Spectrum (Firmware)(ROM)",	0, KB(16)};
static const ROM inves_spectrum_plus_rom = {"Inves Spectrum + (Firmware)(ROM)", 0, KB(16)};

static const ROM zx_spectrum_plus_128k_roms[] = {
	{"ZX Spectrum + 128K (Firmware)(ROM 1 of 2)", 0,      KB(16)},
	{"ZX Spectrum + 128K (Firmware)(ROM 2 of 2)", KB(16), KB(16)}
};

static const ROM zx_spectrum_plus_128k_es_roms[] = {
	{"ZX Spectrum + 128K (ES)(Firmware)(ROM 1 of 2)", 0,	  KB(16)},
	{"ZX Spectrum + 128K (ES)(Firmware)(ROM 2 of 2)", KB(16), KB(16)}
};

MachineABI machine_abi_table[] = {

	{.model_name		= "ZX Spectrum 16K (Issue 1)",
	.context_size		= sizeof(ZXSpectrum),
	.memory_size		= Q_ZX_SPECTRUM_16K_ISSUE_1_MEMORY_SIZE,
	.roms			= (ROM *)&zx_spectrum_rom,
	.rom_count		= 1,
	.initialize		= (void *)zx_spectrum_initialize,
	.power			= (void *)zx_spectrum_power,
	.reset			= (void *)zx_spectrum_reset,
	.run_one_frame		= (void *)zx_spectrum_run_one_frame,
	.run_one_scanline	= (void *)zx_spectrum_run_one_scanline},

	{.model_name		= "ZX Spectrum 48K (Issue 2)",
	.context_size		= sizeof(ZXSpectrum),
	.memory_size		= Q_ZX_SPECTRUM_48K_ISSUE_2_MEMORY_SIZE,
	.roms			= (ROM *)&zx_spectrum_rom,
	.rom_count		= 1,
	.initialize		= (void *)zx_spectrum_initialize,
	.power			= (void *)zx_spectrum_power,
	.reset			= (void *)zx_spectrum_reset,
	.run_one_frame		= (void *)zx_spectrum_run_one_frame,
	.run_one_scanline	= (void *)zx_spectrum_run_one_scanline},

	{.model_name		= "ZX Spectrum 48K (Issue 3)",
	.context_size		= sizeof(ZXSpectrum),
	.memory_size		= Q_ZX_SPECTRUM_48K_ISSUE_3_MEMORY_SIZE,
	.roms			= (ROM *)&zx_spectrum_rom,
	.rom_count		= 1,
	.initialize		= (void *)zx_spectrum_initialize,
	.power			= (void *)zx_spectrum_power,
	.reset			= (void *)zx_spectrum_reset,
	.run_one_frame		= (void *)zx_spectrum_run_one_frame,
	.run_one_scanline	= (void *)zx_spectrum_run_one_scanline},

	{.model_name		= "ZX Spectrum +",
	.context_size		= sizeof(ZXSpectrum),
	.memory_size		= Q_ZX_SPECTRUM_PLUS_MEMORY_SIZE,
	.roms			= (ROM *)&zx_spectrum_rom,
	.rom_count		= 1,
	.initialize		= (void *)zx_spectrum_initialize,
	.power			= (void *)zx_spectrum_power,
	.reset			= (void *)zx_spectrum_reset,
	.run_one_frame		= (void *)zx_spectrum_run_one_frame,
	.run_one_scanline	= (void *)zx_spectrum_run_one_scanline},

	{.model_name		= "ZX Spectrum + 128K",
	 .context_size		= sizeof(ZXSpectrum128K),
	 .memory_size		= Q_ZX_SPECTRUM_PLUS_128K_MEMORY_SIZE,
	 .roms			= (ROM *)&zx_spectrum_plus_128k_roms[0],
	 .rom_count		= 2,
	 .initialize		= (void *)zx_spectrum_plus_128k_initialize,
	 .power			= (void *)zx_spectrum_power,
	 .reset			= (void *)zx_spectrum_reset,
	 .run_one_frame		= (void *)zx_spectrum_run_one_frame,
	 .run_one_scanline	= (void *)zx_spectrum_run_one_scanline},

	{.model_name		= "ZX Spectrum + 128K (ES)",
	 .context_size		= sizeof(ZXSpectrum128K),
	 .memory_size		= Q_ZX_SPECTRUM_PLUS_128K_MEMORY_SIZE,
	 .roms			= (ROM *)&zx_spectrum_plus_128k_es_roms[0],
	 .rom_count		= 2,
	 .initialize		= (void *)zx_spectrum_plus_128k_initialize,
	 .power			= (void *)zx_spectrum_power,
	 .reset			= (void *)zx_spectrum_reset,
	 .run_one_frame		= (void *)zx_spectrum_run_one_frame,
	 .run_one_scanline	= (void *)zx_spectrum_run_one_scanline},

	{.model_name		= "ZX Spectrum +2"},
	{.model_name		= "ZX Spectrum +2 (ES)"},
	{.model_name		= "ZX Spectrum +2 (FR)"},
	{.model_name		= "ZX Spectrum +2A"},
	{.model_name		= "ZX Spectrum +2A (ES)"},
	{.model_name		= "ZX Spectrum +3"},
	{.model_name		= "ZX Spectrum +3 (ES)"},

	{.model_name		= "Inves Spectrum +",
	.context_size		= sizeof(ZXSpectrum),
	.memory_size		= Q_INVES_SPECTRUM_PLUS_MEMORY_SIZE,
	.roms			= (ROM *)&inves_spectrum_plus_rom,
	.rom_count		= 1,
	.initialize		= (void *)zx_spectrum_initialize,
	.power			= (void *)zx_spectrum_power,
	.reset			= (void *)zx_spectrum_reset,
	.run_one_frame		= (void *)zx_spectrum_run_one_frame,
	.run_one_scanline	= (void *)zx_spectrum_run_one_scanline},

};

qsize machine_abi_count = 14;

/* ZXSpectrum.c EOF */
