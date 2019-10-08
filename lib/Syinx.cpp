#include "Syinx.h"
#include "SyAdapter.h"
#include "SyPthreadAdapter.h"
#include "SyTaskAdapter.h"
#include "SyPthreadPool.h"
#include "SyResAdapter.h"

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/un.h>

#ifdef SYINXMOD_ADD_MYSQL
#include <mysql/mysql.h>
#else
#endif 


using namespace std;

SyinxKernel* SyinxKernel::mSyinx = nullptr;

//设置默认为零
int SyinxKernel::SyDatabaseMod = 0;




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

	if (mSyinx->mShmData->ExitSignal == true)
	{
		SyinxKernel::SyinxKernel_Close();
	}

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
	//关闭监听
	evconnlistener_free(mSyinx->SyinxListen);
	event_base_free(mSyinx->SyinxBase);

	//free
	if (SyinxKernel::mSyinx->mSyPth != NULL)
	{
		SyinxKernel::mSyinx->mSyPth->SyinxAdapter_Pth_destroy();
		delete  SyinxKernel::mSyinx->mSyPth;
	}

	if (SyinxKernel::mSyinx->mSyResource != NULL)
	{
		SyinxKernel::mSyinx->mSyResource->SyinxAdapterResource_Free();
		delete SyinxKernel::mSyinx->mSyResource;
	}


	if (SyinxKernel::mSyinx != NULL)
		delete SyinxKernel::mSyinx;

	

	SyinxKernel::SyinxSyinxKernel_FreeShm();

	cout << "Syinx is close" << endl;
}

void SyinxKernel::SyinxKernel_Client_Close(const int mClient_fd)
{
}

void SyinxKernel::SyinxSyinxKernel_MakeShm()
{
	int shmid = shmget(SET_SHM_KEY, sizeof(SyinxKernelShmMsg), IPC_CREAT | IPC_EXCL | 0664);
	if (-1 == shmid)
	{
		perror("shmget is failed");
		exit(0);
	}
	SyinxKernel::mSyinx->ShmId = shmid;
	void* ShmData = shmat(shmid, NULL, 0);
	mSyinx->ShmData = ShmData;
	memset(ShmData, 0, sizeof(SyinxKernelShmMsg));
	auto mShmData = (SyinxKernelShmMsg*)ShmData;

	for (int i = 0; i < mSyinx->PthNum; ++i)
	{
		mShmData->threads[i] = mSyinx->mSyPth->mSyinxPriPthPool->threads[i];
	}

	mShmData->PthNum = mSyinx->PthNum;
	mShmData->SyinxKernewWork = true;
	mShmData->ExitSignal = false;
	strcpy(mShmData->IP, mSyinx->Server_Sockaddr.sin_addr.c_str());
	strcpy(mShmData->Port, mSyinx->Server_Sockaddr.Prot.c_str());

	mSyinx->mShmData = mShmData;
}


void SyinxKernel::SyinxSyinxKernel_FreeShm()
{
	if (shmctl(mSyinx->ShmId, IPC_RMID, NULL) == -1)
	{
		perror("shmctl delete is failed");
		return;
	}
}

void SyinxKernel::SyinxSyinxKernel_LocalSock()
{


}




void SyinxKernelWork::PrintfServerStatus()
{

	int shmid = shmget(SET_SHM_KEY, 0, 0);
	if (-1 == shmid)
	{
		perror("Syinx is not run");
		exit(0);
	}
	void* ShmData = shmat(shmid, NULL, SHM_RDONLY);
	auto mShmData = (SyinxKernelShmMsg*)ShmData;
	if (mShmData->SyinxKernewWork)
	{
		printf("\n");
		cout << "Syinx-status:" << "\033[32;1m  run  \033[0m" << endl;
	}
	else
	{
		
		cout << "Syinx-status:" << "\033[31;1m  close  \033[0m" << endl;
		exit(0);
	}
	printf("IP:[%s] Prot:[%s]\n",mShmData->IP, mShmData->Port);
	printf("Total server connections : [%d]\n", mShmData->AllClientNum);
	printf("--------------------------------------------------------------------------\n");
	printf("WorkTid           Status            Connects           Capacity\n");
	for (int i = 0; i < mShmData->PthNum; ++i)
	{
		printf("%X           run                %d                   \n", mShmData->threads[i], mShmData->CurrentClientNum[i]);
	}
	
	printf("\n");
	if (shmdt(ShmData) == -1)
	{
		perror("shmdt is failed");
		return;
	}
}
void SyinxKernelWork::Makedaemon(int Prot)
{
	int pid_t;
	pid_t = fork();
	if (-1 == pid_t)
	{
		perror("ERR : fork()");
		exit(0);
	}

	else if (pid_t > 0)
		exit(0);

	setsid();
	chdir("./");
	umask(0);

	SyinxKernel::SyinxKernel_Init(Prot);
	this->mSyinx = SyinxKernel::mSyinx;
	this->SyinxKernelStatus = true;
	SyinxKernel::SyinxSyinxKernel_MakeShm();
	SyinxKernel::SyinxKernel_Run();


}
void SyinxKernelWork::SyinxExit()
{
	int shmid = shmget(SET_SHM_KEY, 0, 0);
	if (-1 == shmid)
	{
		perror("Syinx is not run");
		exit(0);
	}
	void* ShmData = shmat(shmid, NULL, 0);
	auto mShmData = (SyinxKernelShmMsg*)ShmData;

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(Port);
	inet_pton(AF_INET, "192.168.12.31", &addr.sin_addr.s_addr);

	int _fd = socket(AF_INET, SOCK_STREAM, 0);

	int iRet = connect(_fd, (const sockaddr*)& addr, sizeof(addr));
	mShmData->ExitSignal = true;
	close(_fd);

	if (shmdt(ShmData) == -1)
	{
		perror("shmdt is failed");
		return;
	}


}
SyinxKernelWork::SyinxKernelWork(int Prot, int argc, char* argv[])
{
	if (argc != 2)
	{
		cout << "ERR : Syinx Uncarried parameter" << endl;
		exit(0);
	}
	this->Port = Prot;
	if (!strcmp(COMMMEND_PARAM, "run"))
	{
		this->Makedaemon(Prot);
	}
	else if (!strcmp(COMMMEND_PARAM, "-s"))
	{
		//打印状态
		this->PrintfServerStatus();
		exit(0);
	}
	else if (!strcmp(COMMMEND_PARAM, "-v") | !strcmp(COMMMEND_PARAM, "version"))
	{
		cout << "Syinx Version:0.1.4" << endl;
		exit(0);
	}
	else if (!strcmp(COMMMEND_PARAM, "-c") | !strcmp(COMMMEND_PARAM, "close"))
	{
		this->SyinxExit();
		exit(0);
	}
	else
	{
		cout << "ERR : Syinx Error carrying parameter" << endl;
		exit(0);
	}

}

SyinxKernelWork::~SyinxKernelWork()
{
	
}

