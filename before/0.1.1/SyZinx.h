#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include<time.h>
#include<event2/event.h>
#include <event2/listener.h>  
#include <event2/bufferevent.h>  
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <list>
#include <map>
#include <jsoncpp/json/json.h>
#include "Log.h"
#include "Adapter.h"
/*
一个线程对应一个事件数
	客户端选择或者被初始化对应不同的事件数进行io





*/
#define USE_JSON_CONFIG
#define CMD_PRINTF_ERR
#define Client_fd	     int

//声明
class IChannel;

#ifndef _SYZINX_H_
#define _SYZINX_H_

#ifdef USE_JSON_CONFIG
class SyZinxConfig {
public:
	SyZinxConfig();
	~SyZinxConfig();

public:
	//配置json文件
	void Make_Msgconfig();
	//读取json文件
	void Reader_Msgconfig();
private:
	//写入json文件
	void Writer_MSgconfig();
public:
	SyZinxLog mLog;
};
#endif 

struct SyZinxKernelMsg
{
	int mClient_fd;
	struct buffereveant* Client_buffer;
};

//用于初始化的核心框架
class SyZinxKernel 
{
	friend class SyZinxAdapterPth;
	friend class SyZinxRecycle;
	friend class SyZinxAdapterResource;
public:
	
	//初始化
	static int SyZinxKernel_Init(const short _inPort);


	//运行框架(wait)
	static void SyZinxKernel_Run();

	//关闭框架
	static void SyZinxKernel_Close();

	//客户端退出时将会释放客户端任务链,客户端文件描述符为key值
	static void SyZinxKernel_Client_Close(const int mClient_fd);



private:
	//用于保存一个base句柄
	struct event_base* SyZinxBase;

	//保存用于监听套接字的evconnlistener
	struct evconnlistener* SyZinxListen;

	//客户端port
	short _Port;
public:
	//绑定一份一个SyZinxAdapterPt线程适配器
	SyZinxAdapterPth* mSyPth;
	//绑定一个任务适配器
	SyZinxAdapterMission* mSyMission;
	//绑定一个资源适配器
	SyZinxAdapterResource* mSyResource;
	
public:
	int PthNum = 5;
	//保存服务器的sockaddr_in
	struct Server_Sockaddr {
		std::string Prot;
		std::string sin_addr;
		std::string family;
	}Server_Sockaddr;

public:
	//自己的指针
	static SyZinxKernel* mSyZinx;


private: //私有构造函数
	SyZinxKernel();
	SyZinxKernel(SyZinxKernel&) = default;
	~SyZinxKernel();
public:
	SyZinxLog mLog;


};

class SyZinxRecycle
{
public:
	SyZinxRecycle();
	~SyZinxRecycle();
	SyZinxLog mLog;
	/*
	1.static std::multimap<int, SyZinxKernelMsg*> mZinxMap; 中的SyZinxKernelMsg需要释放
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