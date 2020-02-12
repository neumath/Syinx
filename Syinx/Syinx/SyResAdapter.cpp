#include "SyInc.h"
#include "Syinx.h"
#include "SyTaskAdapter.h"
#include "../module/SyPthreadAdapter.h"
#include "SyResAdapter.h"
#include "../Sylog/SyLog.h"
#include "../Sylog/easylogging++.h"

SyinxAdapterResource::SyinxAdapterResource(int IChNum) :m_IChannelNum(IChNum)
{
	m_listConnectClient.clear();
}

SyinxAdapterResource::~SyinxAdapterResource()
{
	m_listConnectClient.clear();
}

bool SyinxAdapterResource::Initialize()
{
	
	for (size_t Index  = 0;Index < m_IChannelNum;++Index)
	{
		IChannel* tagTemp = new IChannel;
		if (!tagTemp->Initialize())
		{
			LOG(ERROR) << "IChannel Initialize Failed";
			return false;
		}
		if (tagTemp != nullptr)
		{
			m_queueILoop.push(tagTemp);
		}
		else
		{
			return false;
		}
	}
	if(m_queueILoop.size() != m_IChannelNum)
	{
		LOG(ERROR) << "Create IChannel failed";
		return false;
	}

	LOG(INFO) << "Create IChannel: " << m_queueILoop.size() << " Success";
	return true;
	
}

//将新连接的客户端进行分配
bool SyinxAdapterResource::AllocationIChannel(bufferevent* buffer, SOCKETS _FD)
{
	if (m_queueILoop.empty())
	{
		LOG(ERROR) << "Loop is Empty";
		bufferevent_free(buffer);
		return false;
	}
	IChannel* ICh = m_queueILoop.front();
	if (ICh == nullptr)
	{
		return false;
	}
	m_queueILoop.pop();
	ICh->OnClientConnect(_FD, buffer);
	
	return SocketFdAdd(ICh);
}

bool SyinxAdapterResource::SocketFdAdd(IChannel* ICh)
{
	m_listConnectClient.push_back(ICh);
	LOG(INFO) << ICh->GetSocket() << " is Connect " << endl;
	LOG(INFO) << "Current number of client connections : [ " << m_listConnectClient.size()<<" ] ";

	auto _buffer = ICh->GetBuffer();
	if (nullptr != _buffer)
	{
		bufferevent_setcb(_buffer, SyinxKernel_Recv_Cb, SyinxKernel_Send_Cb, SyinxKernel_Event_Cb, (void*)ICh);
		bufferevent_setwatermark(_buffer, EV_READ, LIBEVENT_READ_WATERMARK,0);
		bufferevent_enable(_buffer, SET_SOCKETS_EVENT_RDWR);
	}
	return true;
}

bool SyinxAdapterResource::SocketFdDel(IChannel* ICh)
{
	LOG(INFO) << ICh->GetSocket() << " DisConnect ";
	m_listConnectClient.remove(ICh);
	ICh->Clear();
	m_queueILoop.push(ICh);
	LOG(INFO) << "Current number of client connections : [ " << m_listConnectClient.size() << " ] ";

	return true;
}

void SyinxAdapterResource::GameServerDoAction()
{
	for (auto Iter : m_listConnectClient)
	{
		Iter->OnStatusDoAction();
	}
}

bool SyinxAdapterResource::Close()
{
	for (auto& Iter : m_listConnectClient)
	{
		Iter->Clear();
		m_queueILoop.push(Iter);
	}
	size_t _size = m_queueILoop.size();
	for (int Index = 0;Index < _size;++Index)
	{
		auto _Ich = m_queueILoop.front();
		m_queueILoop.pop();
		if (_Ich != nullptr)
		{
			_Ich->Clear();
			delete _Ich;
			_Ich = nullptr;
		}
		
	}
	return true;
}

