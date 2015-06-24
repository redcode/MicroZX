/* ACH ZX Spectrum Snapshot Codec v1.0
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013 Manuel Sainz de Baranda y Goñi <manuel@redcodesofware.com>
Released under the terms of the GNU General Public License v3. */

#include <Q/formats/hardware state/machine/ZX Spectrum/ACH.h>
#include <Q/hardware/machine/platform/computer/ZX Spectrum.h>
#include <Q/hardware/CPU/architecture/Z80.h>
#include <Q/functions/base/value.h>
#include <Q/keys/status.h>
#include <string.h>


static QStatus test(QACH *object, qsize object_size)
	{
	if (object_size	!= sizeof(QACH)) return Q_ERROR_INVALID_SIZE;

	return OK;
	}


static void decode(
	QACH*			object,
	QZXSpectrumState*	machine,
	QZ80State*		cpu,
	quint8*			memory
)
	{
	machine->keyboard.value_uint64 = Q_UINT64(0xFFFFFFFFFFFFFFFF);
	machine->flash		       = FALSE;
	machine->ula_io.value	       = object->border_color & 7;

	Q_Z80_STATE_PC(	 cpu) = q_uint16_little_endian(object->pc);
	Q_Z80_STATE_SP(	 cpu) = q_uint16_little_endian(object->sp);
	Q_Z80_STATE_A (	 cpu) = object->a;
	Q_Z80_STATE_F (	 cpu) = object->f;
	Q_Z80_STATE_B (	 cpu) = object->b;
	Q_Z80_STATE_C (	 cpu) = object->c;
	Q_Z80_STATE_D (	 cpu) = object->d;
	Q_Z80_STATE_E (	 cpu) = object->e;
	Q_Z80_STATE_H (	 cpu) = object->h;
	Q_Z80_STATE_L (	 cpu) = object->l;
	Q_Z80_STATE_IX(	 cpu) = q_uint16_little_endian(object->ix.value_uint16);
	Q_Z80_STATE_IY(	 cpu) = q_uint16_little_endian(object->iy.value_uint16);
	Q_Z80_STATE_A_(	 cpu) = object->a_;
	Q_Z80_STATE_F_(	 cpu) = object->f_;
	Q_Z80_STATE_B_(	 cpu) = object->b_;
	Q_Z80_STATE_C_(	 cpu) = object->c_;
	Q_Z80_STATE_D_(	 cpu) = object->d_;
	Q_Z80_STATE_E_(	 cpu) = object->e_;
	Q_Z80_STATE_H_(	 cpu) = object->h_;
	Q_Z80_STATE_L_(	 cpu) = object->l_;
	Q_Z80_STATE_I(	 cpu) = object->i;
	Q_Z80_STATE_R(	 cpu) = object->r;
	Q_Z80_STATE_HALT(cpu) = FALSE;
	Q_Z80_STATE_IRQ( cpu) = FALSE;
	Q_Z80_STATE_NMI( cpu) = FALSE;
	Q_Z80_STATE_IFF1(cpu) =
	Q_Z80_STATE_IFF2(cpu) = object->iff2 & 1;
	Q_Z80_STATE_EI(	 cpu) = FALSE;
	Q_Z80_STATE_IM(	 cpu) = object->im   & 3;

	memcpy(memory, object->memory, 1024 * 64);
	}


static void encode(
	QACH*			object,
	QZXSpectrumState*	machine,
	QZ80State*		cpu,
	quint8*			memory
)
	{
	memset(object, 0, sizeof(QACH) - 1024 * 64);

	object->a		= Q_Z80_STATE_A(cpu);
	object->f		= Q_Z80_STATE_F(cpu);
	object->b		= Q_Z80_STATE_B(cpu);
	object->c		= Q_Z80_STATE_C(cpu);
	object->d		= Q_Z80_STATE_D(cpu);
	object->e		= Q_Z80_STATE_E(cpu);
	object->h		= Q_Z80_STATE_H(cpu);
	object->l		= Q_Z80_STATE_L(cpu);
	object->pc		= q_uint16_little_endian(Q_Z80_STATE_PC(cpu));
	object->sp		= q_uint16_little_endian(Q_Z80_STATE_SP(cpu));
	object->r		= Q_Z80_STATE_R(cpu);
	object->border_color	= machine->ula_io.fields.border_color;
	object->im		= Q_Z80_STATE_IM(cpu);
	object->i		= Q_Z80_STATE_I(cpu);
	object->iff2		= Q_Z80_STATE_IFF2(cpu);
	object->a_		= Q_Z80_STATE_A_(cpu);
	object->f_		= Q_Z80_STATE_F_(cpu);
	object->b_		= Q_Z80_STATE_B_(cpu);
	object->c_		= Q_Z80_STATE_C_(cpu);
	object->d_		= Q_Z80_STATE_D_(cpu);
	object->e_		= Q_Z80_STATE_E_(cpu);
	object->h_		= Q_Z80_STATE_H_(cpu);
	object->l_		= Q_Z80_STATE_L_(cpu);
	object->ix.value_uint16 = q_uint16_little_endian(Q_Z80_STATE_IX(cpu));
	object->iy.value_uint16 = q_uint16_little_endian(Q_Z80_STATE_IY(cpu));

	memcpy(object->memory, memory, 1024 * 64);
	}


/* ACH.c EOF */
