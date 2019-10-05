#ifndef _SYPTH_H_
#define _SYPTH_H_
#include "Log.h"
#include "SyDef.h"
/*
class SyinxAdapter -> class SyinxAdapterPth              线程适配器
class SyinxAdapter -> class SyinxAdapterMission		   任务适配器
class SyinxAdapterMission -> class IChannel            通道层用于接收客户端发来的数据
class IChannelFactory /                                 用于初始化化一系列数据

*/


class SyinxKernel;
class SyinxAdapter
{
public:
	SyinxAdapter() {}
	virtual ~SyinxAdapter() {}
	SyinxLog mLog;
};
#endif