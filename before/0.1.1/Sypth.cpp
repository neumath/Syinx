#include "Adapter.h"
#include <unistd.h>

SyZinxAdapterPth::SyZinxAdapterPth()
{
	
}

SyZinxAdapterPth::~SyZinxAdapterPth()
{
	PthreadPool::threadpool_destroy(mSyZinxPthPool);
}

void *Pthread_ProcessCb(void* arg)
{
	printf("********************\n");
	printf("thread 0x%x working on task %d\n ", (unsigned int)pthread_self(), *(int*)arg);
	sleep(1);
	printf("task %d is end\n", *(int*)arg);
}
int SyZinxAdapterPth::SyZinxAdapterPth_Init()
{
	int PthNum = this->PthNum;
	try
	{
		
		this->mSyZinxPthPool= PthreadPool::threadpool_create(PthNum, PthNum, PthNum);
		if (this->mSyZinxPthPool == NULL)
			throw -1;
	}
	catch (int err)
	{
		mLog.Log(__FILE__, __LINE__, SyZinxLog::ERROR, -1, "threadpool_create failed ");
	}
	


	return 0;
}

int SyZinxAdapterPth::SyZinxAdapter_Pth_Add()
{
	printf("********************\n");
	for (size_t i = 0; i < PthNum; i++)
	{
		PthreadPool::threadpool_add(this->mSyZinxPthPool, Pthread_ProcessCb, (void*)&i);   /* 向线程池中添加任务 */
	}
	return 0;
}

int SyZinxAdapterPth::SyZinxAdapter_Pth_destroy()
{
	return 0;
}

int SyZinxAdapterPth::getPthNum() const
{
	return PthNum;
}
