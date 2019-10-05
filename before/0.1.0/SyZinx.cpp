#include "SyZinx.h"
#include "IChannel.h"
#include "IRole.h"
#include "IProtocol.h"
//初始化自己的指针
SyZinxKernel* SyZinxKernel::mSyZinx = NULL;

struct event_base* SyZinxKernel::SyZinxBase = nullptr;

struct evconnlistener* SyZinxKernel::SyZinxListen = nullptr;

MemRecycled Recycler;

SyZinxKernel::SyZinxKernel()
{
	
}

SyZinxKernel::~SyZinxKernel()
{
}



/*
evconnlistener_cb cb : 回调函数用于返回一个客户端连接的文件描述符
void(*evconnlistener_cb)(struct evconnlistener* listener,
	evutil_socket_t fd,
	struct sockaddr* sock, int socklen, void* arg);
监听回调函数
A callback that we invoke when a listener has a new connection.

   @param listener The evconnlistener
   @param fd The new file descriptor
   @param addr The source address of the connection
   @param socklen The length of addr
   @param user_arg the pointer passed to evconnlistener_new()
当有新的客户端连接的时候就会调用此回调函数
fd为客户端的文件描述符
sock为客户端的原始结构体
socklen为长度

*/

//event recv send回调函数传递参数
struct  SyZinxDeliver;
struct SyZinxBuffer_Event_Deliver
{
	SyZinxDeliver* iSyZinxDeliver;
	int iClient_Fd;
};

//listen回调函数传入的参数
struct  SyZinxDeliver
{

	//base句柄
	struct event_base* iSyZinxBase;

	//保存用于监听套接字的evconnlistener
	struct evconnlistener* iSyZinxListen;

	//核心框架地址
	SyZinxKernel* iSyZinx;
};

//读回调
void SyZinxBuffer_RecvData_Cb(struct bufferevent* buffer, void* arg)
{
	//当有可读事件发生时将会调用此函数进入环节链进行一系列任务
	SyZinxBuffer_Event_Deliver* tmpEvent_Deliver = (SyZinxBuffer_Event_Deliver*)(arg);
	int mClient_fd = tmpEvent_Deliver->iClient_Fd;
#if 1
	char buf[BUFFSIZE];
	memset(buf, 0, sizeof(buf));
	int iRet = bufferevent_read(buffer, buf, BUFFSIZE);
	if (-1 == iRet)
	{
		return;
	}
	std::string RecvString(buf, iRet);
	//根据当前的触发的客户端套接字不同来调用不同的客户端任务链
	auto client_buffer = SyZinxKernel::mSyZinx->Ichannel_Map[mClient_fd];
	client_buffer->Ichannel_RecvBuffer(RecvString);
#endif
}

//写回调
void SyZinxBuffer_SendData_Cb(struct bufferevent* buffer, void* arg)
{
	//当有可写事件发生时将会调用此函数荣任务链返回的序列化字符串会进入该函数
	std::cout << "write event is trigger" << std::endl;

	SyZinxBuffer_Event_Deliver* tmpEvent_Deliver = (SyZinxBuffer_Event_Deliver*)(arg);
	int mClient_fd = tmpEvent_Deliver->iClient_Fd;

}




//事件回调
void SyZinxBuffer_Event_Cb(struct bufferevent* buffer, short events, void* arg)
{
	SyZinxBuffer_Event_Deliver* tmpEvent_Deliver = (SyZinxBuffer_Event_Deliver*)(arg);

	//框架地址
	SyZinxKernel* mSyZinx = tmpEvent_Deliver->iSyZinxDeliver->iSyZinx;
	int iKey;
	if (events & BEV_EVENT_EOF) // Client端关闭连接 
	{
		std::cout << "Cliented : "<< tmpEvent_Deliver->iClient_Fd<<" is exit"<< std::endl;
		delete tmpEvent_Deliver;
		iKey = tmpEvent_Deliver->iClient_Fd;
		SyZinxKernel::SyZinxKernel_Client_Close(iKey);
	}
	else if (events & BEV_EVENT_ERROR) // 连接出错 
	{
		std::cout << "Got an error on the connection:" << std::endl;
		
			
	}
	else if (events & BEV_EVENT_WRITING)//写入时发生做错误
	{
		std::cout << "send to client is err" << std::endl;
	}
	// 如果还有其它的event没有处理，那就关闭这个bufferevent 

#if 0 //switch会出现events判断问题后续会摒弃
	switch (events)
	{
	case BEV_EVENT_READING://读取时发生错误
		std::cout << "操 作 时 发 生 错 误" << std::endl;
		delete tmpEvent_Deliver;
		iKey = tmpEvent_Deliver->iClient_Fd;
		SyZinxKernel::SyZinxKernel_Client_Close(iKey);
		break;
	case BEV_EVENT_WRITING://写入操作时发生某事件，具体是哪种事件请看其他标志	
	{
		//可能是客户端已经断开连接
		delete tmpEvent_Deliver;
		iKey = tmpEvent_Deliver->iClient_Fd;
		SyZinxKernel::SyZinxKernel_Client_Close(iKey);
		std::cout << "client is exit" << std::endl;
		break;
	}
	case BEV_EVENT_ERROR: //操 作 时 发 生 错 误 。 关 于 错 误 的 更 多 信 息 ， 请 调 用
		std::cout << "操 作 时 发 生 错 误" << std::endl;
		delete tmpEvent_Deliver;
		iKey = tmpEvent_Deliver->iClient_Fd;
		SyZinxKernel::SyZinxKernel_Client_Close(iKey);
		std::cout << "client is exit" << std::endl;
		break;
	case BEV_EVENT_TIMEOUT://发生超时。
		break;
	case BEV_EVENT_EOF://客户端退处
		std::cout << "client is exit" << std::endl;
		break;
	case BEV_EVENT_CONNECTED://请求的连接过程已经完成。
		break;
	default:
		break;
	}
#endif
	return;
}


void SyZinxKernel_Listen_CB(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* arg)
{
	SyZinxDeliver* poSyZinxDeliver = (SyZinxDeliver*)arg;
	if (NULL == poSyZinxDeliver)
	{
		return;
	}

	struct event_base* poSyZinxBase = poSyZinxDeliver->iSyZinxBase;
	
	//框架地址
	SyZinxKernel* mSyZinx = poSyZinxDeliver->iSyZinx;

	//test
	std::cout << "new client is connect  " <<"Client:"<<fd<< std::endl;

	//为客户端的一个bufferevent结构体
	struct bufferevent* Client_buffer = NULL;

	Client_buffer = bufferevent_socket_new(poSyZinxBase, fd, BEV_OPT_CLOSE_ON_FREE);
	//创建一系列工厂类
	SyZinxTcpFactory* NewClientFac = new SyZinxTcpFactory(fd, Client_buffer);

	//继续讲传递参数的类以及套接字传递到回调函数
	SyZinxBuffer_Event_Deliver* poEvent_Deliver = new SyZinxBuffer_Event_Deliver;
	poEvent_Deliver->iSyZinxDeliver = poSyZinxDeliver;
	poEvent_Deliver->iClient_Fd = fd;

	//设置客户端的回调函数
	//根据客户端读取或者写入时调用
	bufferevent_setcb(Client_buffer, SyZinxBuffer_RecvData_Cb, SyZinxBuffer_SendData_Cb, SyZinxBuffer_Event_Cb, (void*)poEvent_Deliver);

	//将bufferevent上框架的map容器
	mSyZinx->iBufferevent_Map.insert(std::make_pair(fd, Client_buffer));

	//设置客户端events为读事件以及循环检测事件
	bufferevent_enable(Client_buffer, EV_READ | EV_PERSIST);

	
#if 0
	auto client_buffer = SyZinxKernel::mSyZinx->IRole_Map[fd];
	client_buffer->Init_Player();
#endif
}
int SyZinxKernel::SyZinxKernel_Init(const short _inPort)
{
	if (_inPort <= 0 || _inPort > 65250)
	{
		return -1;
	}
	mSyZinx = new SyZinxKernel();

	int iRet = -1;

	//初始化结构体
	struct sockaddr_in _Serveraddr;
	memset(&_Serveraddr, 0, sizeof _Serveraddr);
	_Serveraddr.sin_family = AF_INET;
	_Serveraddr.sin_port = htons(_inPort);

	//创建句柄
	SyZinxBase = event_base_new();
	if (NULL == SyZinxBase)
	{
		return -1;
	}

	//设置传递参数
	SyZinxDeliver* poSyZinxDeliver = new SyZinxDeliver;
	poSyZinxDeliver->iSyZinxBase = SyZinxBase;
	poSyZinxDeliver->iSyZinxListen = SyZinxListen;
	poSyZinxDeliver->iSyZinx = mSyZinx;

	//设置监听
	SyZinxListen = evconnlistener_new_bind(SyZinxBase, SyZinxKernel_Listen_CB, (void*)poSyZinxDeliver,
	LEV_OPT_LEAVE_SOCKETS_BLOCKING | LEV_OPT_THREADSAFE, 10, (const sockaddr*)& _Serveraddr, sizeof(_Serveraddr));
	if (SyZinxListen == NULL)
	{
		return -1;
	}
	return 1;
}

void SyZinxKernel::SyZinxKernel_Run()
{
	
	return SyZinxKernel::mSyZinx->Run();
}

void SyZinxKernel::Run()
{
	//循环监听事件
	event_base_dispatch(SyZinxKernel::SyZinxBase);
}

void SyZinxKernel::SyZinxKernel_Close()
{
	//关闭
	evconnlistener_free(SyZinxKernel::SyZinxListen);
	event_base_free(SyZinxKernel::SyZinxBase);

	

}
bufferevent* SyZinxKernel::SyZinxKernel_GetBuffer(int _iclientfd)
{
	return mSyZinx->iBufferevent_Map[_iclientfd];
}

void SyZinxKernel::SyZinxKernel_Client_Close(const int mClient_fd)
{
	struct bufferevent* tmpbuffer = SyZinxKernel::mSyZinx->iBufferevent_Map[mClient_fd];
	Ichannel* tmpIchannel = SyZinxKernel::mSyZinx->Ichannel_Map[mClient_fd];
	Iprotocol* tmpIprotocol = SyZinxKernel::mSyZinx->IcProtocol_Map[mClient_fd];
	Irole* tmpIrole = SyZinxKernel::mSyZinx->IRole_Map[mClient_fd];

	SyZinxKernel::mSyZinx->iBufferevent_Map.erase(mClient_fd);
	SyZinxKernel::mSyZinx->Ichannel_Map.erase(mClient_fd);
	SyZinxKernel::mSyZinx->IcProtocol_Map.erase(mClient_fd);
	SyZinxKernel::mSyZinx->IRole_Map.erase(mClient_fd);

	//释放buffer
	bufferevent_free(tmpbuffer);
	delete tmpIprotocol->mClient_ByteString;
	delete tmpIchannel;
	delete tmpIprotocol;
	delete tmpIrole;
	std::cout << "free success" << std::endl;

}
void SyZinxKernel::SyZinxKernel_SaveDatabase(SyZinxDatabase* _iDatabase)
{
	if (_iDatabase == NULL)
	{
		return;
	}
	mSyZinx->Database = _iDatabase;
}
/*no*/

MemRecycled::~MemRecycled()
{
	if (SyZinxKernel::mSyZinx != nullptr)
	{
		delete SyZinxKernel::mSyZinx;
		SyZinxKernel::mSyZinx = nullptr;
	}
	std::cout << "~MemRecycled()" << std::endl;
}

