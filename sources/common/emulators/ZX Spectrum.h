/* Spectra - ZXSpectrum.h
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2011 RedCode Software.
Released under the terms of the GNU General Public License v2. */

#define USE_STATIC_EMULATION_CPU_Z80
#include "Z80.h"
#include <Z/hardware/machine/platform/computer/ZX Spectrum.h>
#include <Z/ABIs/emulation.h>

typedef struct {
	zsize cycles_per_frame;
	zsize cycles_per_scanline;
	zsize cycles_per_int;
} Timmings;

#define ZX_SPECTRUM_VALUES				  \
	zuint8*			memory;			  \
	Z80*			cpu;			  \
	zsize*			cpu_cycles;		  \
	void*			video_output_buffer;  \
	zuint8*			audio_input_buffer;   \
	zint16*			audio_output_buffer; \
							  \
	struct {ZEmulatorRun	run;			  \
		ZSwitch	power;			\
		ZDo	reset;			\
		ZSwitch	irq;			\
	} cpu_abi;					\
							\
	zuint8			keyboard[8];		\
	zuint32			border_color;		\
	zsize			frame_cycles;		\
	zsize			frames_since_flash;	\
	zint16			current_audio_sample;	\
	zsize			audio_sample_index;	\
	zsize			audio_input_base_index;	\
	zboolean		accurate;		\
	ZZXSpectrumState	state;			\
	const void*		screen_border;		\
	const void*		cycles;			\
	const void*		contention;		\
	zuint8			port_fe;		\
	zuint8			port_fe_update_cycle;	\
	zuint8*			vram;			\

typedef struct {
	ZX_SPECTRUM_VALUES
} ZXSpectrum;

