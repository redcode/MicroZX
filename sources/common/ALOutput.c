/* ACME - ALOutput.c
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#include "ALOutput.h"
#include <Q/functions/buffering/QRingBuffer.h>
#include <stdlib.h>


void al_output_initialize(ALOutput *object)
	{
	object->buffers = NULL;
	alGenSources(1, &object->source);
	alSource3f(object->source, AL_POSITION, 0, 0, 0);
	alSource3f(object->source, AL_VELOCITY, 0, 0, 0);
	alSourcei (object->source, AL_LOOPING, AL_FALSE);
	}


void al_output_finalize(ALOutput *object)
	{
	ALint count;
	ALuint buffer;

	alSourceStop(object->source);
	alGetSourcei(object->source, AL_BUFFERS_PROCESSED, &count);

	while (count--) alSourceUnqueueBuffers(object->source, 1, &buffer);
	alDeleteBuffers((ALsizei)object->input_buffer->buffer_count, object->buffers);
	alDeleteSources(1, &object->source);
	}


void al_output_set_input(
	ALOutput*    object,
	QRingBuffer* buffer,
	qsize	     sample_rate,
	qsize	     frame_size
)
	{
	object->input_buffer = buffer;
	object->sample_rate  = (ALsizei)sample_rate;
	object->frame_size   = (ALsizei)frame_size;

	alGenBuffers
		((ALsizei)((object->unused_buffer_count = buffer->buffer_count)),
		 object->buffers = realloc(object->buffers, sizeof(ALuint) * buffer->buffer_count));
	}


void al_output_synchronize(ALOutput *object)
	{while (q_ring_buffer_try_consume(object->input_buffer) != NULL);}

#include <stdio.h>

void al_output_play(ALOutput *object)
	{
	ALenum state;
	ALint count;
	ALuint buffer;
	void *frame;
	ALsizei frame_size = object->frame_size * sizeof(qint16);

	alGetSourcei(object->source, AL_BUFFERS_PROCESSED, &count);

	while ((frame = q_ring_buffer_try_consume(object->input_buffer)) != NULL)
		{
		if (count--) alSourceUnqueueBuffers(object->source, 1, &buffer);

		else if (object->unused_buffer_count)
			buffer = object->buffers[--object->unused_buffer_count];

		else	{
		//	printf("buffer overflow\n");
			}

		alBufferData(buffer, AL_FORMAT_MONO16, frame, frame_size, object->sample_rate);
		alSourceQueueBuffers(object->source, 1, &buffer);
		}

	alGetSourcei(object->source, AL_SOURCE_STATE, &state);
	if (state != AL_PLAYING) alSourcePlay(object->source);
	}


/* ALOutput.c EOF */
