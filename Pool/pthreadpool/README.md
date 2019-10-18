# PthreadPool

### 一: 简介

> 当前版本:0.1

该线程池配合Syinx与本人开发, 无任何其他开源代码,也可以作为单独的模块拿来使用.版权请详见主页LICENSE



### 二: 如何使用

```c++
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
```

### 三: 错误枚举

```c++
enum SyPthreadPoolErr
{
	Success = 1,
	//锁失败
	MutexInitErr,
	CondInitErr,
	LockErr,              /*线程上锁失败*/
	UnLockErr,            /*线程解锁失败*/
	CondWaitErr,          /*阻塞线程失败*/
	CondSignalErr,        /*信号唤醒线程失败*/

	DeleteErr,            /*销毁线程池失败*/
	DistoryErr,             /*释放失败*/
	VarIsNULL,              /*传递变量为空*/
	QueueIsMax,             /*任务队列满了*/

	Shutdown = 404,         /*关闭*/

};
```



