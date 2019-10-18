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



/*-----------------------------次要线程函数----------------------------*/

int SyinxAdapterPth::SyinxAdapterPth_Init()
{
	this->mSyinxPthPool = SyinxPthreadPool::threadpool_create(this->PthPoolNum, this->TaskMaxNum, 0);
	if (this->mSyinxPthPool == NULL)
	{
		return -1;
	}
}

int SyinxAdapterPth::SyinxAdapter_Pth_Add(void* (*taskfunc)(void*), void* arg)
{
	return SyinxPthreadPool::threadpool_add(SyinxPthreadPool::PthPool, taskfunc, arg, 0);
}



int SyinxAdapterPth::SyinxAdapter_Pth_destroy()
{
	return SyinxPthreadPool::threadpool_destroy(this->mSyinxPthPool, 0);
}


