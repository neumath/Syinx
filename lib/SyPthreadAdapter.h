#pragma once
#include "SyPthreadPool.h"

#define SOCKETS	                                         int
class SyinxAdapter;
class SyinxPthreadPool;

struct threadpool_t;
struct Principal_Pth;

//线程适配器
class SyinxAdapterPth :public SyinxAdapter
{
	friend class SyinxKernel;
	friend class SyinxAdapterResource;
public:
	SyinxAdapterPth();
	SyinxAdapterPth(int PthPoolNum, int TaskMaxNum) : PthPoolNum(PthPoolNum), TaskMaxNum(TaskMaxNum) {}
	~SyinxAdapterPth();


public://初始化次要线程池
	int SyinxAdapterPth_Init();

	//向线程池里添加一个任务,任务会被加载到任务队列
	static int SyinxAdapter_Pth_Add(void* (*taskfunc)(void*), void* arg);

	//销毁线程池
	int SyinxAdapter_Pth_destroy();

	//并发线程池数量
	int PthPoolNum;
	//设置任务队列数量
	int TaskMaxNum;
private:

	//绑定资源适配器
	SyinxAdapterResource* mPthRes;

	//绑定框架地址
	SyinxKernel* mSyinx;


	//保存次要线程池属性结构体
	threadpool_t* mSyinxPthPool;

};
