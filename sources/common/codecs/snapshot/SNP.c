/* SNP ZX Spectrum Snapshot Codec v1.0
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013 Manuel Sainz de Baranda y Goñi <manuel@redcodesofware.com>
Released under the terms of the GNU General Public License v3. */

#include <Z/formats/snapshot/machine/computer/ZX Spectrum/SNP.h>
#include <Z/hardware/machine/platform/computer/ZX Spectrum.h>
#include <Z/hardware/CPU/architecture/Z80.h>
#include <Z/functions/base/value.h>
#include <Z/keys/status.h>
#include <string.h>


static ZStatus test(ZSNP *object, zsize object_size)
	{
	if (object_size	!= sizeof(ZSNP)) return Z_ERROR_INVALID_SIZE;

	return Z_OK;
	}


static void decode(
	ZSNP*			object,
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
	Z_Z80_STATE_IRQ( cpu) = FALSE;
	Z_Z80_STATE_NMI( cpu) = FALSE;
	Z_Z80_STATE_IFF1(cpu) = object->iff1 & 1;
	Z_Z80_STATE_IFF2(cpu) = object->iff2 & 1;
	Z_Z80_STATE_EI(	 cpu) = FALSE;
	Z_Z80_STATE_IM(	 cpu) = object->im & 3;

	memcpy(memory + 1024 * 16, object->ram, 1024 * 48);
	}


static void encode(
	ZSNP*			object,
	ZZXSpectrumState*	machine,
	ZZ80State*		cpu,
	zuint8*			memory
)
	{
	memcpy(object->ram, memory + 1024 * 16, 1024 * 48);

	object->af.value_uint16  = z_uint16_little_endian(Z_Z80_STATE_AF(cpu));
	object->border_color	 = machine->ula_io.fields.border_color;
	object->reserved	 = 0;
	object->bc.value_uint16  = z_uint16_little_endian(Z_Z80_STATE_BC(cpu));
	object->de.value_uint16  = z_uint16_little_endian(Z_Z80_STATE_DE(cpu));
	object->hl.value_uint16  = z_uint16_little_endian(Z_Z80_STATE_HL(cpu));
	object->pc		 = z_uint16_little_endian(Z_Z80_STATE_PC(cpu));
	object->sp		 = z_uint16_little_endian(Z_Z80_STATE_SP(cpu));
	object->ix.value_uint16  = z_uint16_little_endian(Z_Z80_STATE_IX(cpu));
	object->iy.value_uint16  = z_uint16_little_endian(Z_Z80_STATE_IY(cpu));
	object->iff1		 = Z_Z80_STATE_IFF1(cpu);
	object->iff1		 = Z_Z80_STATE_IFF2(cpu);
	object->im		 = Z_Z80_STATE_IM(cpu);
	object->r		 = Z_Z80_STATE_R(cpu);
	object->i		 = Z_Z80_STATE_I(cpu);
	object->af_.value_uint16 = z_uint16_little_endian(Z_Z80_STATE_AF_(cpu));
	object->bc_.value_uint16 = z_uint16_little_endian(Z_Z80_STATE_BC_(cpu));
	object->de_.value_uint16 = z_uint16_little_endian(Z_Z80_STATE_DE_(cpu));
	object->hl_.value_uint16 = z_uint16_little_endian(Z_Z80_STATE_HL_(cpu));
	}


/* SNP.c EOF */
