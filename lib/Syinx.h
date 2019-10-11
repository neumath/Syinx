#ifndef _Syinx_H_
#define _Syinx_H_
#include "SyInc.h"
#include "SyLog.h"

//设置线程安全以及关闭底层套接字  详细请看libevent文档
#define SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING           (LEV_OPT_LEAVE_SOCKETS_BLOCKING | LEV_OPT_THREADSAFE)
//设置线程安全以及关闭底层套接字以及设置close标志位  详细请看libevent文档
#define SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING_OR_FREE   (LEV_OPT_LEAVE_SOCKETS_BLOCKING | LEV_OPT_THREADSAFE |  LEV_OPT_CLOSE_ON_FREE)
//设置读写循环
#define SET_SOCKETS_EVENT_RDWR                          (EV_READ | EV_WRITE | EV_PERSIST)
//设置读循环
#define SET_SOCKETS_EVENT_RD                            (EV_READ | EV_PERSIST)
//命令参数
#define COMMMEND_PARAM                                   argv[1]
#define SET_SHM_KEY                                      0x8855

#define SYINXMOD_ADD_MYSQL                               
#define SYINXMOD_ADD_ORACLE    

//框架全部信息用于进程通信
struct SyinxKernelShmMsg
{
	bool SyinxKernewWork;     /*保存当前服务器状态*/

	int PthNum;               /*保存当前开启的主任务数量*/

	int AllClientNum;         /*所有客户端连接数量*/

	int CurrentClientNum[4];

	pthread_t threads[4];        /*保存每一个tid*/
	int       mPthStatus[4];     /*保存每一个线程的工作状态*/

	char IP[16];                 /*保存ip*/

	char Port[8];                /*保存端口号*/

	bool ExitSignal;             /*syinx退出信号*/

}; 
enum  PthStatus
{
	PthRun = 0,
	PthWait,
	PthExit,
};
class SyinxLog;
class IChannel;
class SyinxAdapterMission;
class SyinxAdapterPth;
class SyinxAdapterResource;
class SyinxPthreadPool;

class SyinxKernel;
struct SyinxKernelShmMsg;

class SyinxKernelObject
{
public:
	SyinxKernelObject() {}
	~SyinxKernelObject() {}
private:

};
//用于初始化的核心框架
class SyinxKernel 
{
	friend class SyinxKernelWork;
	friend class SyinxAdapterPth;
	friend class SyinxRecycle;
	friend class SyinxAdapterResource;
	friend void SyinxKernel_Listen_CB(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* arg);
public:
	
	//初始化框架
	static int SyinxKernel_Init(const short _inPort);


	//运行框架(wait)
	static void SyinxKernel_Run();

	//关闭框架
	static void SyinxKernel_Close();

	//客户端退出时将会释放客户端任务链,客户端文件描述符为key值       已废弃
	static void SyinxKernel_Client_Close(const int mClient_fd);

	//通过共享内存共享数据情报
	//创建共享内存
	static void SyinxSyinxKernel_MakeShm();
	
	//释放共享内存
	static void SyinxSyinxKernel_FreeShm();

	//创建初始化本地套接字
	static void SyinxSyinxKernel_LocalSock();

	SyinxLog mLog;
private:
	//框架初始化适配器
	int SyinxKernel_InitAdapter();

	//过程初始化
	int SyinxKernel_Procedureinit();

private:
	//用于保存一个主要的base句柄
	struct event_base* SyinxBase;

	//保存用于监听套接字的evconnlistener
	struct evconnlistener* SyinxListen;

	//客户端port
	short _Port;

	//保存一个共享内存的shmid
	int ShmId;

	//保存共享内存的万能指针
	void* ShmData;
	SyinxKernelShmMsg* mShmData;
	
	//保存本套接字
	int LocalSocket;
public:
	//绑定一份一个SyinxAdapterPt线程适配器
	SyinxAdapterPth* mSyPth;
	//绑定一个任务适配器
	SyinxAdapterMission* mSyMission;
	//绑定一个资源适配器
	SyinxAdapterResource* mSyResource;

public:
	int PthNum = 1;
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
public:

	//数据库核心模块引用
	static int SyDatabaseMod;
private:
	//引用条件编译
	int SyinxAddDatabase();

};
//托管核心初始化开启类,以及判断输入参数
class SyinxKernelWork
{
public:
	SyinxKernelWork(){}
	SyinxKernelWork(int Prot, int argc, char* argv[]);
	~SyinxKernelWork();
public:
	
	//打印服务器状态  -s
	void PrintfServerStatus();

	
	void Makedaemon(int Prot);

	void SyinxExit();

	//保存框架的地址
	SyinxKernel* mSyinx;

	int Port;

	//保存进程id
	int Pid_t;

	bool SyinxKernelStatus = false;
	
	int SyinxKernelExit;
};

#endif 


//json
/*
{

	设置用户:char
	服务器端口:int

	服务器集群数量:int
	#数量必须与Group的server的数量一致
	Group:(char)服务器集群名字{
			server#1:(int)id
			server#2:
	}
}


*/