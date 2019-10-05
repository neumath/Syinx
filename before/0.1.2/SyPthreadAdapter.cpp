
#include "Syinx.h"
#include "SyAdapter.h"
#include "SyPthreadPool.h"
#include "SyResAdapter.h"
#include "SyPthreadAdapter.h"

#include <unistd.h>
#include <vector>
#include <map>
#include <iostream>
SyinxAdapterPth::SyinxAdapterPth()
{
	
}

SyinxAdapterPth::~SyinxAdapterPth()
{
	SyinxPthreadPool::threadpool_destroy(mSyinxPthPool);
}

//线程任务函数
void *Pthread_ProcessCb(void* arg)
{
	struct PthTransmitMsg* ptMsg = (PthTransmitMsg*)arg;
	auto base = ptMsg->sBase;

	//遍历
	event_base_dispatch(base);
	std::cout << "exit" << std::endl;
}

int SyinxAdapterPth::SyinxAdapterPth_Init()
{
	int PthNum = this->PthNum;
	try
	{
		
		this->mSyinxPthPool= SyinxPthreadPool::threadpool_create(PthNum, PthNum, PthNum);
		if (this->mSyinxPthPool == NULL)
			throw -1;
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
