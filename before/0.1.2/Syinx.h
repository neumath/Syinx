#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include<time.h>
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
#include "SyAdapter.h"
#include "SyDef.h"

/*
一个线程对应一个事件数
	客户端选择或者被初始化对应不同的事件数进行io





*/


//声明
class IChannel;
class SyinxAdapterMission;
class SyinxAdapterPth;
class SyinxAdapterResource;

#ifndef _Syinx_H_
#define _Syinx_H_

#ifdef USE_JSON_CONFIG
class SyinxConfig {
public:
	SyinxConfig();
	~SyinxConfig();

public:
	//配置json文件
	void Make_Msgconfig();
	//读取json文件
	void Reader_Msgconfig();
private:
	//写入json文件
	void Writer_MSgconfig();
public:
	SyinxLog mLog;
};
#endif 

struct SyinxKernelMsg
{
	int mClient_fd;
	struct buffereveant* Client_buffer;
};

//用于初始化的核心框架
class SyinxKernel 
{
	friend class SyinxAdapterPth;
	friend class SyinxRecycle;
	friend class SyinxAdapterResource;
public:
	
	//初始化
	static int SyinxKernel_Init(const short _inPort);


	//运行框架(wait)
	static void SyinxKernel_Run();

	//关闭框架
	static void SyinxKernel_Close();

	//客户端退出时将会释放客户端任务链,客户端文件描述符为key值
	static void SyinxKernel_Client_Close(const int mClient_fd);



private:
	//用于保存一个base句柄
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
	int PthNum = 5;
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
	SyinxLog mLog;


};

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