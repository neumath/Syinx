#include "pthreadpool.h"
#include "Log.h"

#include <pthread.h>
#include <iostream>
#include <vector>
#include <map>
#include <event2/listener.h>  
#include <event2/bufferevent.h>  
/*
class SyZinxAdapter -> class SyZinxAdapterPth              线程适配器
class SyZinxAdapter -> class SyZinxAdapterMission		   任务适配器
class SyZinxAdapterMission -> class IChannel            通道层用于接收客户端发来的数据
class IChannelFactory /                                 用于初始化化一系列数据

*/

#ifndef _SYPTH_H_
#define _SYPTH_H_

class SyZinxKernel;
class SyZinxAdapter
{
public:
	SyZinxAdapter() {}
	virtual ~SyZinxAdapter() {}
	SyZinxLog mLog;
};

class SyZinxAdapterResource;
//线程适配器
class SyZinxAdapterPth :public SyZinxAdapter
{
	friend class SyZinxKernel;
public:
	SyZinxAdapterPth();
	SyZinxAdapterPth(int PthNum) : PthNum(PthNum) {}
	~SyZinxAdapterPth();

	//初始化线程池
	 int SyZinxAdapterPth_Init();

	//向线程池添加一个通道层
	int SyZinxAdapter_Pth_Add();

	//销毁线程池
	static int SyZinxAdapter_Pth_destroy();

	//获取当前任务线程数量
	int getPthNum() const;
public:

	//保存服务器的线程数量
	int PthNum;//测试为5
	
private:
	//绑定资源适配器
	SyZinxAdapterResource* mPthRes;

	//绑定框架地址
	SyZinxKernel* mSyZinx;

	//保存线程池属性结构体
	threadpool_t* mSyZinxPthPool;
	
};

class SyZinxAdapterMission;
class IChannel;

struct ClientMsg {
	IChannel* pChannel;
	bufferevent* pClient_buffer;
};

//资源适配器
class SyZinxAdapterResource : public SyZinxAdapter
{
	friend class SyZinxKernel;
	friend class SyZinxAdapterPth;
	friend class SyZinxAdapterMission;
public:
	SyZinxAdapterResource();
	~SyZinxAdapterResource();

	//获取集群中所有活跃玩家数量
	int getallClientNum();

	//将集群中某一个玩家清除
	int deleteClient();
private:
	//初始化集群资源
	int SyZinxAdapterResource_Init(int PthNum);

private:
	SyZinxAdapterPth* mResPth;
	SyZinxAdapterResource(SyZinxAdapterResource& _intmp) = delete;

	//一个线程对应一个事件树,每个事件树的用户放在multimap上
	std::vector<std::multimap<int, ClientMsg*>> mSyChannelVec;
	std::vector<event_base*> mSyBaseVec;
};

//任务处理类
class SyZinxAdapterMission : public SyZinxAdapter
{
public:
	SyZinxAdapterMission() {}
	virtual ~SyZinxAdapterMission() {}
public:
	//留好接收数据的坑
	virtual void RecvData() = 0;
	virtual void SendData() = 0;

};

//创建通道层工厂类用于一系列初始化
class IChannelFactory
{
public:
	IChannelFactory() {}
	~IChannelFactory() {}
};

class IChannel : public SyZinxAdapterMission
{
public:
	IChannel() {}
	~IChannel() {}
public:
	virtual void RecvData() override;
	virtual void SendData() override;
};

#endif 


