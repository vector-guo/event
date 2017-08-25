#include"os_epoll.h"
#include"os_log.h"
#include"os_thread.h"
#include"os_list.h"
//#include"os_hlist.h"
#include<stddef.h>


#define EVENT_HASH_NUM    40960
#define MAX_EVENT_POLL 102400

int epfd;
static struct epoll_event ev;
static os_locker_t epoll_locker;
static os_event_t event_poll[MAX_EVENT_POLL];


static struct hlist_head event_table[EVENT_HASH_NUM];		//event的哈希表
static os_locker_t event_table_locker;


int event_hash(int fd)
{
	return fd % EVENT_HASH_NUM;
}
int event_same(os_event_t * first,os_event_t * two)
{
	if(first->fd == two->fd)
	{
		return 0;
	}
	else
	{
		return -1;
	}
	return first->fd == two->fd;
}

int os_epoll_init()
{

	printf("os_epoll_init\n");
    os_mutex_init(&epoll_locker);
    
    epfd = epoll_create(1000); //��linux2.6֮��1000������
    if(epfd < 0)
    {
        os_log_error("epoll create err");
        return -1;
    }

   int i;
    for(i=0;i<MAX_EVENT_POLL;i++)
    {
        os_mutex_init(&event_poll[i].locker);
		os_mutex_init(&event_poll[i].event_locker);
		
        event_poll[i].active = 0;
    }

	os_mutex_init(&event_table_locker);
	for(i=0;i<EVENT_HASH_NUM;i++)
	{
		INIT_HLIST_HEAD(&event_table[i]);
	}
	
    return 0;    
}

int os_add_event(os_event_t * event)
{

        int op;
        int which = event->which_op;

		os_event_t * event_temp,*n;
		int event_exist = 0;
		int key_temp = event_hash(event->fd);
		os_event_t *   event_index;
		hlist_for_each_entry_safe(event_temp,n,&event_table[key_temp],node)
		{
			if(!event_same(event_temp,event))
			{
				event_exist = 1;
				break ;
			}
		}

		//printf("event_exist: %d\n",event_exist);
		if(event_exist)
		{
			op = EPOLL_CTL_MOD;
			event_index = event_temp;
			event->active = event_temp->active;
			if((which | EPOLLET) == event->active)
			{
				printf("already exist\n");
				return 0;
			}
			//printf("exist active: %x\n",event->active);
		}
		else
		{
			//printf("add new fd\n");
			op = EPOLL_CTL_ADD;
			event->active = EPOLLET;
			
		        int i;
		        for(i=0;i<MAX_EVENT_POLL;i++)
		        {
		            if(!os_mutex_try_lock(&event_poll[i].locker))
		            {
		                
						event_poll[i].active    =  event->active;
						event_poll[i].fd        =  event->fd;
						event_poll[i].udp       =  event->udp;
						event_poll[i].doevent   =  event->doevent;
						event_poll[i].tcp       =  event->tcp; 
						event_poll[i].__user    =  event->__user;
						memcpy(&event_poll[i].list,&event->list,sizeof(struct list_head));
						event_index = &event_poll[i];
						
						//将事件排列到哈希表
						int key;
						INIT_HLIST_NODE(&event_poll[i].node);
						key = event_hash(event_poll[i].fd);
						if(key >= EVENT_HASH_NUM)
						{
							return -1;
						}
						
						os_mutex_lock(&event_table_locker);
						hlist_add_head(&event_poll[i].node, &event_table[key]);
						os_mutex_unlock(&event_table_locker);
						
		                break;
		            }
		        }
		}

        struct epoll_event ev;
        if (which & EV_RE)
        {
           
           event_index->active |= EPOLLIN;
           
           //ev.data.fd = event->fd;
           ev.events= event_index->active;
           ev.data.ptr = event_index;

           printf("add read envet %x  %x fd %d \n",ev.events,EPOLLET | EPOLLIN,event->fd);

        }
        if (which & EV_WR)
        {
           event_index->active |= EPOLLOUT;
           
           //ev.data.fd = event->fd;
           ev.events= event_index->active;
           (ev.data.ptr) = event_index;

           //printf("add write envet %x  %x fd %d\n",ev.events,EPOLLET | EPOLLOUT,event->fd);
        }

		//printf("event op : %x\n",ev.events);
		epoll_ctl(epfd,op,event->fd,&ev);
        return 0;
        
}

int os_del_event(os_event_t * pevent)
{
	struct epoll_event epv = {0, {0}};    
    epoll_ctl(epfd, EPOLL_CTL_DEL, pevent->fd, &epv);

	os_mutex_lock(&event_table_locker);
	hlist_del(&pevent->node);			 	//从事件哈希表中删除
	os_mutex_unlock(&event_table_locker);
	
	os_mutex_unlock(&pevent->locker);		//解除事件占用锁，表示event_poll[x]又可以使用了
}

int os_event_wait(os_event_t * event)
{

   struct epoll_event events[1000];
   
   

#if MUTEX_EPOLL
   int ret = os_mutex_try_lock(&epoll_locker);
   if(ret == -1)
   {
        return -1;
   }
#endif
   int cnt = epoll_wait(epfd,events,1000,5000);
    
   if(cnt <=0 )  //��ʱ���߳���
   {

       //os_log_error("epoll wait err %d",cnt);
       
#if    MUTEX_EPOLL
        os_mutex_unlock(&epoll_locker);
#endif
        return 0;
   }
   INIT_LIST_HEAD(&event->list);

   //printf("epoll return %d\n",cnt);
   int i;
   for(i=0;i<cnt;i++)
   {
        
        /*int subs = (events[i].data.ptr);
        if(subs > 1024)
        {
            continue;;
        }*/
        os_event_t * event_subs = ( os_event_t *)(events[i].data.ptr);

		//printf("............%x\n",events[i].events);
		if(events[i].events & EPOLLRDHUP)
		{
			printf("............\n");
		}
        if(events[i].events & EPOLLIN)
        {
            event_subs->which_op |= EV_RE;
        }
        if(events[i].events & EPOLLOUT)
        {
            event_subs->which_op |= EV_WR;
        }
		if(events[i].events & EPOLLERR)
		{
			printf("epoll wait err!\n");
		}
		//printf("event epool: %d\n",event_subs->fd);
		if(!os_mutex_try_lock(&event_subs->event_locker))
		{
			list_add_tail(&event_subs->list, &event->list);
		}
		else   //此处看一下是否重新生成一个事件，触发读.实际上，如果是ET模式，while(1)一直读，不需要再触发，但是粘包。
		{
			/*int i;
	        for(i=0;i<1024;i++)
	        {
	            if(!os_mutex_try_lock(&event_poll[i].locker))
	            {
	            	printf("event %d\n",i);
	                memcpy(&event_poll[i],&event_poll[subs],sizeof(os_event_t));
					list_add_tail(&event_poll[i].list, &event->list);
	                break;
	            }
	        }*/
			
		}
        
        
   }
   
#if    MUTEX_EPOLL   
   os_mutex_unlock(&epoll_locker);
#endif

   return cnt;
   
}

