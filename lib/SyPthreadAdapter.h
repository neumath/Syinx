#pragma once
#include "SyPthreadPool.h"

#define SOCKETS	                                         int
class SyinxAdapter;
class SyinxPthreadPool;

struct threadpool_t;
struct Principal_Pth;

//主线程工作回调函数
void* PrincipalPth_Cb(void* dst);

//线程适配器
class SyinxAdapterPth :public SyinxAdapter
{
	friend class SyinxKernel;
	friend class SyinxAdapterResource;
public:
	SyinxAdapterPth();
	SyinxAdapterPth(int PthNum) : PthNum(PthNum) {}
	~SyinxAdapterPth();
	//初始化主工作线程池
	int SyinxAdapterPriPth_Init();


	//初始化回调函数参数结构体
	int SyinxAdapterPriPth_InitMsg();

	/*
	 @function SyinxAdapterPriPth_Setcb
	 设置当前的主线程回调函数参数
	 @param flags 设置回调函数分配
	 @返回一个新创建的线程池或NULL
	
	 */
	int SyinxAdapterPriPth_Setcb(std::vector<event_base*>* BaseVec,int flags, int locaiont);

	//create主线程使其工作
	int SyinxAdapterPriPth_Run();

private:
	
public://初始化次要线程池
	int SyinxAdapterPth_Init();

   //向线程池添加一个通道层
	int SyinxAdapter_Pth_Add();

	//销毁线程池
	int SyinxAdapter_Pth_destroy();

	//获取当前任务线程数量
	int getPthNum() const;
public:

	//保存服务器的线程数量
	int PthNum;//测试为5

private:
	//回调函数参数设置
	void* arg;

	//绑定资源适配器
	SyinxAdapterResource* mPthRes;

	//绑定框架地址
	SyinxKernel* mSyinx;

	//保存主要线程池结构体
	Principal_Pth* mSyinxPriPthPool;

	//保存次要线程池属性结构体
	threadpool_t* mSyinxPthPool;

};