#include "SyZinx.h"

#include <iostream>
#include <fstream>
#include <istream>
using namespace std;

SyZinxKernel* SyZinxKernel::mSyZinx = nullptr;


SyZinxRecycle recycle;


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

SyZinxKernel::SyZinxKernel()
{
	
}
SyZinxKernel::~SyZinxKernel()
{
}

//如果有客户端连接
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
	std::cout << "new client is connect  " << "Client:" << fd << std::endl;

	//将客户端上到map集群
	SyZinxKernelMsg * newMsg = new SyZinxKernelMsg;
	
	

}
int SyZinxKernel::SyZinxKernel_Init(const short _inPort)
{
	if (_inPort <= 0 || _inPort >= 65535)
	{
		return -1;
	}
	SyZinxKernel::mSyZinx = new SyZinxKernel;

	//初始化结构体
	struct sockaddr_in _Serveraddr;
	memset(&_Serveraddr, 0, sizeof _Serveraddr);
	_Serveraddr.sin_family = AF_INET;
	_Serveraddr.sin_port = htons(_inPort);
	

	
	//创建句柄
	mSyZinx->SyZinxBase = event_base_new();
	if (NULL == mSyZinx->SyZinxBase)
	{
		return -1;
	}
	//设置传递参数
	SyZinxDeliver* poSyZinxDeliver = new SyZinxDeliver;
	poSyZinxDeliver->iSyZinxBase = mSyZinx->SyZinxBase;
	poSyZinxDeliver->iSyZinxListen = mSyZinx->SyZinxListen;
	poSyZinxDeliver->iSyZinx = mSyZinx;

	//设置监听
	mSyZinx->SyZinxListen = evconnlistener_new_bind(mSyZinx->SyZinxBase, SyZinxKernel_Listen_CB, (void*)poSyZinxDeliver,
		LEV_OPT_LEAVE_SOCKETS_BLOCKING | LEV_OPT_THREADSAFE, 10, (const sockaddr*)& _Serveraddr, sizeof(_Serveraddr));
	if (mSyZinx->SyZinxListen == NULL)
	{
		return -1;
	}
	mSyZinx->Server_Sockaddr.family = _Serveraddr.sin_family;
	mSyZinx->Server_Sockaddr.Prot = to_string(ntohs(_Serveraddr.sin_port));
	mSyZinx->Server_Sockaddr.sin_addr = inet_ntoa(_Serveraddr.sin_addr);

	//初始化线程适配器
	SyZinxAdapterPth* pth = new SyZinxAdapterPth(mSyZinx->PthNum);
	pth->SyZinxAdapterPth_Init();
	mSyZinx->mSyPth = pth;

	//初始化任务适配器
	SyZinxAdapterMission* pMiss = new IChannel;
	mSyZinx->mSyMission = pMiss;

	//初始化资源管理器
	SyZinxAdapterResource* pRes = new SyZinxAdapterResource;
	pRes->SyZinxAdapterResource_Init(mSyZinx->PthNum);
	mSyZinx->mSyResource = pRes;

	//pth->SyZinxAdapter_Pth_Add();
	//线程适配器绑定资源管理器
	pth->mPthRes = pRes;
	pRes->mResPth = pth;

	pth->mSyZinx = mSyZinx;
	return 1;
}

void SyZinxKernel::SyZinxKernel_Run()
{
	try
	{

		int iRet = event_base_dispatch(mSyZinx->SyZinxBase);
		throw iRet;
	}
	catch (int err)
	{
		mSyZinx->mLog.Log(__FILE__, __LINE__, SyZinxLog::ERROR, err, "event_base_dispatch");
	}
}

void SyZinxKernel::SyZinxKernel_Close()
{
	//关闭
	evconnlistener_free(mSyZinx->SyZinxListen);
	event_base_free(mSyZinx->SyZinxBase);
}

void SyZinxKernel::SyZinxKernel_Client_Close(const int mClient_fd)
{
}


SyZinxRecycle::SyZinxRecycle()
{
}
SyZinxRecycle::~SyZinxRecycle()
{
	mLog.Log(__FILE__, __LINE__, SyZinxLog::ERROR, 0, "SyZinxRecycle delete all Res!");
	std::cout << "析构函数被触发" << std::endl;
	delete SyZinxKernel::mSyZinx;
	delete  SyZinxKernel::mSyZinx->mSyPth;
	delete SyZinxKernel::mSyZinx->mSyMission;
	delete SyZinxKernel::mSyZinx->mSyResource;
}
