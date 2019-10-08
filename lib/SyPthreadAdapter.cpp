#include <unistd.h>
#include <vector>
#include <map>
#include <iostream>
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
	int iRet;
	do {
		iRet = event_base_dispatch(base);
		//iRet = event_base_loop(base, EVLOOP_ONCE );
		if (iRet < 0)
		{
			break;
		}
		std::cout << iRet << std::endl;
		sleep(1);
		printf("pth is wait pid is :%X\n", (unsigned int)pthread_self());
		
			
	} while (iRet >=0);

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
