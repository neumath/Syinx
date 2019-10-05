#pragma once
#include "SyAdapter.h"

//任务处理类
class SyinxAdapterMission : public SyinxAdapter
{
public:
	SyinxAdapterMission() {}
	virtual ~SyinxAdapterMission() {}
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

class IChannel : public SyinxAdapterMission
{
public:
	IChannel() {}
	~IChannel() {}
public:
	virtual void RecvData() override;
	virtual void SendData() override;
};