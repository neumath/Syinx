#ifndef _SYRESADAPTER_H_
#define _SYRESADAPTER_H_
class SyinxAdapter;
class SyinxAdapterPth;
class SyinxKernel;
#include <vector>
#define SOCKETS	                                         int
#define BUFFSIZE                                         256
struct SyinxConfMsg;
class SyinxConfig;
//资源适配器
class SyinxAdapterResource : public SyinxAdapter
{

	friend void SyinxKernel_Listen_CB(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* arg);
	friend class SyinxKernel;
	friend class SyinxAdapterPth;
	friend class SyinxAdapterMission;
	friend class IChannel;
	friend void SyinxKernel_Event_Cb(struct bufferevent* bev, short what, void* ctx);
public:
	SyinxAdapterResource();
	SyinxAdapterResource(int PthNum) :PthNum(PthNum) {}
	~SyinxAdapterResource();

private:

	//将新连接的客户端进行分配
	int SyinxAdapterResource_AllotClient(bufferevent* buffer, SOCKETS _FD);

	//将新的文件描述符做上树操作
	int SocketFd_Add(bufferevent* buffer, SOCKETS _FD);

	//将文件描述符下树
	int SocketFd_Del(bufferevent* buffer, SOCKETS _FD);

	//释放资源组件
	int SyinxAdapterResource_Free();

	//更新共享内存
	int SyinxAdapterResource_UpdateShm();
private:/*绑定(mRes+Task 资源管理器绑定任务管理器)*/
	//绑定任务类
	SyinxAdapterMission* mResTask;
	//绑定线程类
	SyinxAdapterPth* mResPth;
	//绑定核心框架类
	SyinxKernel* mSyinx;
private:

	//线程数(IO树)
	int PthNum;

	SyinxAdapterResource(SyinxAdapterResource& _intmp) = delete;

	//将用户的通道层挂在树上
	std::multimap<bufferevent*, IChannel*> mIChannelMap;

};
#endif
