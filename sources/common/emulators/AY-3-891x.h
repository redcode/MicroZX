/* General Instrument AY-3-891x PSG Emulator v1.0
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#include <Z/hardware/PSG/General Instrument/AY-3-891x.h>
#include <Z/types/generic functions.h>

#ifndef __modules_emulation_PSG_AY_3_891x_H__
#define __modules_emulation_PSG_AY_3_891x_H__

typedef struct {
	zuint8 r[16];

	struct {Z16BitAddressRead8Bit  read;
		Z16BitAddressWrite8Bit write;
		Z16BitAddressRead8Bit  in;
		Z16BitAddressWrite8Bit out;
		ZRead32Bit	       int_data;
		ZSwitch		       halt;
	} cb;
} AY3891x;

Z_C_SYMBOLS_BEGIN



Z_C_SYMBOLS_END

#endif /* __modules_emulation_PSG_AY_3_891x_H__ */
