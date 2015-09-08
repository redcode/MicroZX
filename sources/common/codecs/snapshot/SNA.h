#ifndef mZX_SNA_h
#define mZX_SNA_h

#include <Z/formats/snapshot/machine/ZX Spectrum/SNA.h>
#include <Z/hardware/machine/platform/computer/ZX Spectrum.h>
#include <Z/hardware/CPU/architecture/Z80.h>

ZStatus sna_v48k_test(ZSNAv48K *object, zsize object_size);

zsize sna_v48k_encoding_size(
	ZZXSpectrumState*	machine,
	ZZ80State*		cpu,
	zuint*			memory
);

void sna_v48k_decode(
	ZSNAv48K*		object,
	ZZXSpectrumState*	machine,
	ZZ80State*		cpu,
	zuint8*			memory
);

void sna_v48k_encode(
	ZSNAv48K*		object,
	ZZXSpectrumState*	machine,
	ZZ80State*		cpu,
	zuint8*			memory
);


#endif
