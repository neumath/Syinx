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
#include "SyPthreadPool.h"

#define DEFAULT_TIME 10                 /*10s检测一次*/
#define MIN_WAIT_TASK_NUM 10            /*如果queue_size > MIN_WAIT_TASK_NUM 添加新的线程到线程池*/ 
#define DEFAULT_THREAD_VARY 10          /*每次创建和销毁线程的个数*/
#define true 1
#define false 0


/*主线程回调结构体*/
typedef struct
{
	void* (*function)(void*);          /* 函数指针，回调函数 */
	void* arg;                          /* 上面函数的参数 */
} PrincipalPth_task_t;                   

//为主线程设置一个管理者线程
typedef struct
{
	void* (*function)(void*);          /* 函数指针，回调函数 */
	void* arg;                          /* 上面函数的参数 */
} PrincipalPth_adjust_t;

//主线程结构体
struct Principal_Pth
{
	//pthread_mutex_t lock;               /* 用于锁住本结构体 */
	//pthread_mutex_t thread_counter;     /* 记录忙状态线程个数de琐 -- busy_thr_num */

	pthread_t* threads;                 /* 存放线程池中每个线程的tid。数组 */
	pthread_t adjust_tid;               /* 存管理线程tid */

	PrincipalPth_task_t* task_queue;      /* 任务队列(数组首地址) */

	int* PthStatus;                     /*保存每一个线程的工作状态*/

	//uint64_t* PthRecvDataByte;              /*每一个线程接收数据量,单位:Byte(数组)*/
	//uint64_t* PthSendDataByte;              /*每一个线程发送数据量,单位:Byte(数组)*/

	int thr_num;                        /* 当前主线程数量 */
	int busy_thr_num;                   /* 忙状态线程个数 */
	int wait_exit_thr_num;              /* 要销毁的线程个数 */

	int queue_front;                    /* task_queue队头下标 */
	int queue_rear;                     /* task_queue队尾下标 */
	int queue_size;                     /* task_queue队中实际任务数 */
	int queue_max_size;                 /* task_queue队列可容纳任务数上限 */

	bool shutdown;                       /* 标志位，线程池使用状态，true或false */
};
/*主线程额外函数*/
//线程主线程回调函数
void* PrincipalPth_WrokCb(void* arg);
//释放主线程结构体
int PrincipalPth_free(Principal_Pth * PrPth);



/* 次线程结构体 */
typedef struct
{
	void* (*function)(void*);          /* 函数指针，回调函数 */
	void* arg;                          /* 上面函数的参数 */
} threadpool_task_t;                    /* 各子线程任务结构体 */

/* 次要线程结构体 */
struct threadpool_t 
{
    pthread_mutex_t lock;               /* 用于锁住本结构体 */    
    pthread_mutex_t thread_counter;     /* 记录忙状态线程个数de琐 -- busy_thr_num */

    pthread_cond_t queue_not_full;      /* 当任务队列满时，添加任务的线程阻塞，等待此条件变量 */
    pthread_cond_t queue_not_empty;     /* 任务队列里不为空时，通知等待任务的线程 */

    pthread_t *threads;                 /* 存放线程池中每个线程的tid。数组 */
    pthread_t adjust_tid;               /* 存管理线程tid */
    threadpool_task_t *task_queue;      /* 任务队列(数组首地址) */

    int min_thr_num;                    /* 线程池最小线程数 */
    int max_thr_num;                    /* 线程池最大线程数 */
    int live_thr_num;                   /* 当前存活线程个数 */
    int busy_thr_num;                   /* 忙状态线程个数 */
    int wait_exit_thr_num;              /* 要销毁的线程个数 */

    int queue_front;                    /* task_queue队头下标 */
    int queue_rear;                     /* task_queue队尾下标 */
    int queue_size;                     /* task_queue队中实际任务数 */
    int queue_max_size;                 /* task_queue队列可容纳任务数上限 */

    int shutdown;                       /* 标志位，线程池使用状态，true或false */
};
/*-------------------------主要工作线程------------------------------------------*/
Principal_Pth* SyinxPthreadPool::PrincipalPth_create(int max_thr_num)
{
	int iRet = 0;
	Principal_Pth* PrPth = NULL;
	PrPth = (Principal_Pth*)malloc(sizeof(Principal_Pth));
	if (PrPth == NULL)
	{
		//log
	}
	//初始化数组
	PrPth->threads = (pthread_t*)malloc(sizeof(pthread_t) * max_thr_num);
	memset(PrPth->threads, 0, sizeof(pthread_t) * max_thr_num);

	PrPth->thr_num = max_thr_num;
	PrPth->busy_thr_num = 0;
	PrPth->wait_exit_thr_num = 0;

	//初始化队列
	PrPth->queue_front = 0;
	PrPth->queue_rear = 0;
	PrPth->queue_max_size = max_thr_num;
	PrPth->queue_size = 0;
	PrPth->shutdown = false;

	//初始化接收以及发送数据量数组
	//PrPth->PthRecvDataByte = (uint64_t*)malloc(sizeof(uint64_t) * max_thr_num);
	//PrPth->PthSendDataByte = (uint64_t*)malloc(sizeof(uint64_t) * max_thr_num);

	//初始化每一个线程的工作状态数组
	PrPth->PthStatus = (int*)malloc(sizeof(int) * max_thr_num);

	//任务队列开辟空间
	PrPth->task_queue = (PrincipalPth_task_t*)malloc(sizeof(PrincipalPth_task_t) * max_thr_num);
	return PrPth;
#if 0
	pthread_attr_t Pthattr;

	pthread_attr_init(&Pthattr);
	//设置线程分离
	pthread_attr_setdetachstate(&Pthattr, PTHREAD_CREATE_DETACHED);

	for (int i = 0; i < max_thr_num; ++i)
	{
		pthread_create(&(PrPth->threads[i]), &Pthattr, PrincipalPth_WrokCb, (void*)PrPth);

	}

#endif
}
//绑定主线程池的回调函数
int SyinxPthreadPool::PrincipalPth_funcbind(Principal_Pth* PrPth, void* (*function)(void* arg), std::vector<event_base*>* BaseVec, int flags,int location)
{
	if (NULL == PrPth)
	{
		//log
		return -1;
	}
	if (flags == SET_PTHPOOLCB_EQUALL || flags == 0 )
	{
		for (int i = 0; i < PrPth->thr_num; ++i)
		{
			PrPth->task_queue[i].function = function;
			PrPth->task_queue[i].arg = (void*)(*BaseVec)[i];
			
		}
	}
	else if (flags == SET_PTHPOLLCB_UNLIKE)
	{
		if (location >= 0 || location < PrPth->thr_num)
		{
			PrPth->task_queue[location].function = function;
			PrPth->task_queue[location].arg = (void*)(*BaseVec)[location];
		}
	}
	return 1;
}
int SyinxPthreadPool::PrincipalPth_add(Principal_Pth* PrPth, int flags)
{
	if (NULL == PrPth)
		return -1;

	pthread_attr_t Pthattr;
	pthread_attr_init(&Pthattr);
	//设置线程分离
	pthread_attr_setdetachstate(&Pthattr, PTHREAD_CREATE_DETACHED);
	for (int i = 0; i < PrPth->thr_num; ++i)
	{
		pthread_create(&(PrPth->threads[i]), &Pthattr, PrPth->task_queue[i].function, PrPth->task_queue[i].arg);
		PrPth->PthStatus[i] = PriPthWork;
		//std::cout << PrPth->task_queue[i].function << std::endl;
	}
	return 1;
}

int SyinxPthreadPool::PrincipalPth_Destory(Principal_Pth* PrPth)
{
	if (PrPth->task_queue != NULL)
	{
		free(PrPth->task_queue);
	}
	if (PrPth->threads != NULL)
	{
		free(PrPth->threads);
	}
	if (PrPth->PthStatus != NULL)
	{
		free(PrPth->PthStatus);
	}
	free(PrPth);
	PrPth = NULL;
}


/*次要线程函数----------------------------*/
void *threadpool_thread(void *threadpool);

void *adjust_thread(void *threadpool);

int is_thread_alive(pthread_t tid);
int threadpool_free(threadpool_t *pool);

SyinxPthreadPool::SyinxPthreadPool()
{
}

SyinxPthreadPool::~SyinxPthreadPool()
{
}



/*次要工作线程*/
//创建线程池
//threadpool_create(3,100,100);  
threadpool_t * SyinxPthreadPool::threadpool_create(int min_thr_num, int max_thr_num, int queue_max_size)
{
    int i;
    threadpool_t *pool = NULL;
    do 
	{
        if((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL) 
		{  
            printf("malloc threadpool fail");
            break;                                      /*跳出do while*/
        }
		//初始化
        pool->min_thr_num = min_thr_num;
        pool->max_thr_num = max_thr_num;
        pool->busy_thr_num = 0;
        pool->live_thr_num = min_thr_num;               /* 活着的线程数 初值=最小线程数 */
        pool->wait_exit_thr_num = 0;
        pool->queue_size = 0;                           /* 有0个产品 */
        pool->queue_max_size = queue_max_size;
        pool->queue_front = 0;
        pool->queue_rear = 0;
        pool->shutdown = false;                         /* 不关闭线程池 */

        /* 根据最大线程上限数， 给工作线程数组开辟空间, 并清零 */
        pool->threads = (pthread_t *)malloc(sizeof(pthread_t)*max_thr_num); 
        if (pool->threads == NULL) 
		{
            printf("malloc threads fail");
            break;
        }
        memset(pool->threads, 0, sizeof(pthread_t)*max_thr_num);

        /* 队列开辟空间 */
        pool->task_queue = (threadpool_task_t *)malloc(sizeof(threadpool_task_t)*queue_max_size);
        if (pool->task_queue == NULL) 
		{
            printf("malloc task_queue fail\n");
            break;
        }

        /* 初始化互斥琐、条件变量 */
        if (pthread_mutex_init(&(pool->lock), NULL) != 0
                || pthread_mutex_init(&(pool->thread_counter), NULL) != 0
                || pthread_cond_init(&(pool->queue_not_empty), NULL) != 0
                || pthread_cond_init(&(pool->queue_not_full), NULL) != 0)
        {
            printf("init the lock or cond fail\n");
            break;
        }

		//启动工作线程
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        for (i = 0; i < min_thr_num; i++) 
		{
            pthread_create(&(pool->threads[i]), &attr, threadpool_thread, (void *)pool);/*pool指向当前线程池*/
            printf("start thread 0x%x...\n", (unsigned int)pool->threads[i]);
        }

		//创建管理者线程
        pthread_create(&(pool->adjust_tid), &attr, adjust_thread, (void *)pool);

        return pool;

    } while (0);

	/* 前面代码调用失败时,释放pool存储空间 */
    threadpool_free(pool);

    return NULL;
}

/* 向线程池中 添加一个任务 */
//threadpool_add(thp, process, (void*)&num[i]);   /* 向线程池中添加任务 process: 小写---->大写*/

int SyinxPthreadPool::threadpool_add(threadpool_t *pool, void*(*function)(void *arg), void *arg)
{
    pthread_mutex_lock(&(pool->lock));

    /* ==为真，队列已经满， 调wait阻塞 */
    while ((pool->queue_size == pool->queue_max_size) && (!pool->shutdown)) 
	{
        pthread_cond_wait(&(pool->queue_not_full), &(pool->lock));
    }

    if (pool->shutdown) 
	{
        pthread_cond_broadcast(&(pool->queue_not_empty));
        pthread_mutex_unlock(&(pool->lock));
        return 0;
    }

    /* 清空 工作线程 调用的回调函数 的参数arg */
    if (pool->task_queue[pool->queue_rear].arg != NULL) 
	{
        pool->task_queue[pool->queue_rear].arg = NULL;
    }

    /*添加任务到任务队列里*/
    pool->task_queue[pool->queue_rear].function = function;
    pool->task_queue[pool->queue_rear].arg = arg;
    pool->queue_rear = (pool->queue_rear + 1) % pool->queue_max_size;       /* 队尾指针移动, 模拟环形 */
    pool->queue_size++;

    /*添加完任务后，队列不为空，唤醒线程池中 等待处理任务的线程*/
    pthread_cond_signal(&(pool->queue_not_empty));
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}

/* 线程池中各个工作线程 */
void *threadpool_thread(void *threadpool)
{
    threadpool_t *pool = (threadpool_t *)threadpool;
    threadpool_task_t task;

    while (true) 
	{
        /* Lock must be taken to wait on conditional variable */
        /*刚创建出线程，等待任务队列里有任务，否则阻塞等待任务队列里有任务后再唤醒接收任务*/
        pthread_mutex_lock(&(pool->lock));

        /*queue_size == 0 说明没有任务，调 wait 阻塞在条件变量上, 若有任务，跳过该while*/
        while ((pool->queue_size == 0) && (!pool->shutdown)) 
		{  
			sleep(1);
            printf("thread 0x%x is waiting\n", (unsigned int)pthread_self());
           // pthread_cond_wait(&(pool->queue_not_empty), &(pool->lock));//暂停到这

            /*清除指定数目的空闲线程，如果要结束的线程个数大于0，结束线程*/
            if (pool->wait_exit_thr_num > 0) 
			{
                pool->wait_exit_thr_num--;

                //如果线程池里线程个数大于最小值时可以结束当前线程
                if (pool->live_thr_num > pool->min_thr_num) 
				{
                    printf("thread 0x%x is exiting\n", (unsigned int)pthread_self());
                    pool->live_thr_num--;
                    pthread_mutex_unlock(&(pool->lock));
					//pthread_detach(pthread_self());
                    pthread_exit(NULL);
                }
            }
        }//while

        /*如果指定了true，要关闭线程池里的每个线程，自行退出处理---销毁线程池*/
        if (pool->shutdown) 
		{
            pthread_mutex_unlock(&(pool->lock));
            printf("thread 0x%x is exiting\n", (unsigned int)pthread_self());
            //pthread_detach(pthread_self());
            pthread_exit(NULL);     /* 线程自行结束 */
        }

        /*从任务队列里获取任务, 是一个出队操作*/
        task.function = pool->task_queue[pool->queue_front].function;
        task.arg = pool->task_queue[pool->queue_front].arg;

        pool->queue_front = (pool->queue_front + 1) % pool->queue_max_size;       /* 出队，模拟环形队列 */
        pool->queue_size--;

        /*通知可以有新的任务添加进来*/
        pthread_cond_broadcast(&(pool->queue_not_full));

        /*任务取出后，立即将 线程池琐 释放*/
        pthread_mutex_unlock(&(pool->lock));

        /*执行任务*/ 
        printf("thread 0x%x start working\n", (unsigned int)pthread_self());
        pthread_mutex_lock(&(pool->thread_counter));                            /*忙状态线程数变量琐*/
        pool->busy_thr_num++;                                                   /*忙状态线程数+1*/
        pthread_mutex_unlock(&(pool->thread_counter));

        (*(task.function))(task.arg);                                           /*执行回调函数任务*/
        //task.function(task.arg);                                              /*执行回调函数任务*/

        /*任务结束处理*/ 
        printf("thread 0x%x end working\n", (unsigned int)pthread_self());
        pthread_mutex_lock(&(pool->thread_counter));
        pool->busy_thr_num--;                                       /*处理掉一个任务，忙状态数线程数-1*/
        pthread_mutex_unlock(&(pool->thread_counter));
    }

    pthread_exit(NULL);
}

/* 管理线程 */
void *adjust_thread(void *threadpool)
{
    int i;
    threadpool_t *pool = (threadpool_t *)threadpool;
    while (!pool->shutdown) 
	{

        sleep(DEFAULT_TIME);                                    /*定时 对线程池管理  10秒*/

        pthread_mutex_lock(&(pool->lock));
        int queue_size = pool->queue_size;                      /* 关注 任务数 */
        int live_thr_num = pool->live_thr_num;                  /* 存活 线程数 */
        pthread_mutex_unlock(&(pool->lock));

        pthread_mutex_lock(&(pool->thread_counter));
        int busy_thr_num = pool->busy_thr_num;                  /* 忙着的线程数 */
        pthread_mutex_unlock(&(pool->thread_counter));

        /* 创建新线程 算法： 任务数大于最小线程池个数, 且存活的线程数少于最大线程个数时 如：30>=10 && 40<100*/
        if (queue_size >= MIN_WAIT_TASK_NUM && live_thr_num < pool->max_thr_num) 
		{
            pthread_mutex_lock(&(pool->lock));  
            int add = 0;

            /*一次增加 DEFAULT_THREAD 个线程*/
            for (i = 0; i < pool->max_thr_num && add < DEFAULT_THREAD_VARY
                    && pool->live_thr_num < pool->max_thr_num; i++) 
			{
                if (pool->threads[i] == 0 || !is_thread_alive(pool->threads[i])) 
				{
                    pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void *)pool);
                    add++;
                    pool->live_thr_num++;
                }
            }

            pthread_mutex_unlock(&(pool->lock));
        }

        /* 销毁多余的空闲线程 算法：忙线程X2 小于 存活的线程数 且 存活的线程数 大于 最小线程数时*/
  //      if ((busy_thr_num * 2) < live_thr_num  &&  live_thr_num > pool->min_thr_num) 
		//{
  //          /* 一次销毁DEFAULT_THREAD个线程, 隨機10個即可 */
  //          pthread_mutex_lock(&(pool->lock));
  //          pool->wait_exit_thr_num = DEFAULT_THREAD_VARY;      /* 要销毁的线程数 设置为10 */
  //          pthread_mutex_unlock(&(pool->lock));

  //          for (i = 0; i < DEFAULT_THREAD_VARY; i++) 
		//	{
  //              /* 通知处在空闲状态的线程, 他们会自行终止*/
  //              pthread_cond_signal(&(pool->queue_not_empty));
  //          }
  //      }
    }

    return NULL;
}
//线程池的销毁
int SyinxPthreadPool::threadpool_destroy(threadpool_t *pool)
{
    int i;
    if (pool == NULL) 
	{
        return -1;
    }
    pool->shutdown = true;

    /*先销毁管理线程*/
    //pthread_join(pool->adjust_tid, NULL);

    for (i = 0; i < pool->live_thr_num; i++) 
	{
        /*通知所有的空闲线程*/
        pthread_cond_broadcast(&(pool->queue_not_empty));
    }

    /*for (i = 0; i < pool->live_thr_num; i++) 
	{
        pthread_join(pool->threads[i], NULL);
    }*/

	//线程池释放空间
    threadpool_free(pool);

    return 0;
}

int threadpool_free(threadpool_t *pool)
{
    if (pool == NULL) 
	{
        return -1;
    }

    if (pool->task_queue) 
	{
        free(pool->task_queue);
    }

    if (pool->threads) 
	{
        free(pool->threads);
        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_mutex_lock(&(pool->thread_counter));
        pthread_mutex_destroy(&(pool->thread_counter));
        pthread_cond_destroy(&(pool->queue_not_empty));
        pthread_cond_destroy(&(pool->queue_not_full));
    }

    free(pool);
    pool = NULL;

    return 0;
}

//当前存活的线程的个数
int SyinxPthreadPool::threadpool_all_threadnum(threadpool_t *pool)
{
    int all_threadnum = -1;
	//加锁  线程池
    pthread_mutex_lock(&(pool->lock));
    all_threadnum = pool->live_thr_num;
    pthread_mutex_unlock(&(pool->lock));

    return all_threadnum;
}
//获取当前忙状态的线程的个数
int SyinxPthreadPool::threadpool_busy_threadnum(threadpool_t *pool)
{
    int busy_threadnum = -1;

    pthread_mutex_lock(&(pool->thread_counter));
    busy_threadnum = pool->busy_thr_num;
    pthread_mutex_unlock(&(pool->thread_counter));

    return busy_threadnum;
}

int is_thread_alive(pthread_t tid)
{
    int kill_rc = pthread_kill(tid, 0);     //发0号信号，测试线程是否存活
    if (kill_rc == ESRCH) 
	{
        return false;
    }

    return true;
}

/*测试*/ 

#if 0
/* 线程池中的线程，模拟处理业务 */
void *process(void *arg)
{
    printf("thread 0x%x working on task %d\n ",(unsigned int)pthread_self(),*(int *)arg);
    sleep(1);
    printf("task %d is end\n", *(int *)arg);

    return NULL;
}


int main(void)
{
    /*threadpool_t *threadpool_create(int min_thr_num, int max_thr_num, int queue_max_size);*/
    threadpool_t *thp = threadpool_create(3,100,100);   /*创建线程池，池里最小3个线程，最大100，队列最大100*/
    printf("pool inited");

    //int *num = (int *)malloc(sizeof(int)*20);
    int num[20], i;
    for (i = 0; i < 20; i++) 
	{
        num[i]=i;
        printf("add task %d\n",i);
        threadpool_add(thp, process, (void*)&num[i]);   /* 向线程池中添加任务 */
    }

    sleep(10);                                          /* 等子线程完成任务 */
    threadpool_destroy(thp);

    return 0;
}

#endif
