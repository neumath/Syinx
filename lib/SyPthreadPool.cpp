#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <vector>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <iostream>
using namespace std;
#include "SyPthreadPool.h"
#include "Syinx.h"


#define DEFAULT_TIME 10                 /*10s检测一次*/
#define MIN_WAIT_TASK_NUM 10            /*如果queue_size > MIN_WAIT_TASK_NUM 添加新的线程到线程池*/ 
#define DEFAULT_THREAD_VARY 10          /*每次创建和销毁线程的个数*/
#define true 1
#define false 0



/*次要线程函数----------------------------*/


SyinxPthreadPool::SyinxPthreadPool()
{
}

SyinxPthreadPool::~SyinxPthreadPool()
{
}



/*次要工作线程*/
//创建线程池
threadpool_t* SyinxPthreadPool::PthPool = nullptr;

//typedef struct {
//	void* (*function)(void*);
//	void* argument;
//} SyPthreadPool_task_t;
//
//struct threadpool_t {
//	pthread_mutex_t                     lock;           /*用于内部工作的互斥锁*/
//	pthread_cond_t                      notify;         /*线程间通知的条件变量*/
//	pthread_t* threads;        /*线程数组，这里用指针来表示*/
//
//	std::queue<SyPthreadPool_task_t*>   Taskqueue;      /*存储任务的数组，即任务队列*/
//
//	int thread_count;                               /*线程数量*/
//	int queue_size;                                 /*任务队列最大数量*/
//
//	int count;                                      /*当前任务数量*/
//	int shutdown;                                   /*标识线程池是否关闭*/
//	int started;                                    /*开启的线程数*/
//};

//声明线程池处理函数
void* SyPthreadPool_thread(void* dst);

int SyPthreadPool_free(threadpool_t* pool);

//线程池函数
void* SyPthreadPool_thread(void* dst)
{
	auto Pthpool = (threadpool_t*)dst;

	SyPthreadPool_task_t PthTask;

	char _buf[BUFSIZ] = { 0 };
	sprintf(_buf, "Pthread EVENT: %ld is Start!", pthread_self());
	SyinxLog::mLog.Log(__FILE__, __LINE__, SyinxLog::INFO, 1, _buf);

	while (1)
	{
		pthread_mutex_lock(&Pthpool->Pthlock);

		//锁住线程如果没有任务就永远阻塞,直到信号来临
		while (Pthpool->count == 0 && Pthpool->shutdown == false)
		{
			pthread_cond_wait(&Pthpool->Pthcond, &Pthpool->Pthlock);
		}

		if (Pthpool->count == Pthpool->queue_size)
		{
			break;
		}

		if (Pthpool->shutdown)
		{
			break;
		}

		//取得任务队列
		PthTask.startfunc = Pthpool->Taskqueue[Pthpool->HeadIndex].startfunc;
		PthTask.arg = Pthpool->Taskqueue[Pthpool->HeadIndex].arg;

		//如果headnindex到了最后,则变成从0开始
		Pthpool->HeadIndex += 1;
		Pthpool->HeadIndex = (Pthpool->HeadIndex == Pthpool->queue_size) ? 0 : Pthpool->HeadIndex;
		Pthpool->count -= 1;

		//解锁
		pthread_mutex_unlock(&(Pthpool->Pthlock));

		//运行任务队列函数
		auto func = PthTask.startfunc;
		func(PthTask.arg);
		//(*(PthTask.startfunc))(PthTask.arg);


	}
	sprintf(_buf, "Pthread %s: %ld is Exit!", "LogEvent_Error", pthread_self());
	SyinxLog::mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, 1, _buf);

	cout << "pth exit" << endl;
	//如果线程崩了
	Pthpool->started--;
	//解锁
	pthread_mutex_unlock(&Pthpool->Pthlock);
	pthread_exit(NULL);
	return NULL;
}

//线程池线程数,任务队列数
threadpool_t* SyinxPthreadPool::threadpool_create(int thread_count, int queue_size, int flags)
{
	SyinxPthreadPool::PthPool = new threadpool_t;

	int iRet = 0;
	//初始化线程策略
	PthPool->threads = new pthread_t[thread_count];
	PthPool->thread_count = thread_count;
	PthPool->queue_size = queue_size;

	PthPool->count = PthPool->HeadIndex = PthPool->TailIndex = 0;
	PthPool->shutdown = false;
	PthPool->started = 0;
	//初始化任务队列
	PthPool->Taskqueue = new SyPthreadPool_task_t[queue_size];

	//初始化锁锁,用于锁住本结构体
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
	//设置分离
	pthread_attr_setdetachstate(&Pthattr, PTHREAD_CREATE_DETACHED);

	for (size_t i = 0; i < PthPool->thread_count; ++i)
	{
		int iRet = pthread_create(&PthPool->threads[i], &Pthattr, SyPthreadPool_thread, (void*)PthPool);
		if (iRet != 0)
		{

			SyinxLog::mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, iRet, "pthread_create is failed");
			return NULL;
		}
		PthPool->started += 1;
	}
	return PthPool;
}

int SyinxPthreadPool::threadpool_add(threadpool_t* pool, void* (*callback)(void*), void* arg, int flags)
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
		//如果队列满了
		if (PthPool->queue_size == PthPool->count)
		{
			return QueueIsMax;
		}
		if (PthPool->shutdown)
		{
			iRet = SyinxPthreadPool::threadpool_destroy(SyinxPthreadPool::PthPool, 0);
			if (iRet == Success)
				return Success;
			else
				return iRet;
		}

		PthPool->Taskqueue[PthPool->TailIndex].startfunc = callback;
		PthPool->Taskqueue[PthPool->TailIndex].arg = arg;

		//尾插头拿先进先出
		PthPool->TailIndex = _Index;

		PthPool->count += 1;

		//发送一个signel信号唤醒线程
		iRet = pthread_cond_signal(&PthPool->Pthcond);
		if (iRet != 0)
		{
			return CondSignalErr;
		}

	} while (0);
	pthread_mutex_unlock(&PthPool->Pthlock);

	return Success;
}

int SyinxPthreadPool::threadpool_destroy(threadpool_t* pool, int flags)
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

	//唤醒所有线程
	iRet = pthread_cond_broadcast(&PthPool->Pthcond);
	if (iRet != 0)
	{
		return CondSignalErr;
	}

	pthread_mutex_unlock(&PthPool->Pthlock);

	return SyPthreadPool_free(PthPool);
}

int SyPthreadPool_free(threadpool_t* pool)
{
	if (pool == NULL)
	{
		return VarIsNULL;
	}

	//销毁当前threadpool_create
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

}
