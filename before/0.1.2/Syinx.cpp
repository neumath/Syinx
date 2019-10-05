#include "Syinx.h"
#include "SyPthreadAdapter.h"
#include "SyTaskAdapter.h"
#include "SyResAdapter.h"
#include <iostream>
#include <fstream>
#include <istream>
using namespace std;

SyinxKernel* SyinxKernel::mSyinx = nullptr;


SyinxRecycle recycle;


//listen回调函数传入的参数
struct  SyinxDeliver
{

	//base句柄
	struct event_base* iSyinxBase;

	//保存用于监听套接字的evconnlistener
	struct evconnlistener* iSyinxListen;

	//核心框架地址
	SyinxKernel* iSyinx;
};

SyinxKernel::SyinxKernel()
{
	
}
SyinxKernel::~SyinxKernel()
{
}

//如果有客户端连接
void SyinxKernel_Listen_CB(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* arg)
{
	SyinxDeliver* poSyinxDeliver = (SyinxDeliver*)arg;
	if (NULL == poSyinxDeliver)
	{
		return;
	}

	struct event_base* poSyinxBase = poSyinxDeliver->iSyinxBase;

	//框架地址
	SyinxKernel* mSyinx = poSyinxDeliver->iSyinx;

	//test
	std::cout << "new client is connect  " << "Client:" << fd << std::endl;

	//将客户端上到map集群
	mSyinx->mSyResource->SyinxAdapterResource_AllotClient(fd);
	
	

}
int SyinxKernel::SyinxKernel_Init(const short _inPort)
{
	if (_inPort <= 0 || _inPort >= 65535)
	{
		return -1;
	}
	SyinxKernel::mSyinx = new SyinxKernel;

	//初始化结构体
	struct sockaddr_in _Serveraddr;
	memset(&_Serveraddr, 0, sizeof _Serveraddr);
	_Serveraddr.sin_family = AF_INET;
	_Serveraddr.sin_port = htons(_inPort);
	

	
	//创建句柄
	mSyinx->SyinxBase = event_base_new();
	if (NULL == mSyinx->SyinxBase)
	{
		return -1;
	}
	//设置传递参数
	SyinxDeliver* poSyinxDeliver = new SyinxDeliver;
	poSyinxDeliver->iSyinxBase = mSyinx->SyinxBase;
	poSyinxDeliver->iSyinxListen = mSyinx->SyinxListen;
	poSyinxDeliver->iSyinx = mSyinx;

	
	mSyinx->Server_Sockaddr.family = _Serveraddr.sin_family;
	mSyinx->Server_Sockaddr.Prot = to_string(ntohs(_Serveraddr.sin_port));
	mSyinx->Server_Sockaddr.sin_addr = inet_ntoa(_Serveraddr.sin_addr);

	//初始化线程适配器
	SyinxAdapterPth* pth = new SyinxAdapterPth(mSyinx->PthNum);
	pth->SyinxAdapterPth_Init();
	mSyinx->mSyPth = pth;

	//初始化任务适配器
	SyinxAdapterMission* pMiss = new IChannel;
	mSyinx->mSyMission = pMiss;

	//初始化资源管理器
	SyinxAdapterResource* pRes = new SyinxAdapterResource;
	pRes->SyinxAdapterResource_Init(mSyinx->PthNum);
	mSyinx->mSyResource = pRes;

	//pth->SyinxAdapter_Pth_Add();
	//线程适配器绑定资源管理器
	pth->mPthRes = pRes;
	pRes->mResPth = pth;

	pth->mSyinx = mSyinx;

	//设置监听
	mSyinx->SyinxListen = evconnlistener_new_bind(mSyinx->SyinxBase, SyinxKernel_Listen_CB, (void*)poSyinxDeliver,
		SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING, 10, (const sockaddr*)& _Serveraddr, sizeof(_Serveraddr));
	if (mSyinx->SyinxListen == NULL)
	{
		return -1;
	}
	return 1;
}

void SyinxKernel::SyinxKernel_Run()
{
	try
	{

		int iRet = event_base_dispatch(mSyinx->SyinxBase);
		throw iRet;
	}
	catch (int err)
	{
		mSyinx->mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, err, "event_base_dispatch");
	}
}

void SyinxKernel::SyinxKernel_Close()
{
	//关闭
	evconnlistener_free(mSyinx->SyinxListen);
	event_base_free(mSyinx->SyinxBase);
}

void SyinxKernel::SyinxKernel_Client_Close(const int mClient_fd)
{
}


SyinxRecycle::SyinxRecycle()
{
}
SyinxRecycle::~SyinxRecycle()
{
	mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, 0, "SyinxRecycle delete all Res!");
	std::cout << "析构函数被触发" << std::endl;
	delete SyinxKernel::mSyinx;
	delete  SyinxKernel::mSyinx->mSyPth;
	delete SyinxKernel::mSyinx->mSyMission;
	delete SyinxKernel::mSyinx->mSyResource;
}
