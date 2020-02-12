#include "../Syinx/SyInc.h"
#include "SyPthreadPool.h"
#include "../Syinx/Syinx.h"
#include "../Sylog/SyLog.h"
#include "../Sylog/easylogging++.h"


SyinxPthreadPool& g_SyinxPthPool = SyinxPthreadPool::MakeSingleton();

SyinxPthreadPool::SyinxPthreadPool()
{
	PthPool = nullptr;
}

SyinxPthreadPool::~SyinxPthreadPool()
{
	PthPool = nullptr;
}

SyinxPthreadPool& SyinxPthreadPool::MakeSingleton()
{
	static SyinxPthreadPool mPool;
	return mPool;
}


void* SyPthreadPool_thread(void* dst);

bool SyPthreadPool_free(threadpool_t* pool);


void* SyPthreadPool_thread(void* dst)
{
	auto Pthpool = (threadpool_t*)dst;

	SyPthreadPool_task_t PthTask;

	char _buf[1024] = { 0 };
	sprintf(_buf, "thread EVENT: %ld is Start!", pthread_self());
	pthread_t upthread_t = pthread_self();
	LOG(INFO) << _buf;
	while (true)
	{
		pthread_mutex_lock(&Pthpool->Pthlock);

		while (Pthpool->count == 0 && Pthpool->PthpoolClose == false)
		{
			pthread_cond_wait(&Pthpool->Pthcond, &Pthpool->Pthlock);
		}

		if (Pthpool->count == Pthpool->queue_size)
		{
			continue;
		}

		if (Pthpool->PthpoolClose)
		{
			break;
		}

		// get task queue
		PthTask.startfunc = Pthpool->Taskqueue[Pthpool->HeadIndex].startfunc;
		PthTask.arg = Pthpool->Taskqueue[Pthpool->HeadIndex].arg;

		// queue
		++Pthpool->HeadIndex;
		Pthpool->HeadIndex = (Pthpool->HeadIndex == Pthpool->queue_size) ? 0 : Pthpool->HeadIndex;
		--Pthpool->count;

		// unlock
		pthread_mutex_unlock(&(Pthpool->Pthlock));

		// Logic Function  do work 
		auto func = PthTask.startfunc;
		func(PthTask.arg);
		// (*(PthTask.startfunc))(PthTask.arg);


	}
	sprintf(_buf, "Pthread : %ld is Exit!",  pthread_self());
	SyinxLog::mLog.Log(__FILE__, __LINE__, SyinxLog::INFO, 1, _buf);

	std::cout << "pth exit" << std::endl;

	Pthpool->started--;
	//unlock
	pthread_mutex_unlock(&Pthpool->Pthlock);
	pthread_exit(NULL);
	return NULL;
}

// create pthread queue
threadpool_t* SyinxPthreadPool::threadpool_create(uint32_t thread_count, uint32_t queue_size, uint32_t flags)
{

	SyinxPthreadPool::PthPool = new threadpool_t;
	
	int iRet = 0;
	//init
	PthPool->threads = new pthread_t[thread_count];
	PthPool->thread_count = thread_count;
	PthPool->queue_size = queue_size;

	PthPool->count = PthPool->HeadIndex = PthPool->TailIndex = 0;
	PthPool->PthpoolClose = false;
	PthPool->started = 0;
	//init queue
	PthPool->Taskqueue = new SyPthreadPool_task_t[queue_size];

	//init mutex
	iRet = pthread_mutex_init(&PthPool->Pthlock, NULL);
	if (iRet != 0)
	{
		return NULL;
	}
	iRet = pthread_cond_init(&PthPool->Pthcond, NULL);
	if (iRet != 0)
	{
		return NULL;
	}
	if (
		PthPool->threads == NULL ||
		PthPool->Taskqueue == NULL
		)
	{
		SyPthreadPool_free(PthPool);
	}
	pthread_attr_t Pthattr;
	iRet = pthread_attr_init(&Pthattr);
	if (iRet != 0)
	{
		return NULL;
	}
	// set detach
	pthread_attr_setdetachstate(&Pthattr, PTHREAD_CREATE_DETACHED);

	for (uint32_t i = 0; i < PthPool->thread_count; ++i)
	{
		int iRet = pthread_create(&PthPool->threads[i], &Pthattr, SyPthreadPool_thread, (void*)PthPool);
		if (iRet != 0)
		{

			SyinxLog::mLog.Log(__FILE__, __LINE__, SyinxLog::LOGERROR, iRet, "pthread_create is failed");
			return NULL;
		}
		PthPool->started += 1;
	}
	return PthPool;
}



uint32_t SyinxPthreadPool::threadpool_add(threadpool_t* pool, void* (*callback)(void*), void* arg, uint32_t flags)
{
	int _Index = 0;
	if (pool == NULL || callback == NULL)
	{
		return VarIsNULL;
	}
	int iRet = 0;

	iRet = pthread_mutex_lock(&PthPool->Pthlock);
	if (iRet != 0)
	{
		return LockErr;
	}

	_Index = PthPool->TailIndex + 1;
	_Index = (_Index == PthPool->queue_size) ? 0 : _Index;
	do
	{
		//queue max
		if (PthPool->queue_size == PthPool->count)
		{
			return QueueIsMax;
		}
		if (PthPool->PthpoolClose)
		{
			iRet = SyinxPthreadPool::threadpool_destroy(SyinxPthreadPool::PthPool, 0);
			if (iRet == Success)
				return Success;
			else
				return iRet;
		}

		PthPool->Taskqueue[PthPool->TailIndex].startfunc = callback;
		PthPool->Taskqueue[PthPool->TailIndex].arg = arg;

		PthPool->TailIndex = _Index;

		++PthPool->count;

		iRet = pthread_cond_signal(&PthPool->Pthcond);
		if (iRet != 0)
		{
			return CondSignalErr;
		}

	} while (0);
	pthread_mutex_unlock(&PthPool->Pthlock);

	return Success;
}

uint32_t SyinxPthreadPool::threadpool_destroy(threadpool_t* pool, uint32_t flags)
{
	int iRet = 0;
	if (pool == NULL)
	{
		return VarIsNULL;
	}

	iRet = pthread_mutex_lock(&PthPool->Pthlock);
	if (iRet != 0)
	{
		return LockErr;
	}

	//rouse all pthread
	iRet = pthread_cond_broadcast(&PthPool->Pthcond);
	if (iRet != 0)
	{
		return CondSignalErr;
	}

	pthread_mutex_unlock(&PthPool->Pthlock);

	return SyPthreadPool_free(PthPool);
}

bool SyPthreadPool_free(threadpool_t* pool)
{
	if (pool == NULL)
	{
		return false;
	}

	//delete 
	delete[] pool->threads;
	pool->threads = NULL;
	delete[] pool->Taskqueue;
	pool->Taskqueue = NULL;
	
	if (pool->threads)
	{
		pthread_mutex_destroy(&pool->Pthlock);
		pthread_cond_destroy(&pool->Pthcond);
	}
	delete pool;

	return true;

}
