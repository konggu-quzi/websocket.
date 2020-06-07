//#include <arpa/inet.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>


#include "websocket_Handlder.h"
#include "websocket.h"


websocket::websocket(u16_t u_port):
	sec(TIMEWAIT),
	port(u_port)
{

//		intilcok(&fifolock);
	printf("websocket port:%d\n", u_port);
}

websocket::~websocket()
{
	//销毁锁
	//释放申请的内存
	printf("~websocket\n");
}



char *websocket::printErr()
{
	return errbuf;
}


WsServer *WsServer::m_WsServer = NULL;


//用户实现：
//		最大客户端连接数，
//		接收消息的回调--处理消息和发送消息（涉及参数表的读写）
//		（可选接收ping的回调函数）,
//		超时时间	
WsServer::WsServer(u16_t u_port = PORT, void *u_recvFuntion = NULL, void *u_recvPingFuntion = NULL):
		m_epollfd(-1),
		m_listenfd(-1),
		maxClientNums(MAXCLIENTNUMS), 
		curClientNums(0), //显示调用父类的构造函数
		recvHandlderFuntion(u_recvFuntion),
		recvPingPongHandlderFuntion(u_recvPingFuntion),
		websocket(u_port)
		
{
	printf("WsServer port:%d\n", u_port);
//	init(&Clientlock);
	memset(errbuf, 0, sizeof(errbuf));
	if(0 != init())
		exit(1);
}         

WsServer::~WsServer()
{
	//销毁锁
	//释放申请的内存
	printf("~WsServer\n");
}

//初始化本地fd
int WsServer::init()
{
	if (m_listenfd < 0) {
		sprintf(errbuf,"m_listenfd > 0\n");
		return -1;
	}
	
//	m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
//	if (m_listenfd == -1) {
//		DEBUG_LOG("创建套接字失败!");
//		return -1;
//	}
//	struct sockaddr_in server_addr;
//	memset(&server_addr, 0, sizeof(sockaddr_in));
//	server_addr.sin_family = AF_INET;
//	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//	server_addr.sin_port = htons(port);
//	if(-1 == bind(m_listenfd, (struct sockaddr *)(&server_addr), sizeof(server_addr))){
//		DEBUG_LOG("绑定套接字失败!");
//		return -1;
//	}
//	if(-1 == listen(m_listenfd, 5)){
//		DEBUG_LOG("监听失败!");
//		return -1;
//	}

//	m_epollfd = epoll_create(MAXEVENTSSIZE);
//
//	ctl_event(m_listenfd, true);	
//	DEBUG_LOG("服务器启动成功!");
	return 0;
}

int WsServer::epoll_loop() {
	struct sockaddr_in client_addr;
	socklen_t clilen;
	int nfds = 0;
	int fd = 0;
	int bufflen = 0;
	struct epoll_event events[MAXEVENTSSIZE];
	
	while(1){
		
		nfds = epoll_wait(m_epollfd, events, MAXEVENTSSIZE, TIMEWAIT);
		for(int i = 0; i < nfds; i++){
			if(events[i].data.fd == m_listenfd){
				fd = accept(m_listenfd, (struct sockaddr *)&client_addr, &clilen);
				//print....
				ctl_event(fd, true);
			}
			else if(events[i].events & EPOLLIN){
				if((fd = events[i].data.fd) < 0)
					continue;
				Websocket_Handler *handler = websocket_handler_map_[fd];
				if(handler == NULL)
					continue;
				if((bufflen = read(fd, recvbuf, BUFFLEN)) <= 0){
					ctl_event(fd, false);
				}
				else{
					handler->process();
				}
			}
		}
		//发送
	}

	return 0;
}

int WsServer::set_noblock(int fd){
	int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

WsServer *WsServer::get_share_WsServer(){
	if(m_WsServer == NULL)
		m_WsServer = new WsServer();
	return m_WsServer;
}

void WsServer::ctl_event(int fd, bool flag){
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = flag ? EPOLLIN : 0;
	epoll_ctl(m_epollfd, flag ? EPOLL_CTL_ADD : EPOLL_CTL_DEL, fd, &ev);
	if(flag){
		set_noblock(fd);
		websocket_handler_map_[fd] = new Websocket_Handler(fd);
		if(fd != m_listenfd)
			DEBUG_LOG("fd: %d 加入epoll循环", fd);
	}
	else{
		close(fd);
		delete websocket_handler_map_[fd];
		websocket_handler_map_.erase(fd);
		DEBUG_LOG("fd: %d 退出epoll循环", fd);
	}
}

void WsServer::start(){
	epoll_loop();
}

