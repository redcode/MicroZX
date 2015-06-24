/* ACME - ALOutputPlayer.m
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#import "ALOutputPlayer.h"
#import "ALOutput.h"
#import <Q/functions/buffering/QRingBuffer.h>
#import <pthread.h>
#import <stdlib.h>
#import "system.h"

static ALCcontext*     context_		  = NULL;
static ALOutput**      activeOutputs_	  = NULL;
static qsize	       activeOutputCount_ = 0;
static pthread_mutex_t mutex_		  = PTHREAD_MUTEX_INITIALIZER;
static pthread_t       thread_;
static qboolean	       mustStop_	  = FALSE;


void *play(void *dummy)
	{
	quint64 frames_per_second = 60;
	quint64 frame_ticks	  = 1000000000 / frames_per_second;
	quint64 next_frame_tick   = q_ticks();
	quint64 delta;
	quint	maximum_frameskip = 5;
	quint	loops;
	qsize	index;

//	pthread_mutex_lock(&mutex_);
//	for (index = activeOutputCount_; index;) al_output_synchronize(activeOutputs_[--index]);
//	pthread_mutex_unlock(&mutex_);

	while (!mustStop_)
		{
		loops = 0;

		do	{
//			pthread_mutex_lock(&mutex_);
			index = activeOutputCount_;
			while (index) al_output_play(activeOutputs_[--index]);
			loops++;
//			pthread_mutex_unlock(&mutex_);
			}
		while ((next_frame_tick += frame_ticks) < q_ticks() && loops < maximum_frameskip);

		//----------------------------------------.
		// Schedule next iteration time and wait. |
		//----------------------------------------'
		if ((delta = next_frame_tick - q_ticks()) <= frame_ticks)
			q_wait(delta);

		//else printf("delta => %lu, next => %lu\n", delta, next_frame_tick);
		}

	return NULL;
	}


@implementation ALOutputPlayer


#	pragma mark - Accessors


	- (QRingBuffer *) buffer {return &_buffer;}


#	pragma mark - Overwritten

	- (id) init
		{
		if ((self = [super init]))
			{
			[self setSampleRate: 44100 frameSize: 882 slotCount: 4];
			}

		return self;
		}


	- (void) setSampleRate: (qsize) sampleRate
		 frameSize:	(qsize) frameSize
		 slotCount:	(quint) slotCount
		{
		q_ring_buffer_initialize
			(&_buffer,
			 _buffer.buffers = realloc(_buffer.buffers, sizeof(qint16) * frameSize * slotCount),
			 sizeof(qint16) * frameSize,
			 slotCount);
		}


	- (void) start
		{
		if (activeOutputCount_)
			{
			pthread_mutex_lock(&mutex_);
			al_output_initialize(&_ALOutput);
			al_output_set_input(&_ALOutput, &_buffer, 44100, 882);
			activeOutputs_ = realloc(activeOutputs_, sizeof(void *) * (activeOutputCount_ + 1));
			activeOutputs_[activeOutputCount_++] = &_ALOutput;
			pthread_mutex_unlock(&mutex_);
			}

		else	{
			*(activeOutputs_ = malloc(sizeof(void *))) = &_ALOutput;
			activeOutputCount_ = 1;

			const ALCchar *defaultDevice = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
			ALCdevice *soundDevice = alcOpenDevice(defaultDevice);

			context_ = alcCreateContext(soundDevice, NULL);
			alcMakeContextCurrent(context_);
			alcProcessContext(context_);

			al_output_initialize(&_ALOutput);
			al_output_set_input(&_ALOutput, &_buffer, 44100, 882);
			*(activeOutputs_   = malloc(sizeof(void *))) = &_ALOutput;
			activeOutputCount_ = 1;
			mustStop_	   = FALSE;

			pthread_attr_t threadAttributes;
			pthread_attr_init(&threadAttributes);
			pthread_create(&thread_, &threadAttributes, (void *(*)(void *))play, self);
			pthread_attr_destroy(&threadAttributes);
			}
		}


@end
