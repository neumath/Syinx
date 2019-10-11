#include <unistd.h>
#include <vector>
#include <map>
#include <iostream>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "Syinx.h"
#include "SyAdapter.h"
#include "SyResAdapter.h"
#include "SyPthreadPool.h"
#include "SyPthreadAdapter.h"
SyinxAdapterPth::SyinxAdapterPth()
{
	
}

SyinxAdapterPth::~SyinxAdapterPth()
{
	//SyinxPthreadPool::threadpool_destroy(mSyinxPthPool);
}
/*------------------------主线程函数---------------------------*/
void* PrincipalPth_Cb(void* dst);
int SyinxAdapterPth::SyinxAdapterPriPth_Init()
{
	auto PriPthPool = SyinxPthreadPool::PrincipalPth_create(this->PthNum);
	this->mSyinxPriPthPool = PriPthPool;
}


int SyinxAdapterPth::SyinxAdapterPriPth_Setcb(std::vector<event_base*>* BaseVec, int flags, int locaiont)
{
	if (NULL == BaseVec)
	{
		//log
		return -1;
	}
	int iRet = 0;
	try
	{
		iRet = SyinxPthreadPool::PrincipalPth_funcbind(this->mSyinxPriPthPool, PrincipalPth_Cb, BaseVec, flags, locaiont);
		throw(iRet);
	}
	catch (int i)
	{
		if (i <= 0)
			/*log*/;
	}

	return 1;
}

int SyinxAdapterPth::SyinxAdapterPriPth_Run()
{
	SyinxPthreadPool::PrincipalPth_add(this->mSyinxPriPthPool, 0);
	return 0;
}

void* PrincipalPth_Cb(void* dst)
{
	//test
	printf("\npth is work pid is : %X.............Ok\n", (unsigned int)pthread_self());
	auto base = (event_base*)dst;
	std::cout << "pth save base index : " << base << std::endl;

	int shmid = shmget(SET_SHM_KEY, 0, 0);
	if (-1 == shmid)
	{
		printf("pth %X open shm is failed\n", pthread_self());
	}
	void* ShmData = shmat(shmid, NULL, 0);
	if (NULL == ShmData)
	{
		printf("pth %X shmat is failed\n", pthread_self());
	}

	//设置当前线程的启动状态
	auto mShmData = (SyinxKernelShmMsg*)ShmData;
	int _Index = 0;
	while (true)
	{
		if (mShmData->threads[_Index] != pthread_self())
			++_Index;
		else
			break;
	}
	//设置线程工作状态

	int iRet;
	do {
		mShmData->mPthStatus[_Index] = PthRun;
		iRet = event_base_dispatch(base);
		//iRet = event_base_loop(base, EVLOOP_ONCE );
		if (iRet < 0)
		{
			break;
		}
		sleep(1);
		mShmData->mPthStatus[_Index] = PthWait;
	} while (iRet >=0);

	mShmData->mPthStatus[_Index] = PthExit;
	shmdt(ShmData);

	printf("pth is exit pid is :%X\n", (unsigned int)pthread_self());
	pthread_exit(0);
}

/*-----------------------------次要线程函数----------------------------*/
void* Pthread_ProcessCb(void* arg)
{

}


int SyinxAdapterPth::SyinxAdapterPth_Init()
{
	int PthNum = this->PthNum;
	try
	{
		
		/*this->mSyinxPthPool= SyinxPthreadPool::threadpool_create(PthNum, PthNum, PthNum);
		if (this->mSyinxPthPool == NULL)
			throw -1;*/
	}
	catch (int err)
	{
		mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, -1, "threadpool_create failed ");
	}
	


	return 0;
}

int SyinxAdapterPth::SyinxAdapter_Pth_Add()
{
	
	
	for (size_t i = 0; i < PthNum; i++)
	{
		
	}
	return 0;
}

int SyinxAdapterPth::SyinxAdapter_Pth_destroy()
{
	int iRet= SyinxPthreadPool::threadpool_destroy(this->mSyinxPthPool);
	if (-1 == iRet)
	{
		mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, -1, "threadpool_destroy failed ");
	}
}

int SyinxAdapterPth::getPthNum() const
{
	return PthNum;
}
