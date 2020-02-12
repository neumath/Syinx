#ifndef _SYRESADAPTER_H_
#define _SYRESADAPTER_H_
#include <unordered_map>
#include <queue>

class SyinxKernel;
class SyinxAdapter;
class SyinxAdapterPth;
class SyinxLog;
class IChannel;
class CPlayer;

#define SOCKETS	                                         int
#define BUFFSIZE										 256

class SyinxAdapterResource
{
	friend void SyinxKernel_Event_Cb(struct bufferevent* bev, short what, void* ctx);
private:
	//将用户的通道层挂在树上
	std::list<IChannel*>				 m_listConnectClient;
	std::queue<IChannel*>				 m_queueILoop;
	size_t								m_IChannelNum;
public:
	SyinxAdapterResource(int IChNum);
	~SyinxAdapterResource();
	bool Initialize();
	bool Close();

	//将新连接的客户端进行分配
	bool AllocationIChannel(bufferevent* buffer, SOCKETS _FD);	//分配一个IChannel

	bool SocketFdAdd(IChannel* ICh);							//将新的文件描述符做上树操作
	bool SocketFdDel(IChannel* ICh);							//将文件描述符下树

	void GameServerDoAction();								//处理所有帧

};
#endif