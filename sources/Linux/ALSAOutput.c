/*     _________  ___
 _____ \_   /\  \/  / Linux/ALSAOutput.c
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include <Z/functions/buffering/ZRingBuffer.h>
#include "system.h"
#include <stdlib.h>
#include "ALSAOutput.h"

#define FRAME_SIZE Z_INT16_SIZE * 882


static void *play(ALSAOutput *object)
	{
	while (object->buffer.fill_count < 2)
		{
		z_wait(1000000000 / 50 + 1000000000 / 100);
		}

	while (!object->must_stop)
		{
		if (object->buffer.fill_count < 2)
			snd_pcm_writei(object->device, z_ring_buffer_consumption_buffer(&object->buffer), 882);

		else	{
			while (object->buffer.fill_count > 3)
				z_ring_buffer_try_consume(&object->buffer);

			snd_pcm_writei(object->device, z_ring_buffer_consumption_buffer(&object->buffer), 882);
			z_ring_buffer_try_consume(&object->buffer);
			}
		}

	return NULL;
	}


void alsa_output_initialize(ALSAOutput *object)
	{
	void *frames = calloc(1, FRAME_SIZE * 4);
	z_ring_buffer_initialize(&object->buffer, frames, Z_INT16_SIZE * 882, 4);
	object->device = NULL;
	object->playing = FALSE;
	}


void alsa_output_finalize(ALSAOutput *object)
	{
	free(object->buffer.buffers);
	}


void alsa_output_start(ALSAOutput *object)
	{
	if (!object->playing)
		{
		void *parameters;
		snd_pcm_t *device;
		unsigned int rate = 44100;
		snd_pcm_uframes_t size = 882 * 2 * 2;

		int error = snd_pcm_open(&device, "default", SND_PCM_STREAM_PLAYBACK, 0);

		/*----------------------------------------.
		| Configure device's hardware parameters. |
		'----------------------------------------*/
		snd_pcm_hw_params_malloc((snd_pcm_hw_params_t **)&parameters);
		snd_pcm_hw_params_any(device, parameters);
		snd_pcm_hw_params_set_access(device, parameters, SND_PCM_ACCESS_RW_INTERLEAVED);
		snd_pcm_hw_params_set_format(device, parameters, SND_PCM_FORMAT_S16_LE);
		snd_pcm_hw_params_set_rate_near(device, parameters, &rate, (int *)NULL);
		snd_pcm_hw_params_set_channels(device, parameters, 1);
		snd_pcm_hw_params_set_buffer_size_near(device, parameters, &size);
		size = 882 * 2;
		snd_pcm_hw_params_set_period_size(device, parameters, size, 0);
		snd_pcm_hw_params(device, parameters);
		snd_pcm_hw_params_free(parameters);

		/*----------------------------------------.
		| Configure device's software parameters. |
		'----------------------------------------*/
		snd_pcm_sw_params_malloc((snd_pcm_sw_params_t **)&parameters);
		snd_pcm_sw_params_current(device, parameters);
		snd_pcm_sw_params_set_start_threshold(device, parameters, 882);
		snd_pcm_sw_params_set_avail_min(device, parameters, 882);
		//snd_pcm_sw_params_set_avail_max(device, parameters, size);
		snd_pcm_sw_params(device, parameters);
		snd_pcm_sw_params_free(parameters);

		snd_pcm_prepare(device);

		object->device = device;
		object->must_stop = FALSE;
		object->playing = TRUE;

		pthread_attr_t attributes;
		pthread_attr_init(&attributes);
		pthread_create(&object->thread, &attributes, (void *(*)(void *))play, object);
		pthread_attr_destroy(&attributes);
		}
	}


void alsa_output_stop(ALSAOutput *object)
	{
	if (object->playing)
		{
		object->must_stop = TRUE;
		pthread_join(object->thread, NULL);
		object->playing = FALSE;
		snd_pcm_close(object->device);
		}
	}
