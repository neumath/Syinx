#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "Syinxtimer.h"
//struct Syinxtimeout_t
//{
//	uint32_t sec;
//
//	uint32_t msec;
//
//	std::list<std::map<int, Syinxtimer_task_t*>*>* TimerTasklist;
//};
void TIMEWAIT(__uint32_t sec, __uint32_t msec)
{
	if (!sec)
	{
		usleep(MSEC(msec));
		return;
	}
	sleep(sec);
	usleep(MSEC(msec));
	return;

}
//定时器回调函数
void* Timerpthread(void* arg)
{
	Syinxtimeout_t* Sytimer_t = (Syinxtimeout_t*)arg;
	Syinxtimer_task_t task_t;

	while (Sytimer_t->shotdown == false)
	{
		if (!Sytimer_t->TimerTasklist.size())
			printf("timeout!\n");
		else if (Sytimer_t->shotdown)
			break;
		for (auto _it = Sytimer_t->TimerTasklist.begin(); _it != Sytimer_t->TimerTasklist.end();)
		{
			
			task_t.function = (*_it)->function;
			task_t.arg = (*_it)->arg;
			//如果达到了设置的第一次运行时间则运行,在判断是否为占位
			if (!(*_it)->lasttime)
			{
				
				if ((*_it)->occupied)/*是占位则每一次定时都会触发,并永远存在队列中(实际只是不下该链表)*/
				{
					((*task_t.function))(task_t.arg);
					++_it;
				}
				else
				{
					auto tmp = *_it;
					((*task_t.function))(task_t.arg);
					Sytimer_t->TimerTasklist.erase(_it++);
					delete tmp;
				}
			}
			else
			{
				(*_it)->lasttime -= (Sytimer_t->sec + Sytimer_t->msec);
				++_it;
			}
		}
		TIMEWAIT(Sytimer_t->sec, Sytimer_t->msec);
		/*圈数++*/
		++Sytimer_t->Cycles;
	}
	pthread_exit(0);
	return NULL;
}

Syinxtimeout_t* SyTimer::Syinxtimer::Syinxtimer_create(__uint32_t sec, __uint32_t msec)
{
	if (sec <= 0 && msec <= 0)
	{
		return NULL;
	}
	Syinxtimeout_t* Sytimer_t = new Syinxtimeout_t;
	Sytimer_t->sec = sec;
	Sytimer_t->msec = msec;
	Sytimer_t->shotdown = false;


	pthread_attr_t pthattr;
	if (pthread_attr_init(&pthattr) != 0)
	{
		return NULL;
	}

	pthread_attr_setdetachstate(&pthattr, PTHREAD_CREATE_DETACHED);

	if (pthread_create(&Sytimer_t->timertid, &pthattr, Timerpthread, (void*)Sytimer_t) != 0)
	{
		return NULL;
	}

	return Sytimer_t;
}

int SyTimer::Syinxtimer::Syinxtimer_add(Syinxtimeout_t* Sytimer_t, void* (*callback)(void*), void* arg, uint32_t timeout, int flags)
{
	if (Sytimer_t == NULL || callback == NULL)
	{
		return SyTimer::ArgIsNULL;
	}

	if (flags == TIME_EVENT_ONCE)/*只运行一次*/
	{
		Sytimer_t->TimerTasklist.push_back(new Syinxtimer_task_t{ callback, arg, timeout,false });
	}
	else if (flags == TIME_EVENT_ALWAYSRUN)/*总是在运行着*/
	{
		Sytimer_t->TimerTasklist.push_back(new Syinxtimer_task_t{ callback, arg, timeout,true });
	}
	else
	{
		return SyTimer::NOMARK;
	}

}

int SyTimer::Syinxtimer::Syinxtimer_destory(Syinxtimeout_t* Sytimer)
{
	if (Sytimer == NULL)
	{
		return SyTimer::ArgIsNULL;
	}
	//销毁线程
	Sytimer->shotdown = true;
	for (auto _it = Sytimer->TimerTasklist.begin(); _it != Sytimer->TimerTasklist.end();)
	{
		auto _tmp = *_it;
		Sytimer->TimerTasklist.erase(_it++);
		delete _tmp;
	}
	delete Sytimer;
	return SyTimer::Success;
}
