#ifndef _Syinx_H_
#define _Syinx_H_
#include "SyInc.h"


//设置线程安全以及关闭底层套接字  
#define SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING           (LEV_OPT_LEAVE_SOCKETS_BLOCKING | LEV_OPT_THREADSAFE)
//设置线程安全以及关闭底层套接字以及设置close标志位  
#define SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING_OR_FREE   (LEV_OPT_LEAVE_SOCKETS_BLOCKING | LEV_OPT_THREADSAFE |  LEV_OPT_CLOSE_ON_FREE)
//设置读写循环
#define SET_SOCKETS_EVENT_RDWR                          (EV_READ | EV_WRITE | EV_PERSIST)
//设置读循环
#define SET_SOCKETS_EVENT_RD                            (EV_READ | EV_PERSIST)

//命令参数
#define COMMMEND_PARAM                                   argv[1]
#define SET_SHM_KEY                                      0x8855

                             
enum  PthStatus
{
	PthRun = 0,
	PthWait,
	PthExit,
};
/*
	声明需要写入log的事件发生级别的字符串
	分为
	EVENT    事件发生   1
	WARNING  警告       2
	ERROR    错误       3
*/


enum SyinxKernelErrno
{
	PortErr = 1,
	CreateBaseErr ,
	ClientConErr ,

	//Success
	SyinxSuccess,
};
class SyinxKernel;

//框架全部信息用于进程通信
struct SyinxKernelShmMsg
{
	bool SyinxKernewWork;     /*保存当前服务器状态*/

	int PthNum;               /*保存当前开启的主任务数量*/

	int AllClientNum;         /*所有客户端连接数量*/

	char IP[16];                 /*保存ip*/

	char Port[8];                /*保存端口号*/

	bool ExitSignal;             /*syinx退出信号*/

	SyinxKernel* mSyinx;         /*保存框架的地址*/

};
struct SyinxKernelTimer_task_t
{
	void* (*taskfunc)(void*);
	void* arg;
};
//框架其他类声明
class SyinxLog;
class IChannel;
class SyinxAdapterMission;
class SyinxAdapterPth;
class SyinxAdapterResource;
class SyinxPthreadPool;
class SyinxConfig;
class SyinxKernelWork;


struct SyinxKernelShmMsg;
struct SyinxConfMsg;


//声明回调
//时间处理回调
void SyinxKernel_TimerEvent_Cb(struct bufferevent* buffer, void* arg);
//读出回调
void SyinxKernel_Recv_Cb(struct bufferevent* bev, void* ctx);
//写事件回调
void SyinxKernel_Send_Cb(struct bufferevent* bev, void* ctx);
//事件回调
void SyinxKernel_Event_Cb(struct bufferevent* bev, short what, void* ctx);

//声明一个任务函数
void* IChannelTaskProcessing(void* arg);
//用于初始化的核心框架
class SyinxKernel
{
	friend class SyinxKernelWork;
	friend class SyinxAdapterPth;
	friend class SyinxRecycle;
	friend class SyinxAdapterResource;
	friend void SyinxKernel_Listen_CB(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* arg);
	friend void SyinxKernel_TimerEvent_Cb(struct bufferevent* buffer, void* arg);
public:

	//初始化框架
	static int SyinxKernel_Init();


	//运行框架(wait)
	static void SyinxKernel_Run();

	//关闭框架
	static void SyinxKernel_Close();

	bool SyinxKernewWork = false;

	//通过共享内存共享数据情报
	//创建共享内存
	static void SyinxKernel_MakeShm();

	//释放共享内存
	static void SyinxKernel_FreeShm();

	//返回共享内存地址
	SyinxKernelShmMsg* GetSyinxKernelShmMsg()const;

private:
	//为自己添加时间计时器
	void SyinxKernel_Addtimefd();

	//保存基于事件描述符的事件队列

	SyinxKernelTimer_task_t** TimeEvent_Task;
	//框架初始化适配器
	int SyinxKernel_InitAdapter();

private:
	//用于保存一个主要的base句柄只适用于连接
	struct event_base* SyinxBase;

	//保存用于监听套接字的evconnlistener
	struct evconnlistener* SyinxListen;

	//客户端port
	short _Port;

	//保存一个共享内存的shmid
	int ShmId;

	//保存共享内存的万能指针
	void* ShmData;
	SyinxKernelShmMsg* mShmData = nullptr;


	//保存读取配置文件的信息
	SyinxConfMsg* SyConfMsg;         //free
public:
	//绑定一份一个SyinxAdapterPt线程适配器
	SyinxAdapterPth* mSyPth;

	//绑定一个资源适配器
	SyinxAdapterResource* mSyResource;

public:

	//保存服务器的sockaddr_in
	struct Server_Sockaddr {
		std::string Prot;
		std::string sin_addr;
		std::string family;
	}Server_Sockaddr;

public:
	//自己的指针
	static SyinxKernel* mSyinx;


private: //私有构造函数
	SyinxKernel();
	SyinxKernel(SyinxKernel&) = default;
	~SyinxKernel();

};
//托管核心初始化开启类,以及判断输入参数
class SyinxKernelWork
{
public:
	SyinxKernelWork() {}
	SyinxKernelWork(int argc, char* argv[]);
	~SyinxKernelWork();
public:

	int SyinxWork();
	
	//打印服务器状态  -s
	void PrintfServerStatus();


	void Makedaemon();

	void SyinxExit();

	//保存框架的地址
	SyinxKernel* mSyinx;


	//保存进程id
	int Pid_t;

	bool SyinxKernelStatus = false;

	int SyinxKernelExit;
};

#endif 