#include "Syinx.h"
#include "SyAdapter.h"
#include "SyResAdapter.h"
#include "SyPthreadAdapter.h"
#include "SyTaskAdapter.h"
#include "SyConfig.h"
#include <vector>
#include <memory>
#include <random>
#include <errno.h>
#include <iterator>
//定义一个随机引擎(均匀分布)
static std::default_random_engine gRandomEngine(time(nullptr));

SyinxAdapterResource::SyinxAdapterResource()
{

}

SyinxAdapterResource::~SyinxAdapterResource()
{
}



//将新连接的客户端进行分配
int SyinxAdapterResource::SyinxAdapterResource_AllotClient(bufferevent* buffer, SOCKETS _FD)
{
	return this->SocketFd_Add(buffer, _FD);
}

int SyinxAdapterResource::SocketFd_Add(bufferevent* buffer, SOCKETS _FD)
{
	if (buffer == NULL || _FD < 0)
	{
		return -1;
	}
	

	IChannelMsg* icmsg = new IChannelMsg;;  //need free
	icmsg->buffer = buffer;
	icmsg->Socket = _FD;


	//初始化对象独享的通道层
	IChannel* newClieICh = new IChannel;
	newClieICh->ICannel_Init(icmsg);

	//绑定资源管理器的地址
	newClieICh->mICnSaveRes = this;
	//上map容器
	this->mIChannelMap.insert(std::make_pair(buffer, newClieICh));

	//更新共享内存
	this->SyinxAdapterResource_UpdateShm();


	//设置buffer回调以及事件回调
	bufferevent_setcb(buffer, SyinxKernel_Recv_Cb, SyinxKernel_Send_Cb, SyinxKernel_Event_Cb, (void*)newClieICh);

	//设置buffer事件
	bufferevent_enable(buffer, SET_SOCKETS_EVENT_RD);
	

	return 0;
}

int SyinxAdapterResource::SocketFd_Del(bufferevent* buffer, SOCKETS _FD)
{
	auto _it = this->mIChannelMap.find(buffer);
	_it->second->IChannel_free();
	bufferevent_free(_it->first);
	this->mIChannelMap.erase(_it);

	//更新共享内存
	this->SyinxAdapterResource_UpdateShm();
	char WriteLog[BUFFSIZE] = { 0 };
	sprintf(WriteLog, "Client is Exit, Current number of connections[%ld]", this->mIChannelMap.size());
	SyinxLog::mLog.Log(__FILE__, __LINE__, SyinxLog::EVENT, 0, WriteLog);
}

int SyinxAdapterResource::SyinxAdapterResource_Free()
{
	for (auto _it : this->mIChannelMap)
	{
		this->SocketFd_Del(_it.first, 0);
	}
}

int SyinxAdapterResource::SyinxAdapterResource_UpdateShm()
{
	auto ShmData = this->mSyinx->ShmData;
	auto mShmData = (SyinxKernelShmMsg*)ShmData;

	mShmData->AllClientNum = this->mIChannelMap.size();

}
