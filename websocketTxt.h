//ws_server

//	参数表初始化：数组--参数表tabid个数（子表id个数）		，
//	单个参数表初始化信息结构体
struct ChildTabPasrInitInfo{
	id;
	len;
	flag;	//读写权限，只有读才可以被订阅，只有写才能被设置
	void *pData;
	void *setParseFunc();
};

struct TabPasrInitInfo{
	tab_uuid;	//参数表id
	idNums;
	ChildTabPasrInitInfo idArr[];		//子表info
};


class websocket
{

public:

	lock; //维护发送消息fifo
	u8_t initParseFlag;
	TimeOut sec;
	
	websocket(struct TabPasrInitInfo initArr[],TimeOut sec)
	{
		intilcok(&lock);
		WsServerInitParse(initArr);
	}

	WsServerInitParse(TabPasrInitInfo initArr[])
	{
		//....


		
		initParseFlag = 1;	//初始化参数才能开始
	}
	WsPushSendMsgToFifo(void *data, sendlevel level, );//添加发送消息到fifo---添加到尾部/头部

private:	
	WsShakeHandFunticon(); //握手函数
	WsAnlanyFuntion(); //解析函数	
	WsPullSendMsgToFifo();//获取发送消息
	{

	}
		
};

clase WsServer:public websocket
{
public:

	list_head/Arr; 	//已连接的tcpfd（链表/数组）. 
	serverlock; //维护已连接的tcpfd（链表/数组）.
	u16_t MaxClientNums;	//最大连接客户端数量
	u16_t CurClientNums;		//当前已连接客户端数量

	
//用户实现：
//		最大客户端连接数，
//		接收消息的回调--处理消息和发送消息（涉及参数表的读写）
//		（可选接收ping的回调函数）,
//		超时时间
	WsServer(u16_t maxClientNums, recvFuntion, recvPingFuntion, TimeOut sec)
	{
		MaxClientNums = maxClientNums;
		intilcok(&serverlock);
		sec.usec = {1, 200us};
		init_list_head(&list_head)	

			
	}
	~WsServer();
//		tcp端口
//		创建任务得接口--平台...
	int WsServerStart(u16_t port, void *CreatTaskFunExt())
	{
//	创建任务一：接受TCP连接 保存fd 当接收到第一个fd的时候创建任务二
//	任务二：发送消息和接收已连接的TCP的消息
//		1.刚连接的tcp需要握手对协议
//		2.select监测读消息，超时-->1，读发送消息队列发送；2、超时x次就发ping消息
//		3.超时y次没接收到ping消息就断开连接
//		4.select监测读错误 如果不是 ENITER 就断开连接 删除fd	

	}


	
}；

clase WsClient:public websocket
{
public:
	u8_t TimeOutTimes;		//发送ping的最大超时次数

	WsClient(MaxClientSize, CreatTaskFunExt(Task))
	{

		创建线程一
		
		创建线程二

			
	}
	list_head/Arr; 	//已连接的tcpfd（链表/数组）

	
}；

