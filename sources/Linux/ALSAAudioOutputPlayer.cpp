/*     _________  ___
 _____ \_   /\  \/  / Linux/ALSAAudioOutputPlayer.c
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include "system.h"
#include <stdlib.h>
#include "ALSAAudioOutputPlayer.hpp"

#define FRAME_SIZE Z_INT16_SIZE * 882


void ALSAAudioOutputPlayer::main()
	{
	while (_buffer.fill_count < 2)
		z_wait(1000000000 / 50 + 1000000000 / 100);

	while (!_must_stop)
		{
		if (_buffer.fill_count < 2)
			snd_pcm_writei(_device, _buffer.consumption_buffer(), 882);

		else	{
			while (_buffer.fill_count > 3)
				_buffer.try_consume();

			snd_pcm_writei(_device, _buffer.consumption_buffer(), 882);
			_buffer.try_consume();
			}
		}
	}


ALSAAudioOutputPlayer::ALSAAudioOutputPlayer() : _device(NULL), playing(false)
	{
	void *frames = calloc(1, FRAME_SIZE * 4);
	_buffer.initialize(frames, Z_INT16_SIZE * 882, 4);
	}


ALSAAudioOutputPlayer::~ALSAAudioOutputPlayer()
	{free(_buffer.buffers);}


void ALSAAudioOutputPlayer::start()
	{
	if (!playing)
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
		snd_pcm_hw_params_any(device, (snd_pcm_hw_params_t *)parameters);
		snd_pcm_hw_params_set_access(device, (snd_pcm_hw_params_t *)parameters, SND_PCM_ACCESS_RW_INTERLEAVED);
		snd_pcm_hw_params_set_format(device, (snd_pcm_hw_params_t *)parameters, SND_PCM_FORMAT_S16_LE);
		snd_pcm_hw_params_set_rate_near(device, (snd_pcm_hw_params_t *)parameters, &rate, (int *)NULL);
		snd_pcm_hw_params_set_channels(device, (snd_pcm_hw_params_t *)parameters, 1);
		snd_pcm_hw_params_set_buffer_size_near(device, (snd_pcm_hw_params_t *)parameters, &size);
		size = 882 * 2;
		snd_pcm_hw_params_set_period_size(device, (snd_pcm_hw_params_t *)parameters, size, 0);
		snd_pcm_hw_params(device, (snd_pcm_hw_params_t *)parameters);
		snd_pcm_hw_params_free((snd_pcm_hw_params_t *)parameters);

		/*----------------------------------------.
		| Configure device's software parameters. |
		'----------------------------------------*/
		snd_pcm_sw_params_malloc((snd_pcm_sw_params_t **)&parameters);
		snd_pcm_sw_params_current(device, (snd_pcm_sw_params_t *)parameters);
		snd_pcm_sw_params_set_start_threshold(device, (snd_pcm_sw_params_t *)parameters, 882);
		snd_pcm_sw_params_set_avail_min(device, (snd_pcm_sw_params_t *)parameters, 882);
		//snd_pcm_sw_params_set_avail_max(device, parameters, size);
		snd_pcm_sw_params(device, (snd_pcm_sw_params_t *)parameters);
		snd_pcm_sw_params_free((snd_pcm_sw_params_t *)parameters);

		snd_pcm_prepare(device);

		_device = device;
		_must_stop = FALSE;
		playing = TRUE;
		_thread = std::thread(&ALSAAudioOutputPlayer::main, this);
		}
	}


void ALSAAudioOutputPlayer::stop()
	{
	if (playing)
		{
		_must_stop = TRUE;
		_thread.join();
		playing = FALSE;
		snd_pcm_close(_device);
		}
	}
