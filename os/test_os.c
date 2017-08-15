#include"os_log.h"
#include"os_time.h"
#include"os_queue.h"
#include"os_thread.h"
#include"os_event.h"
#include"os_modelA.h"
#include"os_socket.h"
//linux kernel list, just copy
#include "os_list.h"
#include"os_heap.h"
#include <stddef.h>  //for typeof()


#include<fcntl.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
//extern int epfd;




void * event_call_back(void * parm);

void * event_call_accept(void * parm)
{
	
	os_event_t * pevent = (os_event_t *)parm;
	
	while(1)
	{
		
			os_socket_tcp_t * tcp =   os_accept_tcp(pevent->tcp);
			if(tcp == NULL)
			{
				break;
			}
			os_event_t event;
		    event.fd = tcp->socket_fd;
		    event.which_op = EV_RE;
			event.tcp = tcp;
			event.doevent = event_call_back;
			
		    os_add_event(&event);

			printf("tcp new fd : %d\n",event.fd);
	}
	
}

void * event_call_write(void * parm)
{
	printf("event write call back\n");
}

void * event_call_back(void * parm)
{

	
	os_event_t * event = (os_event_t *)parm;


	if(event->which_op & EV_RE)
	{
		char buf[1024];
		int len  = os_read_socket(event->fd,  buf, 1024);
		printf("recv  %d bytes\n",len);
		if(len == 0)
		{
			os_del_event(event);
			os_close_socket(event->fd);
			
			printf("close sock : %d\n",event->fd);
		}
	}
	if(event->which_op & EV_WR)
	{
		printf("event write call back\n");
	}

}

int main(int argc,char ** argv)
{

    os_log_init(NULL);

/*
    //test log
    os_log_error("test log error");
    os_log_error("%s %d","test log error",1);
    os_log_error("%s %d","test log error",2);
    os_log_error("%s %d","test log error",3);
    
    dup2(100, fileno(stderr));      //create dup2 err for test os_log_error() func
    os_log_error("%s","dup err");

    os_log_info("test log info");
    os_log_info("%s %d","test log info",1);
    os_log_info("%s %d","test log info",2);
    os_log_info("%s %d","test log info",3);


    //test time
    os_time_t ms = os_time_ms();
    os_log_info("now : %lld",ms);


    //test queue
    os_queue_t queue,queue_head;
    os_queue_t queue1,queue2,queue3,queue4,queue5,queue_add;


    os_queue_init(&queue);
    os_queue_insert_head(&queue, &queue_head);
    os_queue_insert_head(&queue, &queue1);
    os_queue_insert_head(&queue, &queue2);
    os_queue_insert_head(&queue, &queue3);
    os_queue_insert_head(&queue, &queue4);
    os_queue_insert_head(&queue, &queue5);


    os_queue_t * pqueue = os_queue_head(&queue);
    if(pqueue == &queue_head)
    {
        os_log_info("queue head is queue_head");
    }
    else if(pqueue == &queue1)
    {
        os_log_info("queue head is queue1");
    }
    else if(pqueue == &queue5)
    {
        os_log_info("queue head is queue5");
    }
    
    os_queue_remove(&queue3);
*/

 
	os_epoll_init();		//epoll init
	os_modelA_init(4);		//use  model A


	os_socket_udp_t * pepoll = os_create_udp(5683, 0);
    os_event_t event;
    event.fd = pepoll->socket_fd;
    event.which_op = EV_RE;
	event.udp = pepoll;
	event.doevent = event_call_back;
	
    
    os_add_event(&event);


	 pepoll = os_create_udp(9001, 0);
	event.fd = pepoll->socket_fd;
    event.which_op = EV_RE;
	event.udp = pepoll;
	event.doevent = event_call_back;
	
    
    os_add_event(&event);
	/*os_event_t event_temp;
    event_temp.fd = pepoll->socket_fd;
    event_temp.which_op = EV_WR;
	event_temp.udp =pepoll;
	event_temp.doevent = event_call_back;
	
    os_add_event(&event_temp);*/

	os_socket_tcp_t * ptcp   =   os_create_tcp(9000, 0);
	os_tcp_listen(ptcp);
	//os_event_t event;
    event.fd = ptcp->socket_fd;
    event.which_op = EV_RE;
	event.tcp = ptcp;
	event.doevent = event_call_accept;
    os_add_event(&event);

	
	os_heap_init();
	os_heap_insert(10);
	os_heap_insert(5);
	os_heap_insert(200);
	os_heap_insert(40);

	os_heap_insert(3);
	os_heap_insert(2);
	os_heap_insert(1);
	os_heap_insert(2000);
	
	os_heap_insert(30);
	os_heap_insert(500);
	os_heap_insert(8000);
	os_heap_insert(1024);

	os_heap_insert(6);
	os_heap_insert(19);
	os_heap_insert(20);
	os_heap_insert(3000);
	os_heap_debug();
	os_heap_extract(2);
	os_heap_extract(0);
	os_heap_insert(1);
	os_heap_debug();
    sleep(1000);
    

    return 0;
}
