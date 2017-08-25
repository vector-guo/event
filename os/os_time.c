#include"os_time.h"
#include<sys/time.h>
os_time_t os_time_ms()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    

    unsigned long long curTime;
    curTime = t.tv_sec;
    curTime *= 1000;                // sec -> msec
    curTime += t.tv_usec / 1000;    // usec -> msec
    return curTime;
}