
#ifndef _SYRESADAPTER_H_
#define _SYRESADAPTER_H_
class SyinxAdapter;
class SyinxAdapterPth;
class SyinxKernel;
#include <vector>
#define SOCKETS	                                         int
#define BUFFSIZE                                         256
class mResourceMsg 
{
	int PthNum;                /*保存当前的最大任务队列树(线程数)*/
};
//资源适配器
class SyinxAdapterResource : public SyinxAdapter
{
	
	friend void SyinxKernel_Listen_CB(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* arg);
	friend class SyinxKernel;
	friend class SyinxAdapterPth;
	friend class SyinxAdapterMission;
	friend class Ichannel;
public:
	SyinxAdapterResource();
	SyinxAdapterResource(int PthNum) :PthNum(PthNum) {}
	~SyinxAdapterResource();

	//获取集群中所有活跃玩家数量
	int getallClientNum();

	//将集群中某一个玩家清除
	int deleteClient(SOCKETS _FD, int where);

	//将客户端设置定为随机分配,但是不允许超过最大允许的范围
	int SetClientRandAllot(bool flags);

	//设置客户端固定分配
	int SetClientFixationAllot(int Num);

private:
	//初始化集群资源
	int SyinxAdapterResource_Init(int PthNum);

	//将新连接的客户端进行分配
	int SyinxAdapterResource_AllotClient(SOCKETS _FD);

	//将新的文件描述符做上树操作
	int SocketFd_Add(SOCKETS _FD, int where, int events = 0);

	//将文件描述符下树
	int SocketFd_Del(SOCKETS _FD, int where);

	//为base添定时器
	int SyinxAdapterResource_Addtimefd();

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
	//客户端随机分配默认为true
	bool RandAllotFlags = false;

	SyinxAdapterResource(SyinxAdapterResource& _intmp) = delete;

	//一个线程对应一个事件树,每个事件树的用户放在multimap上
	std::vector<std::multimap<SOCKETS, IChannel*>*> mSyChannelVec;
	std::vector<event_base*> mSyBaseVec;
	
};
#endif

