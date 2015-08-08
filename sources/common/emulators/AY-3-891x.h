/* General Instrument AY-3-891x PSG Emulator v1.0
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#include <Q/hardware/PSG/General Instrument/AY-3-891x.h>
#include <Q/types/generic functions.h>

#ifndef __modules_emulation_PSG_AY_3_891x_H__
#define __modules_emulation_PSG_AY_3_891x_H__

typedef struct {
	quint8 r[16];

	struct {Q16BitAddressRead8Bit  read;
		Q16BitAddressWrite8Bit write;
		Q16BitAddressRead8Bit  in;
		Q16BitAddressWrite8Bit out;
		QRead32Bit	       int_data;
		QSwitch		       halt;
	} cb;
} AY3891x;

Q_C_SYMBOLS_BEGIN



Q_C_SYMBOLS_END

#endif /* __modules_emulation_PSG_AY_3_891x_H__ */
