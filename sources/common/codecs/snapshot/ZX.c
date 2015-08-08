/* ZX ZX Spectrum Snapshot Codec v1.0
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013 Manuel Sainz de Baranda y Goñi <manuel@redcodesofware.com>
Released under the terms of the GNU General Public License v3. */

#include <Q/formats/hardware state/machine/ZX Spectrum/ZX.h>
#include <Q/hardware/machine/platform/computer/ZX Spectrum.h>
#include <Q/hardware/CPU/architecture/Z80.h>
#include <Q/functions/base/value.h>
#include <Q/keys/status.h>
#include <string.h>


static QStatus test(QZX *object, qsize object_size)
	{
	if (object_size	!= sizeof(QZX)) return Q_ERROR_INVALID_SIZE;

	return Q_OK;
	}


static void decode(
	QZX*			object,
	QZXSpectrumState*	machine,
	QZ80State*		cpu,
	quint8*			memory
)
	{
	machine->keyboard.value_uint64 = Q_UINT64(0xFFFFFFFFFFFFFFFF);
	machine->flash		       = FALSE;
	machine->ula_io.value	       = 0;

	Q_Z80_STATE_PC(	 cpu) = q_uint16_big_endian(object->pc);
	Q_Z80_STATE_SP(	 cpu) = q_uint16_big_endian(object->sp);
	Q_Z80_STATE_A(	 cpu) = object->a;
	Q_Z80_STATE_F(	 cpu) = object->f;
	Q_Z80_STATE_BC(	 cpu) = q_uint16_big_endian(object->bc.value_uint16);
	Q_Z80_STATE_DE(	 cpu) = q_uint16_big_endian(object->de.value_uint16);
	Q_Z80_STATE_HL(	 cpu) = q_uint16_big_endian(object->hl.value_uint16);
	Q_Z80_STATE_IX(	 cpu) = q_uint16_big_endian(object->ix.value_uint16);
	Q_Z80_STATE_IY(	 cpu) = q_uint16_big_endian(object->iy.value_uint16);
	Q_Z80_STATE_A_(	 cpu) = object->a_;
	Q_Z80_STATE_F_(	 cpu) = object->f_;
	Q_Z80_STATE_BC_( cpu) = q_uint16_big_endian(object->bc_.value_uint16);
	Q_Z80_STATE_DE_( cpu) = q_uint16_big_endian(object->de_.value_uint16);
	Q_Z80_STATE_HL_( cpu) = q_uint16_big_endian(object->hl_.value_uint16);
	Q_Z80_STATE_I(	 cpu) = object->i;
	Q_Z80_STATE_R(	 cpu) = object->r;
	Q_Z80_STATE_HALT(cpu) = object->halt & 1;
	Q_Z80_STATE_IRQ( cpu) = FALSE;
	Q_Z80_STATE_NMI( cpu) = FALSE;
	Q_Z80_STATE_IFF1(cpu) =
	Q_Z80_STATE_IFF2(cpu) = object->iff & 1;
	Q_Z80_STATE_EI(	 cpu) = FALSE;
	Q_Z80_STATE_IM(	 cpu) = object->im & 3;

	memcpy(memory + 1024 * 16, object->ram, 1024 * 48);
	}


static void encode(
	QZX*			object,
	QZXSpectrumState*	machine,
	QZ80State*		cpu,
	quint8*			memory
)
	{
	memcpy(object->rom_tail, memory + 1024 * 16 - 132, 132 + 1024 * 48);

	memset	(object	     + Q_OFFSET_OF(QZX, zero_0), 0,
		 sizeof(QZX) - Q_OFFSET_OF(QZX, zero_0));

	object->settings[0]	 = 10;
	object->settings[1]	 = 10;
	object->settings[2]	 = 4;
	object->settings[3]	 = 1;
	object->settings[4]	 = 1;
	object->iff		 = Q_Z80_STATE_IFF2(cpu);
	object->constants[0]	 = 0;
	object->constants[1]	 = 3;
	object->color_mode	 = Q_ZX_COLOR_MODE_COLOR;
	object->bc.value_uint16  = q_uint16_big_endian(Q_Z80_STATE_BC (cpu));
	object->bc_.value_uint16 = q_uint16_big_endian(Q_Z80_STATE_BC_(cpu));
	object->de.value_uint16  = q_uint16_big_endian(Q_Z80_STATE_DE (cpu));
	object->de_.value_uint16 = q_uint16_big_endian(Q_Z80_STATE_DE_(cpu));
	object->hl.value_uint16  = q_uint16_big_endian(Q_Z80_STATE_HL (cpu));
	object->hl_.value_uint16 = q_uint16_big_endian(Q_Z80_STATE_HL_(cpu));
	object->ix.value_uint16  = q_uint16_big_endian(Q_Z80_STATE_IX (cpu));
	object->iy.value_uint16	 = q_uint16_big_endian(Q_Z80_STATE_IY (cpu));
	object->i		 = Q_Z80_STATE_I(cpu);
	object->r		 = Q_Z80_STATE_R(cpu);
	object->a_		 = Q_Z80_STATE_A_(cpu);
	object->a		 = Q_Z80_STATE_A(cpu);
	object->f_		 = Q_Z80_STATE_F_(cpu);
	object->f		 = Q_Z80_STATE_F(cpu);
	object->pc		 = q_uint16_big_endian(Q_Z80_STATE_PC(cpu));
	object->sp		 = q_uint16_big_endian(Q_Z80_STATE_SP(cpu));
	object->sound_mode	 = Q_ZX_SOUND_MODE_SIMPLE;
	object->halt		 = Q_Z80_STATE_HALT(cpu);
	object->im		 = ((qint16)Q_Z80_STATE_IM(cpu)) - 1;
	}


/* ZX.c EOF */
