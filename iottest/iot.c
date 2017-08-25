//this file to test iot platform 
//virtual device
#include"os_thread.h"
#include"os_time.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include"os_socket.h"


unsigned char MakeFmpCheckCode(unsigned char*pBuf, int iLen)
{
	
	char szTempBuf[128];
	memcpy(szTempBuf, pBuf, iLen);
	unsigned char uRe=0;
	int i=0;
	for (i=0; i<iLen; i++)
	{
		uRe+=(unsigned char)szTempBuf[i];
	}
	

	return uRe;
}
void * udp_onedevice(void * parg)
{
	int * port = (int * )parg;
	os_socket_udp_t * udp = os_create_udp(*port, 0);
	send(udp->socket_fd,"test",4,0);
}

//one thread means one virtual device
void * onedevice(void * parg)
{
	int * dev_id = (int * )parg;
	int sock;
	struct sockaddr_in echoserver;


	
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
         printf("Failed to create socket");
		 return 0;
    }
    memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
    echoserver.sin_family = AF_INET;                  /* Internet/IP */
    echoserver.sin_addr.s_addr = inet_addr("127.0.0.1");  /* IP address */
    echoserver.sin_port = htons(9000);


	if (connect(sock,(struct sockaddr *) &echoserver,sizeof(echoserver)) < 0) 
	{
         printf("Failed to connect with server");
		 return 0;
    }

	//680014680201030106015006 0000000005F5E13A 00020314 92 16
	//680014680201030106015006 0000000000000000 00020314 00 16 
	unsigned char login[128];
	unsigned char id[8];
	id[0] = 0;
	id[1] = 0;
	id[2] = 0;
	id[3] = 0;
	id[4] = 0;
	id[5] = 0;
	id[6] = (*dev_id & 0xff00) >> 8;
	id[7] = *dev_id & 0xff;

	login[0] = 0x68;
	login[1] = 0x0;
	login[2] = 0x14;
	login[3] = 0x68;
	login[4] = 0x02;
	login[5] = 0x01;
	login[6] = 0x03;
	login[7] = 0x01;
	login[8] = 0x06;
	login[9] = 0x01;
	login[10] = 0x50;
	login[11] = 0x06;

	memcpy(&login[12],&id,8);
	login[20] = 00;
	login[21] = 0x02;
	login[22] = 0x03;
	login[23] = 0x14;
	login[24] = MakeFmpCheckCode(&login[4],20);
	login[25] = 0x16;
	
	int a = send(sock,login,26,0);
	os_time_t time;
	printf("send: %d %llu\n",a,os_time_ms());
	//while(1)
	{
		sleep(5);
	}
	close(sock);
	//send(sock,login,26,0);
	while(1)
	{
		sleep(5);
	}
}


int main(int argv,char ** argc)
{
	printf("iot test\n");

	//create 500 virtual devices
	int i;
	for(i = 500;i<600;i++)
    {
    	int * id = (int *)malloc(4);
		*id = i;
		os_thread_create(onedevice,id);
    }


	while(1)
	{
		sleep(10);
	}
	
	return ;
}
