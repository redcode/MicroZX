#ifndef mZX_SNA_h
#define mZX_SNA_h

#include <Q/formats/hardware state/machine/ZX Spectrum/SNA.h>
#include <Q/hardware/machine/platform/computer/ZX Spectrum.h>
#include <Q/hardware/CPU/architecture/Z80.h>

QStatus sna_v48k_test(QSNAv48K *object, qsize object_size);

qsize sna_v48k_encoding_size(
	QZXSpectrumState*	machine,
	QZ80State*		cpu,
	quint*			memory
);

void sna_v48k_decode(
	QSNAv48K*		object,
	QZXSpectrumState*	machine,
	QZ80State*		cpu,
	quint8*			memory
);

void sna_v48k_encode(
	QSNAv48K*		object,
	QZXSpectrumState*	machine,
	QZ80State*		cpu,
	quint8*			memory
);


#endif
