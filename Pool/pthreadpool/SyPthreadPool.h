#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

/*主要线程宏*/
//主线程
#define PRINCIPAL_PTHREAD                                public
//次要线程
#define MINOR_PTHREAD                                    public

/*设置线程回调函数方案*/
//set flags call_back func is equall
#define SET_PTHPOOLCB_EQUALL                                 0x01
//set flags call_back func is unlike
#define SET_PTHPOLLCB_UNLIKE                                 0x02

/*线程工作状态的宏*/
class SyinxKernel;
class SyinxLog;
struct threadpool_t;
struct Principal_Pth;


enum SyPthreadPoolErr
{
	Success = 1,
	//锁失败
	MutexInitErr,
	CondInitErr,
	LockErr,
	UnLockErr,
	CondWaitErr,
	CondSignalErr,

	DeleteErr,
	DistoryErr,             /*释放失败*/
	VarIsNULL,              /*传递变量为空*/
	QueueIsMax,        /*任务队列满了*/

	Shutdown = 404,         /*关闭*/

};

//任务队列回调函数及其参数
typedef struct {
	void* (*startfunc)(void*);
	void* arg;
} SyPthreadPool_task_t;

struct threadpool_t {
	pthread_mutex_t                     Pthlock;    /*用于内部工作的互斥锁*/
	pthread_cond_t                      Pthcond;    /*线程间通知的条件变量*/
	pthread_t* threads;                             /*线程数组，这里用指针来表示*/

	SyPthreadPool_task_t* Taskqueue;                /*存储任务的数组，即任务队列*/
	int queue_size;                                 /*任务队列最大数量*/
	int count;                                      /*当前任务数量*/
	int HeadIndex;                                  /*任务队列头*/
	int TailIndex;                                  /*任务队列尾*/


	int thread_count;                               /*线程数量*/

	bool shutdown;                                   /*标识线程池是否关闭*/
	int  started;                                    /*开启的线程数*/
};


class SyinxPthreadPool
{
	enum PrincipalPthWorkStatus
	{
		PriPthWork=1,
		PriPthWait,
		PRiPthClose,
		PriPthOther,
	};
public:
	SyinxPthreadPool();
	~SyinxPthreadPool();

	//线程池函数
	/*
	@function threadpool_create
	 创建一个threadpool_t对象。
	 @param thread_count为线程池的容量,线程数量
	 @param queue_size队列最大数量
	 @param flags为占位参数,待后续使用。
	 @返回一个新创建的线程池或NULL
	失败返回NULL
	成功返回线程池结构体
	*/
	static threadpool_t* threadpool_create(int thread_count, int queue_size, int flags);

	/*
	@function threadpool_add
	 将函数以及参数添加到任务队列
	 @param pool为先线程池地址
	 @param void* (*callback)(void*)为回调函数
	 @param arg传入参数
	 @param flags为占位参数,待后续使用。
	失败返回SyPthreadPoolErr枚举
	成功返回Success
	*/
	static	int threadpool_add(threadpool_t* pool, void* (*callback)(void*), void* arg, int flags);

	/*
	@function threadpool_destroy
	 销毁pool线程池
	失败返回SyPthreadPoolErr枚举
	成功返回Success
	*/
	static  int threadpool_destroy(threadpool_t* pool, int flags);


	static threadpool_t* PthPool;
};


#endif
