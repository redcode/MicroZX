/* ACME - RingBuffer.c
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013 Manuel Sainz de Baranda y Goñi <manuel@redcodesoftware.com>
Released under the terms of the GNU General Public License v3. */

#include "RingBuffer.h"
#include <stdlib.h>
#include <string.h>

#define cpu_relax() asm volatile("pause\n": : :"memory")


RingBuffer *ring_buffer_new(qsize buffer_count, qsize buffer_size)
	{
	RingBuffer *object = (void *)malloc(sizeof(RingBuffer));

	object->buffer_count	= (int32_t)buffer_count;
	object->buffer_size	= (int32_t)buffer_size;
	object->head		= 0;
	object->tail		= 0;
	object->buffers		= malloc(buffer_count * buffer_size);

	return object;
	}


void ring_buffer_destroy(RingBuffer *object)
	{
	free(object->buffers);
	free(object);
	}


void ring_buffer_increment(RingBuffer *object)
	{
	object->head = (object->head + 1) % object->buffer_count;
	__sync_add_and_fetch(&object->fill_count, 1);
	}


void ring_buffer_decrement(RingBuffer *object)
	{
	object->tail = (object->tail + 1) % object->buffer_count;
	__sync_sub_and_fetch(&object->fill_count, 1);
	}


void *ring_buffer_head(RingBuffer *object)
	{
	return object->buffer_count - object->fill_count
		? object->buffers + object->head * object->buffer_size
		: NULL;
	}


void *ring_buffer_tail(RingBuffer *object)
	{
	return object->fill_count
		? object->buffers + object->tail * object->buffer_size
		: NULL;
	}


qboolean ring_buffer_try_read(RingBuffer *object, void *output)
	{
	void *buffer = ring_buffer_tail(object);

	if (buffer == NULL) return FALSE;
	memcpy(output, buffer, object->buffer_size);
	ring_buffer_decrement(object);
	return TRUE;
	}


qboolean ring_buffer_try_write(RingBuffer *object, void *input)
	{
	void *buffer = ring_buffer_head(object);

	if (buffer == NULL) return FALSE;
	memcpy(buffer, input, object->buffer_size);
	ring_buffer_increment(object);
	return TRUE;
	}


void ring_buffer_read(RingBuffer *object, void *output)
	{while (!ring_buffer_try_read(object, output)) cpu_relax();}


void ring_buffer_write(RingBuffer *object, void *input)
	{while (!ring_buffer_try_write(object, input)) cpu_relax();}


/* RingBuffer.c EOF */
