#ifndef __OS__THREAD__H
#define __OS__THREAD__H
#include<pthread.h>

typedef pthread_mutex_t os_locker_t;

int  os_thread_create(void* (*func)(void * parg),void * param) ;
int os_thread_join();
int os_thread_detach();
int os_thread_create_key();

int os_mutex_init(os_locker_t * locker);
int os_mutex_lock(os_locker_t * locker);
int os_mutex_try_lock(os_locker_t * locker);
int os_mutex_unlock(os_locker_t * locker);

int os_yield();

#endif