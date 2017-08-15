#ifndef __OS__EPOLL__HH
#define __OS__EPOLL__HH

#include"os_event.h"
#include <sys/epoll.h>

int os_epoll_init();
int os_add_event(os_event_t * event);
int os_remove_event(os_event_t * event);
int os_event_wait();
#endif