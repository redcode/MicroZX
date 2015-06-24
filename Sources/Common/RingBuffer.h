/* ACME - RingBuffer.h
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013 Manuel Sainz de Baranda y Goñi <manuel@redcodesoftware.com>
Released under the terms of the GNU General Public License v3. */

#ifndef __RingBuffer_H__
#define __RingBuffer_H__

#include <Q/types/base.h>
#include <libkern/OSAtomic.h>

typedef struct {
	void*			buffers;
	int32_t			buffer_size;
	int32_t			buffer_count;
	int32_t			head;
	int32_t			tail;
	volatile int32_t	fill_count;
} RingBuffer;

RingBuffer*	ring_buffer_new		(qsize	     buffer_size,
					 qsize	     buffer_count);

void		ring_buffer_destroy	(RingBuffer* object);

void		ring_buffer_increment	(RingBuffer* object);

void		ring_buffer_decrement	(RingBuffer* object);

void*		ring_buffer_head	(RingBuffer* object);

void*		ring_buffer_tail	(RingBuffer* object);

qboolean	ring_buffer_try_read	(RingBuffer* object,
					 void*	     output);

qboolean	ring_buffer_try_write	(RingBuffer* object,
					 void*	     input);

void		ring_buffer_read	(RingBuffer* object,
					 void*	     output);

void		ring_buffer_write	(RingBuffer* object,
					 void*	     input);

#endif /* __RingBuffer_H__ */
