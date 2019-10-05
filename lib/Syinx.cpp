#include "Syinx.h"
#include "SyAdapter.h"
#include "SyPthreadAdapter.h"
#include "SyTaskAdapter.h"
#include "SyResAdapter.h"

#ifdef SYINXMOD_ADD_MYSQL
#include <mysql/mysql.h>
#else
#endif 

using namespace std;

SyinxKernel* SyinxKernel::mSyinx = nullptr;

//设置默认为零
int SyinxKernel::SyDatabaseMod = 0;

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

	//将将新来的客户端委托资源管理器来管理
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


	//设置监听
	mSyinx->SyinxListen = evconnlistener_new_bind(mSyinx->SyinxBase, SyinxKernel_Listen_CB, (void*)poSyinxDeliver,
		SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING, 10, (const sockaddr*)& _Serveraddr, sizeof(_Serveraddr));
	if (mSyinx->SyinxListen == NULL)
	{
		return -1;
	}

	//初始化适配器
	mSyinx->SyinxKernel_InitAdapter();
	return 1;
	
}

int SyinxKernel::SyinxKernel_InitAdapter()
{
	//初始化线程管理器
	SyinxAdapterPth *nSyPth = new SyinxAdapterPth(this->PthNum);

	//初始化资源管理器
	SyinxAdapterResource* nSyRes = new SyinxAdapterResource(this->PthNum);

	//初始化任务管理器
	SyinxAdapterMission* nSyTask = new IChannel;

	/*互相绑定*/
	//pth
	nSyPth->mPthRes = nSyRes;
	nSyPth->mSyinx = mSyinx;

	//res
	nSyRes->mResPth = nSyPth;
	nSyRes->mSyinx = mSyinx;
	nSyRes->mResTask = nSyTask;
	//msyinx
	mSyinx->mSyPth = nSyPth;
	mSyResource = nSyRes;
	mSyMission = nSyTask;
	int iRet = 0;
	//init pth
	iRet = mSyinx->mSyPth->SyinxAdapterPriPth_Init();
	if (iRet <= 0)
	{
		mSyinx->mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, iRet, "SyinxAdapterPriPth_Init is failed");
	}

	//init res
	iRet = mSyinx->mSyResource->SyinxAdapterResource_Init(this->PthNum);
	if (iRet <= 0)
	{
		mSyinx->mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, iRet, "SyinxAdapterResource_Init is failed");
	}

	//procedure init
	mSyinx->SyinxKernel_Procedureinit();
	return 1;
}

int SyinxKernel::SyinxKernel_Procedureinit()
{
	mSyinx->mSyPth->SyinxAdapterPriPth_Setcb(&(mSyinx->mSyResource->mSyBaseVec), SET_PTHPOOLCB_EQUALL, 1);
	mSyinx->mSyPth->SyinxAdapterPriPth_Run();
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

void Printfinterface()
{


	printf("---------------------------------------------------------------\n");
	printf("                Welcome To Use Syinx \n");
	printf("---------------------------------------------------------------\n");
	if (true)
	{
		//绿色状态
		cout << "Syinx-status:" << "\033[32;1m  run  \033[0m";
	}
	else
	{
		//红色状态
		cout << "Syinx-status:" << "\033[31;1m  close  \033[0m";
	}
	printf("Version:0.1.1 IP:[%s] Prot:[%s]\n");

	printf("WorkPid        state        Send      Recv        active\n");
	for (int i = 0; i < 4; ++i)
	{

	}

}
SyinxKernelWork::SyinxKernelWork(int Prot)
{
	SyinxKernel::SyinxKernel_Init(Prot);
	Printfinterface();
	SyinxKernel::SyinxKernel_Run();
}

int SyinxKernelWork::SyinxClose()
{
	SyinxKernel::SyinxKernel_Close();
}
