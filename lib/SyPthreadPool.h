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

class SyinxLog;

typedef struct threadpool_t threadpool_t;
typedef struct Principal_Pth Principal_Pth;
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
	//主要线程
PRINCIPAL_PTHREAD:
	/*
	 @function PrincipalPth_create
	 创建一个Principal_Pth主要线程对象。
	 @param max_thr_num主要线程数量不要超过核心数
	 @返回一个新创建的线程池或NULL
	 注意:主要线程数量不要超过当前电脑核心数
	 */
	static Principal_Pth* PrincipalPth_create(int max_thr_num);

	/**
	* @function PrincipalPth_funcbind
	* @desc用于绑定主线程的回调函数以及万能指针
	* @param线程池，将任务添加到其中。
	* @param函数指向执行任务的函数。
	* @param参数传递给函数的参数。
	* @如果一切顺利，返回1，否则-1或者0
	如果不想设置回调函数是否一致,flags可以填0,location 可以填任意数
	否则location不能填超过当前工作主线程数
	 */
	static int PrincipalPth_funcbind(Principal_Pth* PrPth, void* (*function)(void* arg), std::vector<event_base*>* BaseVec,int flags, int location);

	/**
	* @function PrincipalPth_add
	* @descz在主线程里绑定后的回调函数进行处理,主线程将会执行所有的回调函数
	* @PrPth 是要执行任务的主线程池结构体
	* @如果一切顺利，返回1，否则-1或者0
	flags 待开发
	 */
	static int PrincipalPth_add(Principal_Pth* PrPth,int flags );

	/**
	* @function PrincipalPth_Destory
	* @desc停止并销毁线程池。
	* @param池要销毁线程池。
	* @如果destory成功返回0，则返回-1或者0
	 */
	static int PrincipalPth_Destory(Principal_Pth* PrPth);

//次要线程
MINOR_PTHREAD:


	/**
	 @function threadpool_create
	 创建一个threadpool_t对象。
	 @param min_thr_num最小线程数量
	 @param max_thr_num最大线程
	 @param queue_max_size队列的大小。
	 @返回一个新创建的线程池或NULL
	 */
	static threadpool_t* threadpool_create(int min_thr_num, int max_thr_num, int queue_max_size);

	/**
	* @function threadpool_add
	* @desc在线程池队列中添加一个新任务
	* @param线程池，将任务添加到其中。
	* @param函数指向执行任务的函数。
	* @param参数传递给函数的参数。
	* @如果一切顺利，返回0，否则-1
	 */
	static int threadpool_add(threadpool_t* pool, void* (*function)(void* arg), void* arg);

	/**
	* @function threadpool_destroy
	* @desc停止并销毁线程池。
	* @param池要销毁线程池。
	* @如果destory成功返回0，则返回-1	
	 */
	static int threadpool_destroy(threadpool_t* pool);

	/**
	* @desc获取线程号
	* @pool线程池
	* @返回线程的#
	 */
	static int threadpool_all_threadnum(threadpool_t* pool);

	/**
	获取繁忙线程号
	* @param池threadpool
	*返回繁忙线程的#
	 */
	static int threadpool_busy_threadnum(threadpool_t* pool);

};


#endif
