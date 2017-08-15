#ifndef __OS__SOCKET__H
#define __OS__SOCKET__H
#include<fcntl.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>



typedef struct os_socket_udp_s os_socket_udp_t;

struct os_socket_udp_s
{
    int socket_fd;
    
};


typedef struct os_socket_tcp_s os_socket_tcp_t;
struct os_socket_tcp_s
{
    int socket_fd;
	unsigned int local_port;
	unsigned int local_ip;

	unsigned int remote_port;
	unsigned int remote_ip;
	
    
};

os_socket_udp_t * os_create_udp(unsigned short port,unsigned int ip);
int os_read_socket(int sock,char * buf,int maxlen);
int  os_read_udp(os_socket_udp_t * udp ,char * buf,int maxlen);



int  os_tcp_listen(os_socket_tcp_t * tcp);
os_socket_tcp_t * os_create_tcp(unsigned short port,unsigned int ip);
int  os_tcp_listen(os_socket_tcp_t * tcp);

int os_close_socket(int sock);
#endif