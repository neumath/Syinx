#ifndef _SYINXTIMER_H_
#define _SYINXTIMER_H_

#include <list>
#define MSEC(NUM)    ((NUM)*1000)
#define NODESIZE      4
#define QUEUESIZE     10

//一直会存在该节点重复运行
#define TIME_EVENT_ALWAYSRUN       (0x2)
//只会运行一次后退出
#define TIME_EVENT_ONCE            (0x1)

//等待时间
static inline void TIMEWAIT(__uint32_t sec, __uint32_t msec);
struct Syinxtimer_task_t
{
	void* (*function)(void*);
	void* arg;

	//设置一个第一次触发时间,当到达这个时间是遍触发回调以当前时间轮的指定时间为单位
	uint32_t lasttime;	

	//是否设置一个占位
	bool occupied;
};

struct Syinxtimeout_t
{

	__uint32_t sec;

	__uint32_t msec;

	pthread_t timertid;

	std::list<Syinxtimer_task_t*> TimerTasklist;

	bool shotdown;

	__uint64_t  Cycles;                            /*走过的圈数*/
};
namespace SyTimer {
	enum SyTimerErr
	{
		Success = 1,  /*成功*/

		ArgIsNULL,/*参数为空*/

		NOMARK,       /*未指定标志位即flags*/
	};
	class Syinxtimer
	{
	public:
		Syinxtimer() {};
		~Syinxtimer() {};

		/*
		初始化一个定时器设置触发事件为sec+msec,定时器不为单例模式,你可以初始化多个定时器并在公用函数中指定定时器句柄
		@sec:  秒
		@msec:  毫秒
		失败返回空,成功返回Syinxtimeout_t的结构体
		*/
		Syinxtimeout_t* Syinxtimer_create(__uint32_t sec, __uint32_t msec);

		/*
		将一个函数添加到时间轮子上并且指定其调用时机
		Sytimer:  传入Syinxtimeout_t的结构体
		callback:  指定的回调函数指针
		arg:       回调函数参数
		timeout:   指定第一次触发延迟时间,为你指定的(sec+msec)为一周期
		flags:     指定函数调用时机

					TIME_EVENT_ALWAYSRUN:一直会存在该节点重复运行

					TIME_EVENT_ONCE:只会运行一次后退出
		失败范围errno
		链表尾插,先进先出,先插入的会被先调用
		*/
		static int Syinxtimer_add(Syinxtimeout_t* Sytimer, void* (*callback)(void*), void* arg, uint32_t timeout, int flags);

		/*
		销毁定时器
		成功返回success
		*/
		static int Syinxtimer_destory(Syinxtimeout_t* Sytimer);
	};
}
#endif /*_SYINXTIMER_H_*/