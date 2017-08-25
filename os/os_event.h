#ifndef OS_EVENT__H
#define OS_EVENT__H
#include"os_list.h"
#include"os_hlist.h"
#include"os_thread.h"
#include"os_socket.h"

typedef struct os_event_s os_event_t;

#define EV_RE                 0x01
#define EV_WR                 0x02   

struct os_event_s
{
    int fd;                             //�¼�fd
    int which_op;                       //��Ӧ���ֲ������Ƕ� ����д
    unsigned int active;
    struct list_head list;              //��������ͬһ��epool_wait���¼�
	struct hlist_node node;
	os_locker_t locker;                 //��������ʱ������¼����ڱ�ʹ��
    os_locker_t event_locker;			//假设已经有事件了，就上锁，又有新事件加入时，不再往事件列表里面放
	os_socket_udp_t * udp;
	os_socket_tcp_t * tcp;
    void *(* doevent)(void * param);    //��Ӧ�Ĵ�����
	void * __user;						//用户数据
};

#endif