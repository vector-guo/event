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
    int fd;                             //事件fd
    int which_op;                       //对应哪种操作，是读 还是写
    unsigned int active;
    struct list_head list;              //用来连接同一次epool_wait的事件
	struct hlist_node node;
	os_locker_t locker;                 //该锁锁定时，这个事件正在被使用
    os_locker_t event_locker;			//鍋囪宸茬粡鏈変簨浠朵簡锛屽氨涓婇攣锛屽張鏈夋柊浜嬩欢鍔犲叆鏃讹紝涓嶅啀寰�浜嬩欢鍒楄〃閲岄潰鏀�
	os_socket_udp_t * udp;
	os_socket_tcp_t * tcp;
    void *(* doevent)(void * param);    //对应的处理函数
	void * __user;						//鐢ㄦ埛鏁版嵁
};

#endif