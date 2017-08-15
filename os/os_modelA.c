#include"os_modelA.h"
#include"os_thread.h"
#include"os_list.h"
#include"os_event.h"

struct thread_task_pool
{
    int threadid;
    os_event_t event;			//线程中事件 列表
    
    int task;					//该线程下还有多少任务
    os_locker_t add_remove_lock;//  该工作线程下 事件添加、删除锁
};


struct thread_task_pool ** pool;
static int work_thread;
static unsigned int total_task;  //һ�������˶��ٸ�task


void * modelA_work_thread(void * param)
{
    int i;
    struct thread_task_pool * ptask_pool = (struct thread_task_pool *)param;
	os_event_t * event = &ptask_pool->event;
	

    int test=0;
    while(1)
    {
        
	    os_event_t * event_temp , *n;
	    list_for_each_entry_safe(event_temp,n,&event->list,list)
	    {
				if(event_temp->doevent)
				{
					*(event_temp->doevent)(event_temp);
				}
				
				os_mutex_lock(&ptask_pool->add_remove_lock);
				list_del(&event_temp->list);
				event_temp->which_op = 0;
				os_mutex_unlock(&ptask_pool->add_remove_lock);

				os_mutex_unlock(&event_temp->event_locker);			//解除事件锁。表示事件已经处理完毕，新事件可以再加进事件列表
				//printf("recv: %s len %d\n",buf,len);
			
		}
		os_yield();
    }
}

void * modelA_epoll_thread(void * param)
{
    os_event_t event_get;
    
    int event_num; 
    int tasks = 0;
        
    while(1)
    {
        
        event_num =  os_event_wait(&event_get);
        if(event_num <=0)
        {
            continue;
        }
       
        os_event_t * event_temp , *n;
        list_for_each_entry_safe(event_temp,n,&event_get.list,list)   
        {
              tasks++;
             
             int  pool_index = tasks %  work_thread;

             list_del(&event_temp->list);  //del from epoll_wait list
			//printf("event wait: %d\n",event_temp->fd);

             os_mutex_lock(&pool[pool_index]->add_remove_lock);
             list_add(&event_temp->list,&pool[pool_index]->event.list); //���Ӧ���߳�����������
             pool[pool_index]->task ++;
             os_mutex_unlock(&pool[pool_index]->add_remove_lock);
                
        }
    }
}

int os_modelA_init(int work_thread_num)
{

    work_thread = work_thread_num;
    total_task  = 0;


    pool = (struct thread_task_pool**)malloc(sizeof(struct thread_task_pool*) * work_thread_num);
        

    int i;
    for(i=0;i<work_thread_num;i++)
    {

        pool[i] = (struct thread_task_pool*)malloc(sizeof(struct thread_task_pool));
        
        INIT_LIST_HEAD(&pool[i]->event.list);
        pool[i]->task = 0;
		pool[i]->threadid = i;
        os_mutex_init(&pool[i]->add_remove_lock);


        os_thread_create(modelA_work_thread,pool[i]);
        
    }

    os_thread_create(modelA_epoll_thread,NULL);
    return 0;
}
