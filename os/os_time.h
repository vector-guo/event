#ifndef __OS__TIME__h
#define __OS__TIME__h
#include<string.h>

typedef unsigned long long os_time_t;

os_time_t os_time_ms(void);
os_time_t os_time_delay(int ms);


#endif
