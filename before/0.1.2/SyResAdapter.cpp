#include "Syinx.h"
#include "SyAdapter.h"
#include "SyResAdapter.h"
#include "SyPthreadAdapter.h"

#include<memory>
#include <random>
//定义一个随机引擎(均匀分布)
static std::default_random_engine gRandomEngine(time(nullptr));
SyinxAdapterResource::SyinxAdapterResource()
{
	
}

SyinxAdapterResource::~SyinxAdapterResource()
{
}


int SyinxAdapterResource::SyinxAdapterResource_Init(int _PthNum)
{
	
	if (_PthNum == 0)
	{
		mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, -1, "getPthNum is 0 ");
	}
	this->mSyChannelVec.reserve(_PthNum);
	this->mSyBaseVec.reserve(_PthNum);
	
	//初始化资源
	for (size_t i = 0; i < _PthNum; i++)
	{
		this->mSyBaseVec[i] = event_base_new();
		
	}
	this->PthNum = _PthNum;
	return 0;
}
//将新连接的客户端进行分配
int SyinxAdapterResource::SyinxAdapterResource_AllotClient(SOCKETS _FD)
{
	int Client_fd = _FD;
	if (Client_fd <= 0)
	{
		mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, -1, "SOCKETS is 0 ");
		return -1;
	}
	//随机分配
	if (this->RandAllotFlags == true)
	{
		
	}
	else {

	}
	return 0;
}

int SyinxAdapterResource::SocketFd_Add(SOCKETS _FD, int where, int events)
{
	if (_FD <= 0 | where > PthNum | where < 0)
	{
		mLog.Log(__FILE__, __LINE__, SyinxLog::ERROR, -1, "SOCKETS is 0 ");
		return -1;
	}
	struct bufferevent* buffer = bufferevent_socket_new(this->mSyBaseVec[where], _FD, BEV_OPT_CLOSE_ON_FREE);

	//设置buffer回调函数

	//设置buffer事件
	/*挖坑------------------------------------------*/
	bufferevent_setcb(buffer, NULL, NULL, NULL, NULL);
	//设置buffer事件
	bufferevent_enable(buffer, SET_SOCKETS_EVENT_RD);
	//上map容器
	this->mSyChannelVec[where].insert(std::make_pair(_FD, buffer));

	return 0;
}

int SyinxAdapterResource::SocketFd_Del(SOCKETS _FD, int where)
{
	return 0;
}

int SyinxAdapterResource::getallClientNum()
{

}

int SyinxAdapterResource::deleteClient()
{
	return 0;
}

