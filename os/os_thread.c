#include"os_thread.h"

int  os_thread_create(void* (*func)(void * parg),void * param)   //创建分离线程
{
    pthread_t thread_id;
    
    if(pthread_create(&thread_id,NULL,func,param) < 0)
    {
    	printf("create thread err\n");
        return -1;
    }
    
    pthread_detach(pthread_self());                 
    
    return 0;
}

int os_mutex_init(os_locker_t * locker)
{
    if(locker == NULL)
    {
        return -1;
    }
    
    return pthread_mutex_init(locker, NULL);
}

int os_mutex_lock(os_locker_t * locker)
{
    if(locker == NULL)
    {
        return -1;
    }

    return pthread_mutex_lock(locker);

}

int os_mutex_try_lock(os_locker_t * locker)
{
    if(locker == NULL)
    {
        return -1;
    }
    
    int ret = pthread_mutex_trylock(locker);
    if(ret == 0)
    {
        return 0; //获得锁
    }
    else
    {
        return -1; 
    }
}

int os_mutex_unlock(os_locker_t * locker)
{
    if(locker == NULL)
    {
        return -1;
    }
    
    return pthread_mutex_unlock(locker);
    
}

int os_yield()
{
    sched_yield();
}