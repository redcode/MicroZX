/* General Instrument AY-3-891x PSG Emulator v1.0
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#include "AY-3-891x.h"
#include <Q/macros/value.h>

#define R(index) object->r[index]

static const quint8 levels[16] = {
	0x00, 0x03, 0x05, 0x07,
	0x0A, 0x0F, 0x15, 0x23,
	0x2B, 0x43, 0x5A, 0x73,
	0x92, 0xAF, 0xD9, 0xFF
};

/*
#define Q_AY_3_891X_CHANNEL_A_TONE_PERIOD_FINE_TUNE	 0
#define Q_AY_3_891X_CHANNEL_A_TONE_PERIOD_COARSE_TUNE	 1
#define Q_AY_3_891X_CHANNEL_B_TONE_PERIOD_FINE_TUNE	 2
#define Q_AY_3_891X_CHANNEL_B_TONE_PERID_COARSE_TUNE	 3
#define Q_AY_3_891X_CHANNEL_C_TONE_PERIOD_FINE_TUNE	 4
#define Q_AY_3_891X_CHANNEL_C_TONE_PERIOD_COARSE_TUNE	 5
#define Q_AY_3_891X_NOISE_PERIOD			 6
#define Q_AY_3_891X_ENABLEE				 7
#define Q_AY_3_891X_CHANNEL_A_AMPLITUDE			 8
#define Q_AY_3_891X_CHANNEL_B_AMPLITUDE			 9
#define Q_AY_3_891X_CHANNEL_C_AMPLITUDE			10
#define Q_AY_3_891X_ENVELOPE_PERIOD_FINE_TUNE		11
#define Q_AY_3_891X_ENVELOPE_PERIOD_COARSE_TUNE		12
#define Q_AY_3_891X_ENVELOPE_SHAPE_CYCLE		13
#define Q_AY_3_891X_IO_PORT_A_DATA_STORE		14
#define Q_AY_3_891X_IO_PORT_B_DATA_STORE		15*/

void power(AY3891x *object, qboolean state)
	{
	}


void reset(AY3891x *object)
	{
	}


qsize run(AY3891x *object, qsize cycles)
	{

	return cycles;
	}
