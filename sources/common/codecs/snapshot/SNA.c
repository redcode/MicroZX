/* SNA ZX Spectrum Snapshot Codec v1.0
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013 Manuel Sainz de Baranda y Goñi <manuel@redcodesofware.com>
Released under the terms of the GNU General Public License v3. */

#include <Z/formats/snapshot/machine/ZX Spectrum/SNA.h>
#include <Z/hardware/machine/platform/computer/ZX Spectrum.h>
#include <Z/hardware/CPU/architecture/Z80.h>
#include <Z/functions/base/value.h>
#include <Z/keys/status.h>
#include <string.h>


ZStatus sna_v48k_test(ZSNAv48K *object, zsize object_size)
	{
	if (object_size	!= sizeof(ZSNAv48K)) return Z_ERROR_INVALID_SIZE;

	return (object->im < 3 && object->border_color < 8)
		? Z_OK : Z_ERROR_INVALID_FORMAT;
	}

zsize sna_v48k_encoding_size(
	ZZXSpectrumState*	machine,
	ZZ80State*		cpu,
	zuint*			memory
)
	{return sizeof(ZSNAv48K);}


void sna_v48k_decode(
	ZSNAv48K*		object,
	ZZXSpectrumState*	machine,
	ZZ80State*		cpu,
	zuint8*			memory
)
	{
	machine->keyboard.value_uint64 = Z_UINT64(0xFFFFFFFFFFFFFFFF);
	machine->flash		       = FALSE;
	machine->ula_io.value	       = object->border_color & 7;

	Z_Z80_STATE_PC(	 cpu) = Z_SNA_V48K_PC_VALUE;
	Z_Z80_STATE_SP(	 cpu) = z_uint16_little_endian(object->sp);
	Z_Z80_STATE_AF(	 cpu) = z_uint16_little_endian(object->af.value_uint16);
	Z_Z80_STATE_BC(	 cpu) = z_uint16_little_endian(object->bc.value_uint16);
	Z_Z80_STATE_DE(	 cpu) = z_uint16_little_endian(object->de.value_uint16);
	Z_Z80_STATE_HL(	 cpu) = z_uint16_little_endian(object->hl.value_uint16);
	Z_Z80_STATE_IX(	 cpu) = z_uint16_little_endian(object->ix.value_uint16);
	Z_Z80_STATE_IY(	 cpu) = z_uint16_little_endian(object->iy.value_uint16);
	Z_Z80_STATE_AF_( cpu) = z_uint16_little_endian(object->af_.value_uint16);
	Z_Z80_STATE_BC_( cpu) = z_uint16_little_endian(object->bc_.value_uint16);
	Z_Z80_STATE_DE_( cpu) = z_uint16_little_endian(object->de_.value_uint16);
	Z_Z80_STATE_HL_( cpu) = z_uint16_little_endian(object->hl_.value_uint16);
	Z_Z80_STATE_I(	 cpu) = object->i;
	Z_Z80_STATE_R(	 cpu) = object->r;
	Z_Z80_STATE_HALT(cpu) = FALSE;
	Z_Z80_STATE_IRQ( cpu) = FALSE;
	Z_Z80_STATE_NMI( cpu) = FALSE;
	Z_Z80_STATE_IFF1(cpu) =
	Z_Z80_STATE_IFF2(cpu) = object->interrupt.iff2;
	Z_Z80_STATE_EI(	 cpu) = FALSE;
	Z_Z80_STATE_IM(	 cpu) = object->im;

	memcpy(memory + 1024 * 16, object->ram, 1024 * 48);
	}


void sna_v48k_encode(
	ZSNAv48K*		object,
	ZZXSpectrumState*	machine,
	ZZ80State*		cpu,
	zuint8*			memory
)
	{
	object->i		  = Z_Z80_STATE_I(cpu);
	object->hl_.value_uint16  = z_uint16_little_endian(Z_Z80_STATE_HL_(cpu));
	object->de_.value_uint16  = z_uint16_little_endian(Z_Z80_STATE_DE_(cpu));
	object->bc_.value_uint16  = z_uint16_little_endian(Z_Z80_STATE_BC_(cpu));
	object->af_.value_uint16  = z_uint16_little_endian(Z_Z80_STATE_AF_(cpu));
	object->hl.value_uint16	  = z_uint16_little_endian(Z_Z80_STATE_HL (cpu));
	object->de.value_uint16	  = z_uint16_little_endian(Z_Z80_STATE_DE (cpu));
	object->bc.value_uint16	  = z_uint16_little_endian(Z_Z80_STATE_BC (cpu));
	object->iy.value_uint16	  = z_uint16_little_endian(Z_Z80_STATE_IY (cpu));
	object->ix.value_uint16	  = z_uint16_little_endian(Z_Z80_STATE_IX (cpu));
	object->interrupt.unused_0 = 0;
	object->interrupt.iff2	  = Z_Z80_STATE_IFF2(cpu);
	object->interrupt.unused_1 = 0;
	object->r		  = Z_Z80_STATE_R(cpu);
	object->af.value_uint16	  = z_uint16_little_endian(Z_Z80_STATE_AF(cpu));
	object->sp		  = z_uint16_little_endian(Z_Z80_STATE_SP(cpu));
	object->im		  = Z_Z80_STATE_IM(cpu);
	object->border_color	  = machine->ula_io.fields.border_color;

	memcpy(object->ram, memory + 1024 * 16, 1024 * 48);
	}


/* SNA.c EOF */
