/* ACME - ALOutput.h
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#ifndef __ACME_ALOutput_H__
#define __ACME_ALOutput_H__

#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <Q/types/buffering.h>

typedef struct {
	ALuint	     source;
	QRingBuffer* input_buffer;
	ALsizei	     sample_rate;
	ALsizei	     frame_size;
	ALuint*	     buffers;
	qsize	     unused_buffer_count;
} ALOutput;

#define AL_OUTPUT(p) ((ALOutput *)(p))

#ifdef __cplusplus
extern "C" {
#endif

void al_output_initialize  (ALOutput*	 object);

void al_output_finalize	   (ALOutput*	 object);

void al_output_set_input   (ALOutput*	 object,
			    QRingBuffer* buffer,
			    qsize	 sample_rate,
			    qsize	 frame_size);

void al_output_synchronize (ALOutput*	object);

void al_output_play	   (ALOutput*	object);

#ifdef __cplusplus
}
#endif

#endif /* __ACME_ALOutput_H__ */
