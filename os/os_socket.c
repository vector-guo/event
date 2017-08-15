#include<errno.h>
#include"os_socket.h"
#include"os_log.h"

os_socket_udp_t * os_create_udp(unsigned short port,unsigned int ip) 
{
	os_socket_udp_t * udp = (os_socket_udp_t *)malloc(sizeof(os_socket_udp_t)); //这里应该实用socket池，先动态申请
	if(udp == NULL)
	{
		os_log_error("malloc udp socket err!");
		return -1;
	}
	
	int sock = socket(PF_INET,SOCK_DGRAM,0);
	if(sock <= 0)
	{
		os_log_error("socket udp socket err!");
		return -1;	
	}
	udp->socket_fd = sock;


	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		os_log_error("bind port %d err!",port);
		return  -1;
	}
	
	int flags = fcntl(sock, F_GETFL, 0);                        //��ȡ�ļ���flagsֵ��
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
	
	return udp;
}

int os_read_socket(int sock,char * buf,int maxlen)
{
	int once_len,buf_off=0;
	
	while(1) //epoll ET模式，循环读
	{
		once_len = recv(sock,buf+buf_off,maxlen-buf_off,0);
		if(once_len < 0)	
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK) //已经读完
			{
				return buf_off;
			}
			else
			{
				return -1;
			}
		}
		else if(once_len == 0)
	    {
			return 0;
		}
		
		buf_off += once_len;
	}
}

int  os_read_udp(os_socket_udp_t * udp ,char * buf,int maxlen)
{
	int once_len,buf_off=0;
	
	while(1) //epoll ET模式，循环读
	{
		once_len = recv(udp->socket_fd,buf+buf_off,maxlen-buf_off,0);
		if(once_len < 0)	
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK) //已经读完
			{
				return buf_off;
			}
			else
			{
				return -1;
			}
		}
		
		buf_off += once_len;
	}
}

os_socket_tcp_t * os_create_tcp(unsigned short port,unsigned int ip)
{
	
		os_socket_tcp_t * tcp = (os_socket_tcp_t *)malloc(sizeof(os_socket_tcp_t)); //这里应该实用socket池，先动态申请
		if(tcp == NULL)
		{
			os_log_error("malloc udp socket err!");
			return -1;
		}

		int sock = socket(PF_INET,SOCK_STREAM,0);
		if(sock <= 0)
		{
			os_log_error("socket udp socket err!");
			return -1;	
		}

		int on=1;  
	    if((setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
	    {  
	        perror("setsockopt failed");
	    }  
		tcp->socket_fd = sock;


		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		{
			os_log_error("bind port %d err!",port);
			return	-1;
		}

		int flags = fcntl(sock, F_GETFL, 0);						//��ȡ�ļ���flagsֵ��
		fcntl(sock, F_SETFL, flags | O_NONBLOCK);

		return tcp;
}

int  os_tcp_listen(os_socket_tcp_t * tcp)
{
	
	return listen(tcp->socket_fd , 1024*1024);
}

os_socket_tcp_t * os_accept_tcp(os_socket_tcp_t * tcp)
{
	 struct sockaddr_in addr;
	 int size = sizeof(addr);
	 
	 int osSocket = accept(tcp->socket_fd, (struct sockaddr*)&addr, &size);
	 if(osSocket == -1 && errno == EAGAIN)
	 {
		return 0;
	 }
	
	 int flags = fcntl(osSocket, F_GETFL, 0);						 //��ȡ�ļ���flagsֵ��
	 fcntl(osSocket, F_SETFL, flags | O_NONBLOCK);
	 
	 os_socket_tcp_t * ptcp = (os_socket_tcp_t *)malloc(sizeof(os_socket_tcp_t)); //这里应该实用socket池，先动态申请
	 ptcp->socket_fd     = osSocket;
	 ptcp->remote_ip     = ntohl(addr.sin_addr.s_addr);
	 ptcp->remote_port = ntohs(addr.sin_port);
	 
	 return ptcp;
}

int os_close_socket(int sock)
{
	return close(sock);
}
