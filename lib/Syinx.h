
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

#define SYINXMOD_ADD_MYSQL                               
#define SYINXMOD_ADD_ORACLE                              
class SyinxLog;
class IChannel;
class SyinxAdapterMission;
class SyinxAdapterPth;
class SyinxAdapterResource;

struct SyinxKernelMsg
{
	int mClient_fd;
	struct buffereveant* Client_buffer;
};
class SyinxKernel;
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
	friend class SyinxAdapterPth;
	friend class SyinxRecycle;
	friend class SyinxAdapterResource;
public:
	
	//初始化框架
	static int SyinxKernel_Init(const short _inPort);


	//运行框架(wait)
	static void SyinxKernel_Run();

	//关闭框架
	static void SyinxKernel_Close();

	//客户端退出时将会释放客户端任务链,客户端文件描述符为key值       已废弃
	static void SyinxKernel_Client_Close(const int mClient_fd);

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
//托管核心初始化开启类
class SyinxKernelWork
{
public:
	SyinxKernelWork(){}
	SyinxKernelWork(int Prot = 8080);
	~SyinxKernelWork(){}

	int SyinxClose();
};
//回收
class SyinxRecycle
{
public:
	SyinxRecycle();
	~SyinxRecycle();
	SyinxLog mLog;
	/*
	1.static std::multimap<int, SyinxKernelMsg*> mZinxMap; 中的SyinxKernelMsg需要释放
	2.
	*/

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