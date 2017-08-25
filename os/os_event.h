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
    int fd;                             //ÊÂ¼şfd
    int which_op;                       //¶ÔÓ¦ÄÄÖÖ²Ù×÷£¬ÊÇ¶Á »¹ÊÇĞ´
    unsigned int active;
    struct list_head list;              //ÓÃÀ´Á¬½ÓÍ¬Ò»´Îepool_waitµÄÊÂ¼ş
	struct hlist_node node;
	os_locker_t locker;                 //¸ÃËøËø¶¨Ê±£¬Õâ¸öÊÂ¼şÕıÔÚ±»Ê¹ÓÃ
    os_locker_t event_locker;			//å‡è®¾å·²ç»æœ‰äº‹ä»¶äº†ï¼Œå°±ä¸Šé”ï¼Œåˆæœ‰æ–°äº‹ä»¶åŠ å…¥æ—¶ï¼Œä¸å†å¾€äº‹ä»¶åˆ—è¡¨é‡Œé¢æ”¾
	os_socket_udp_t * udp;
	os_socket_tcp_t * tcp;
    void *(* doevent)(void * param);    //¶ÔÓ¦µÄ´¦Àíº¯Êı
	void * __user;						//ç”¨æˆ·æ•°æ®
};

#endif