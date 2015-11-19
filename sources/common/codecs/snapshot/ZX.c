/* ZX ZX Spectrum Snapshot Codec v1.0
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013 Manuel Sainz de Baranda y Goñi <manuel@redcodesofware.com>
Released under the terms of the GNU General Public License v3. */

#include <Z/formats/snapshot/machine/computer/ZX Spectrum/ZX.h>
#include <Z/hardware/machine/platform/computer/ZX Spectrum.h>
#include <Z/hardware/CPU/architecture/Z80.h>
#include <Z/functions/base/value.h>
#include <Z/keys/status.h>
#include <string.h>


static ZStatus test(ZZX *object, zsize object_size)
	{
	if (object_size	!= sizeof(ZZX)) return Z_ERROR_INVALID_SIZE;

	return Z_OK;
	}


static void decode(
	ZZX*			object,
	ZZXSpectrumState*	machine,
	ZZ80State*		cpu,
	zuint8*			memory
)
	{
	machine->keyboard.value_uint64 = Z_UINT64(0xFFFFFFFFFFFFFFFF);
	machine->flash		       = FALSE;
	machine->ula_io.value	       = 0;

	Z_Z80_STATE_PC(	 cpu) = z_uint16_big_endian(object->pc);
	Z_Z80_STATE_SP(	 cpu) = z_uint16_big_endian(object->sp);
	Z_Z80_STATE_A(	 cpu) = object->a;
	Z_Z80_STATE_F(	 cpu) = object->f;
	Z_Z80_STATE_BC(	 cpu) = z_uint16_big_endian(object->bc.value_uint16);
	Z_Z80_STATE_DE(	 cpu) = z_uint16_big_endian(object->de.value_uint16);
	Z_Z80_STATE_HL(	 cpu) = z_uint16_big_endian(object->hl.value_uint16);
	Z_Z80_STATE_IX(	 cpu) = z_uint16_big_endian(object->ix.value_uint16);
	Z_Z80_STATE_IY(	 cpu) = z_uint16_big_endian(object->iy.value_uint16);
	Z_Z80_STATE_A_(	 cpu) = object->a_;
	Z_Z80_STATE_F_(	 cpu) = object->f_;
	Z_Z80_STATE_BC_( cpu) = z_uint16_big_endian(object->bc_.value_uint16);
	Z_Z80_STATE_DE_( cpu) = z_uint16_big_endian(object->de_.value_uint16);
	Z_Z80_STATE_HL_( cpu) = z_uint16_big_endian(object->hl_.value_uint16);
	Z_Z80_STATE_I(	 cpu) = object->i;
	Z_Z80_STATE_R(	 cpu) = object->r;
	Z_Z80_STATE_HALT(cpu) = object->halt & 1;
	Z_Z80_STATE_IRQ( cpu) = FALSE;
	Z_Z80_STATE_NMI( cpu) = FALSE;
	Z_Z80_STATE_IFF1(cpu) =
	Z_Z80_STATE_IFF2(cpu) = object->iff & 1;
	Z_Z80_STATE_EI(	 cpu) = FALSE;
	Z_Z80_STATE_IM(	 cpu) = object->im & 3;

	memcpy(memory + 1024 * 16, object->ram, 1024 * 48);
	}


static void encode(
	ZZX*			object,
	ZZXSpectrumState*	machine,
	ZZ80State*		cpu,
	zuint8*			memory
)
	{
	memcpy(object->rom_tail, memory + 1024 * 16 - 132, 132 + 1024 * 48);

	memset	(object	     + Z_OFFSET_OF(ZZX, zero_0), 0,
		 sizeof(ZZX) - Z_OFFSET_OF(ZZX, zero_0));

	object->settings[0]	 = 10;
	object->settings[1]	 = 10;
	object->settings[2]	 = 4;
	object->settings[3]	 = 1;
	object->settings[4]	 = 1;
	object->iff		 = Z_Z80_STATE_IFF2(cpu);
	object->constants[0]	 = 0;
	object->constants[1]	 = 3;
	object->color_mode	 = Z_ZX_COLOR_MODE_COLOR;
	object->bc.value_uint16  = z_uint16_big_endian(Z_Z80_STATE_BC (cpu));
	object->bc_.value_uint16 = z_uint16_big_endian(Z_Z80_STATE_BC_(cpu));
	object->de.value_uint16  = z_uint16_big_endian(Z_Z80_STATE_DE (cpu));
	object->de_.value_uint16 = z_uint16_big_endian(Z_Z80_STATE_DE_(cpu));
	object->hl.value_uint16  = z_uint16_big_endian(Z_Z80_STATE_HL (cpu));
	object->hl_.value_uint16 = z_uint16_big_endian(Z_Z80_STATE_HL_(cpu));
	object->ix.value_uint16  = z_uint16_big_endian(Z_Z80_STATE_IX (cpu));
	object->iy.value_uint16	 = z_uint16_big_endian(Z_Z80_STATE_IY (cpu));
	object->i		 = Z_Z80_STATE_I(cpu);
	object->r		 = Z_Z80_STATE_R(cpu);
	object->a_		 = Z_Z80_STATE_A_(cpu);
	object->a		 = Z_Z80_STATE_A(cpu);
	object->f_		 = Z_Z80_STATE_F_(cpu);
	object->f		 = Z_Z80_STATE_F(cpu);
	object->pc		 = z_uint16_big_endian(Z_Z80_STATE_PC(cpu));
	object->sp		 = z_uint16_big_endian(Z_Z80_STATE_SP(cpu));
	object->sound_mode	 = Z_ZX_SOUND_MODE_SIMPLE;
	object->halt		 = Z_Z80_STATE_HALT(cpu);
	object->im		 = ((zint16)Z_Z80_STATE_IM(cpu)) - 1;
	}


/* ZX.c EOF */
