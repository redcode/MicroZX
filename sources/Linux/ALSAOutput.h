/*     _________  ___
 _____ \_   /\  \/  / Linux/ALSAOutput.hpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef __mZX_Linux_ALSAOutput_H
#define __mZX_Linux_ALSAOutput_H

#include <Z/types/buffering.h>
#include <alsa/asoundlib.h>
#include <pthread.h>

typedef struct {
	pthread_t	  thread;
	snd_pcm_t*	  device;
	ZRingBuffer	  buffer;
	volatile zboolean must_stop;
	zboolean	  playing;
} ALSAOutput;

Z_C_SYMBOLS_BEGIN

void alsa_output_initialize (ALSAOutput* object);

void alsa_output_finalize   (ALSAOutput* object);

void alsa_output_start	    (ALSAOutput* object);

void alsa_output_stop	    (ALSAOutput* object);

Z_C_SYMBOLS_END

#endif // __mZX_Linux_ALSAOutput_H
