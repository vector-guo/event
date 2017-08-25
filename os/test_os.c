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
#include <signal.h>

//extern int epfd;

typedef struct mqtt_client
{
	os_socket_tcp_t * ptcp;
	char buf[1024];
	int send_off;
	int buflen;
	int sock_avalible;
}CLIENT;

CLIENT mqtt[5000];


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

void * mqtt_write_call(void * parm)
{
	os_event_t * event = (os_event_t *)parm;
	CLIENT * pmqtt = (CLIENT *)event->__user;
	if(pmqtt->send_off <= pmqtt->buflen)
	{
		if(event->which_op & EV_WR)
		{
			int offset = pmqtt->send_off;
			int left   = pmqtt->buflen-pmqtt->send_off;
			int size = os_write_socket(event->tcp->socket_fd,pmqtt->buf[offset],left);
			pmqtt->send_off += size;
		}
	}
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
			//os_del_event(event);
			//os_close_socket(event->fd);
			
			//printf("close sock : %d\n",event->fd);
		}
	}
	if(event->which_op & EV_WR)
	{
		//printf("tcp connect ok --->remote port: %-10d\n",event->tcp->remote_port);
		
		CLIENT * pmqtt = (CLIENT *)event->__user;

		if(pmqtt->sock_avalible != 1)
		{
			pmqtt->sock_avalible = 1;
			return ;
		}
		if(pmqtt->send_off < pmqtt->buflen)
		{
				int offset = pmqtt->send_off;
				int left   = pmqtt->buflen-pmqtt->send_off;
				int size = os_write_socket(event->tcp->socket_fd,pmqtt->buf[offset],left);
				printf("size : %d\n",size);
				pmqtt->send_off += size;
		}
		else
		{
			pmqtt->send_off = 0;
		}
	}
}

int main(int argc,char ** argv)
{
	signal(SIGPIPE, SIG_IGN);
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
	 os_event_t event;

	/*os_socket_udp_t * pepoll = os_create_udp(5683, 0);
    os_event_t event;
    event.fd = pepoll->socket_fd;
    event.which_op = EV_RE;
	event.udp = pepoll;
	event.doevent = event_call_back;
	
    
    os_add_event(&event);*/


/*	 pepoll = os_create_udp(9001, 0);
	event.fd = pepoll->socket_fd;
    event.which_op = EV_RE;
	event.udp = pepoll;
	event.doevent = event_call_back;
	
    
    os_add_event(&event);*/
	/*os_event_t event_temp;
    event_temp.fd = pepoll->socket_fd;
    event_temp.which_op = EV_WR;
	event_temp.udp =pepoll;
	event_temp.doevent = event_call_back;
	
    os_add_event(&event_temp);*/


	//for tcp server
	/*os_socket_tcp_t * ptcp   =   os_create_tcp(9000, 0);
	os_tcp_listen(ptcp);
	//os_event_t event;
    event.fd = ptcp->socket_fd;
    event.which_op = EV_RE;
	event.tcp = ptcp;
	event.doevent = event_call_accept;
    os_add_event(&event);*/
	//for tcp client


	
	unsigned char connect_buf[]={0x10, 0x28, 0x00, 0x06, 0x4d, 0x51, 0x49, 0x73, 0x64, 0x70, 0x03, 0xc2, 0x00, 0x1e, 0x00, 0x0a, 
0x61, 0x76, 0x65, 0x6e, 0x67, 0x61, 0x6c, 0x76, 0x6f, 0x6e, 0x00, 0x03, 0x63, 0x69, 0x64, 0x00, 
0x09, 0x63, 0x61, 0x6d, 0x70, 0x65, 0x61, 0x64, 0x6f, 0x72};
	int i,j=0;
	for(i=0;i<5000;i++)
	{
		j++;
		os_socket_tcp_t * ptcp   =   os_create_tcp(0, 0);
		//int ret = os_connect_socket(ptcp,"124.128.194.94", 8125);
		int ret = os_connect_socket(ptcp,"121.42.249.117", 9000);
		
		mqtt[i].ptcp = ptcp;
		mqtt[i].send_off = 0;
		mqtt[i].buflen = 1024;
		mqtt[i].sock_avalible = 0;

		char tempid[16]={0};
		sprintf(tempid,"%s","200000");
		sprintf(&tempid[6],"%04d",i);
		memcpy(&(mqtt[i].buf[0]),connect_buf,sizeof(connect_buf));
		memcpy(&(mqtt[i].buf[16]),tempid,10);

		//int ret = os_connect_socket(ptcp,"192.168.0.141", 9000);
		//send(ptcp->socket_fd,"a",1,0);
		printf("connect remote : %d errno: %d\n",ret,errno);
		event.fd = ptcp->socket_fd;
		event.which_op = EV_WR;
		event.tcp = ptcp;
		event.doevent = event_call_back;
		event.__user = &mqtt[i];
		os_add_event(&event);
		printf("j = %d\n",j);
		//usleep(1000*10);
	}

	sleep(5);


	
	while(1)
	{
		printf("send once!\n");
		for(i=0;i<5000;i++)
		{
			if(mqtt[i].sock_avalible == 1)
			{
				int size =   send(mqtt[i].ptcp->socket_fd,mqtt[i].buf,mqtt[i].buflen,0);
				/*int ii;
				for(ii=0;ii<size;ii++)
					{
					printf("0x%x ",mqtt[i].buf[ii]);
				}*/
				//printf("\n");
				if(size <=0 )
				{
					printf("err : %d\n",size);
				}
				if(size >=0)
				{
					mqtt[i].send_off = size;
					event.fd = mqtt[i].ptcp->socket_fd;
					event.which_op = EV_WR;
					event.tcp = mqtt[i].ptcp;
					event.doevent = event_call_back;
					os_add_event(&event);
				}
			}
			
			
		}

		
		struct timeval tv;
    	tv.tv_sec=5;
    	tv.tv_usec=0;
		select(0,NULL,NULL,NULL,&tv);
		
	}
	
	/*os_heap_init();
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
	os_heap_debug();*/
	while(1)
	{
    	sleep(1000);

	}
    

    return 0;
}
