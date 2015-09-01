/* ACH ZX Spectrum Snapshot Codec v1.0
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013 Manuel Sainz de Baranda y Goñi <manuel@redcodesofware.com>
Released under the terms of the GNU General Public License v3. */

#include <Z/formats/hardware state/machine/ZX Spectrum/ACH.h>
#include <Z/hardware/machine/platform/computer/ZX Spectrum.h>
#include <Z/hardware/CPU/architecture/Z80.h>
#include <Z/functions/base/value.h>
#include <Z/keys/status.h>
#include <string.h>


static ZStatus test(ZACH *object, zsize object_size)
	{
	if (object_size	!= sizeof(ZACH)) return Z_ERROR_INVALID_SIZE;

	return Z_OK;
	}


static void decode(
	ZACH*			object,
	ZZXSpectrumState*	machine,
	ZZ80State*		cpu,
	zuint8*			memory
)
	{
	machine->keyboard.value_uint64 = Z_UINT64(0xFFFFFFFFFFFFFFFF);
	machine->flash		       = FALSE;
	machine->ula_io.value	       = object->border_color & 7;

	Z_Z80_STATE_PC(	 cpu) = z_uint16_little_endian(object->pc);
	Z_Z80_STATE_SP(	 cpu) = z_uint16_little_endian(object->sp);
	Z_Z80_STATE_A (	 cpu) = object->a;
	Z_Z80_STATE_F (	 cpu) = object->f;
	Z_Z80_STATE_B (	 cpu) = object->b;
	Z_Z80_STATE_C (	 cpu) = object->c;
	Z_Z80_STATE_D (	 cpu) = object->d;
	Z_Z80_STATE_E (	 cpu) = object->e;
	Z_Z80_STATE_H (	 cpu) = object->h;
	Z_Z80_STATE_L (	 cpu) = object->l;
	Z_Z80_STATE_IX(	 cpu) = z_uint16_little_endian(object->ix.value_uint16);
	Z_Z80_STATE_IY(	 cpu) = z_uint16_little_endian(object->iy.value_uint16);
	Z_Z80_STATE_A_(	 cpu) = object->a_;
	Z_Z80_STATE_F_(	 cpu) = object->f_;
	Z_Z80_STATE_B_(	 cpu) = object->b_;
	Z_Z80_STATE_C_(	 cpu) = object->c_;
	Z_Z80_STATE_D_(	 cpu) = object->d_;
	Z_Z80_STATE_E_(	 cpu) = object->e_;
	Z_Z80_STATE_H_(	 cpu) = object->h_;
	Z_Z80_STATE_L_(	 cpu) = object->l_;
	Z_Z80_STATE_I(	 cpu) = object->i;
	Z_Z80_STATE_R(	 cpu) = object->r;
	Z_Z80_STATE_HALT(cpu) = FALSE;
	Z_Z80_STATE_IRQ( cpu) = FALSE;
	Z_Z80_STATE_NMI( cpu) = FALSE;
	Z_Z80_STATE_IFF1(cpu) =
	Z_Z80_STATE_IFF2(cpu) = object->iff2 & 1;
	Z_Z80_STATE_EI(	 cpu) = FALSE;
	Z_Z80_STATE_IM(	 cpu) = object->im   & 3;

	memcpy(memory, object->memory, 1024 * 64);
	}


static void encode(
	ZACH*			object,
	ZZXSpectrumState*	machine,
	ZZ80State*		cpu,
	zuint8*			memory
)
	{
	memset(object, 0, sizeof(ZACH) - 1024 * 64);

	object->a		= Z_Z80_STATE_A(cpu);
	object->f		= Z_Z80_STATE_F(cpu);
	object->b		= Z_Z80_STATE_B(cpu);
	object->c		= Z_Z80_STATE_C(cpu);
	object->d		= Z_Z80_STATE_D(cpu);
	object->e		= Z_Z80_STATE_E(cpu);
	object->h		= Z_Z80_STATE_H(cpu);
	object->l		= Z_Z80_STATE_L(cpu);
	object->pc		= z_uint16_little_endian(Z_Z80_STATE_PC(cpu));
	object->sp		= z_uint16_little_endian(Z_Z80_STATE_SP(cpu));
	object->r		= Z_Z80_STATE_R(cpu);
	object->border_color	= machine->ula_io.fields.border_color;
	object->im		= Z_Z80_STATE_IM(cpu);
	object->i		= Z_Z80_STATE_I(cpu);
	object->iff2		= Z_Z80_STATE_IFF2(cpu);
	object->a_		= Z_Z80_STATE_A_(cpu);
	object->f_		= Z_Z80_STATE_F_(cpu);
	object->b_		= Z_Z80_STATE_B_(cpu);
	object->c_		= Z_Z80_STATE_C_(cpu);
	object->d_		= Z_Z80_STATE_D_(cpu);
	object->e_		= Z_Z80_STATE_E_(cpu);
	object->h_		= Z_Z80_STATE_H_(cpu);
	object->l_		= Z_Z80_STATE_L_(cpu);
	object->ix.value_uint16 = z_uint16_little_endian(Z_Z80_STATE_IX(cpu));
	object->iy.value_uint16 = z_uint16_little_endian(Z_Z80_STATE_IY(cpu));

	memcpy(object->memory, memory, 1024 * 64);
	}


/* ACH.c EOF */
