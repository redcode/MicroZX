#ifndef mZX_system_h
#define mZX_system_h

#include <Q/types/base.h>
#include <pthread/pthread.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
typedef QSystemThread

QStatus q_system_thread_create(QSystemThread *object,)
			pthread_attr_t threadAttributes;
			pthread_attr_init(&threadAttributes);
			pthread_create(&_thread, &threadAttributes, (void *(*)(void *))EmulationMain, self);
			pthread_attr_destroy(&threadAttributes);



void*	q_system_thread_join(QSystemThread *thread);
*/

void	q_initialize_ticks  (void);

quint64 q_ticks		    (void);

void	q_wait		    (quint64 time);

#ifdef __cplusplus
}
#endif

#endif
