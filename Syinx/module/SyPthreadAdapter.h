#pragma once
#include <stdint.h>
#define SOCKETS	                                         int
class SyinxAdapter;
class SyinxPthreadPool;

struct threadpool_t;
struct Principal_Pth;

enum SyinxAdapterPthErrno
{
	InitFailed,
	AddFuncFailed,
	DesPoolFailed,
};
//线程适配器
class SyinxAdapterPth 
{
	friend class SyinxKernel;
	friend class SyinxAdapterResource;
private:
	int										 m_PthPoolNum;					//并发线程池数量
	int										 m_TaskMaxNum;					//设置任务队列数量
	threadpool_t*							 m_sSyinxPthPool;				//保存次要线程池属性结构体

public:
	SyinxAdapterPth(uint32_t PthPoolNum, uint32_t TaskMaxNum);
	~SyinxAdapterPth();

	bool SyinxAdapterPth_Init();
	static uint32_t SyinxAdapterPth_Add(void* (*taskfunc)(void*), void* arg);
	uint32_t SyinxAdapterPth_destroy();
};
