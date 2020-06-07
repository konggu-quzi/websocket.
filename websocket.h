#ifndef _WEBSOCKET_
#define _WEBSOCKET_

#include <stdio.h>		//定义输入／输出函数
#include <stdlib.h>		//定义杂项函数及内存分配函数


#include "m_common.h"
#include "m_websocketparse.h"

typedef std::map<int, Websocket_Handler *> WEB_SOCKET_HANDLER_MAP;


typedef enum {

	normalLevel = 0,
	emergencyLevel,
	
}MsgLevel_t;


struct myFifoData{
//	list_head node;
	int sendTofd;
	u32_t data_len;
	void *pData;
};


#define PORT 9000
#define TIMEWAIT 100
#define BUFFLEN 2048
#define MAXEVENTSSIZE 20

class websocket
{
private:
	char *printErr();
public:
	websocket(u16_t m_port);
	~websocket();
	WsPushSendMsgToFifo(void *data, MsgLevel_t msglevel);//添加发送消息到fifo---添加到尾部/头部
public:
//	list_head fifohead;
	u32_t fifolock; //维护发送消息fifo
	TimeOut sec;
	u16_t port;
	char errbuf[50];
	
protected:       //成员变量，声明为protected或者public，这里选择protected
				//若声明为 private ，则不能被子类继承访问，会报错
private:	
	
};

#define MAXCLIENTNUMS	10

//子类
class WsServer:public websocket {
private:
	WsServer(u16_t u_port, void *u_recvFuntion, void *u_recvPingFuntion);
	~WsServer();
	int init();
	int epoll_loop();
	int set_noblock(int fd);
	void ctl_event(int fd, bool flag);
public:
	u16_t maxClientNums;	//最大连接客户端数量
	u16_t curClientNums;		//当前已连接客户端数量
	void *recvHandlderFuntion;
	void *recvPingPongHandlderFuntion;
	void start();
private:
	int m_epollfd;
	int m_listenfd;
	int *clientFdArr; 	//已连接的tcpfd（链表/数组）
	int clientlock; //维护已连接的tcpfd（链表/数组）
	static WsServer *m_WsServer;
	WEB_SOCKET_HANDLER_MAP websocket_handler_map_;
	
};


#endif

