#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_


class SyinxKernel;
class SyinxLog;
class SyinxAdapterPth;
struct threadpool_t;
struct Principal_Pth;


typedef struct {
	void* (*startfunc)(void*);
	void* arg;
} SyPthreadPool_task_t;

struct threadpool_t {

	pthread_mutex_t					Pthlock;									//	用于内部工作的互斥锁
	pthread_cond_t					Pthcond;									//	线程间通知的条件变量
	pthread_t*						threads;									//	线程数组，这里用指针来表示
																				
	SyPthreadPool_task_t*			Taskqueue;									//	存储任务的数组，即任务队列
	int								queue_size;									//	任务队列最大数量
	int								count;										//	当前任务数量
	int								HeadIndex;									//	任务队列头
	int								TailIndex;									//	任务队列尾
																				
																				
	int								thread_count;								//	线程数量
																				
	bool							PthpoolClose;								//	标识线程池是否关闭
	int								started;									//	开启的线程数
	int								WorkStatus;									//	工作状态
};

class SyinxPthreadPool
{
public:
	enum SyPthreadPoolErr
	{
		PoolErr = 0,
		Success = 1,
		MutexInitErr = 2,							//初始化互斥锁失败
		CondInitErr = 3,							//条件标量初始化失败
		LockErr = 4,
		UnLockErr = 5,
		CondWaitErr = 6,
		CondSignalErr = 7,
		DeleteErr = 8,
		DistoryErr = 9,								//释放失败
		VarIsNULL = 10,								//传递变量为空
		QueueIsMax = 11,							//任务队列满了
		PthShutdown = 12,							//关闭
	};
	enum PrincipalPthWorkStatus
	{
		PriPthWork = 1,
		PriPthWait = 2,
		PRiPthClose = 3,
		PriPthOther = 4,
	};
	SyinxPthreadPool();
	~SyinxPthreadPool();

	static SyinxPthreadPool& MakeSingleton();

	inline threadpool_t* GetPthreadPool()
	{
		if (PthPool != nullptr)
			return PthPool;
		else
			return nullptr;
	}
	threadpool_t* threadpool_create(uint32_t thread_count, uint32_t queue_size, uint32_t flags = 0);

	uint32_t threadpool_add(threadpool_t* pool, void* (*callback)(void*), void* arg, uint32_t flags= 0);

	uint32_t  threadpool_destroy(threadpool_t* pool, uint32_t flags = 0);

private:
	threadpool_t* PthPool;
};

extern SyinxPthreadPool& g_SyinxPthPool;

#endif

