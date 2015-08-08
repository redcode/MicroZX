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
#include <Q/hardware/machine/platform/computer/ZX Spectrum.h>
#include <Q/ABIs/emulation.h>

typedef struct {
	qsize cycles_per_frame;
	qsize cycles_per_scanline;
	qsize cycles_per_int;
} Timmings;

#define ZX_SPECTRUM_VALUES				\
	quint8*			memory;			\
	Z80*			cpu;			\
	qsize*			cpu_cycles;		\
	void*			video_output_buffer;	\
	quint8*			audio_input_buffer;	\
	qint16*			audio_output_buffer;	\
							\
	struct {QEmulatorRun	run;			\
		QSwitch	power;			\
		QDo	reset;			\
		QSwitch	irq;			\
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
	const void*		screen_border;		\
	const void*		cycles;			\
	const void*		contention;		\
	quint8			port_fe;		\
	quint8			port_fe_update_cycle;	\
	quint8*			vram;			\

typedef struct {
	ZX_SPECTRUM_VALUES
} ZXSpectrum;

