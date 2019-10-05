

#ifndef _SYZINX_H_
#define _SYZINX_H_


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include<time.h>
#include<event.h>
#include <event2/listener.h>  
#include <event2/bufferevent.h>  
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <list>
#include <map>

#include <mysql/mysql.h>
class Irole;
class Iprotocol;
class Ichannel;
class SyZinxKernel;
class SyZinxDatabase;
/*define*/
#define SERVER_EPOLL_EVENT 1024
#define Client_fd          int

//暂不使用数据库
//#define _USE_DATABASE_

//用于初始化的核心框架
class SyZinxKernel {
	friend class MemRecycled;
	friend void SyZinxKernel_Listen_CB(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* arg);
	friend void SyZinxBuffer_Event_Cb(struct bufferevent* buffer, short events, void* arg);
	friend void SyZinxBuffer_RecvData_Cb(struct bufferevent* buffer, void* arg);
	friend void SyZinxBuffer_SendData_Cb(struct bufferevent* buffer, void* arg);
	friend class SyZinxTcpFactory; 
	friend class Irole;
public:
	//初始化
	static int SyZinxKernel_Init(const short _inPort);


	//运行框架(wait)
	static void SyZinxKernel_Run();

	//关闭框架
	static void SyZinxKernel_Close();

	//获取当前客户端套接字的bufferevent
	static struct bufferevent* SyZinxKernel_GetBuffer(int _iclientfd);

	//客户端退出时将会释放客户端任务链,客户端文件描述符为key值
	static void SyZinxKernel_Client_Close(const int mClient_fd);

	//保存当前服务器用于连接的的一个datebase对象
	static void SyZinxKernel_SaveDatabase(SyZinxDatabase *_iDatabase);

private:
	//用于保存一个base句柄
	static struct event_base* SyZinxBase;

	//保存用于监听套接字的evconnlistener
	static struct evconnlistener* SyZinxListen;

	//客户端port
	short _Port;

	//保存数据库类
	SyZinxDatabase* Database;

private: //私有构造函数
	SyZinxKernel();
	~SyZinxKernel();

private:/*对象*/
	void Run();

public:
	//自己的指针
	static SyZinxKernel* mSyZinx;
private:

	//保存通道协议业务层的链表
	std::map<Client_fd, struct bufferevent*> iBufferevent_Map;
	std::map<Client_fd, Ichannel*> Ichannel_Map;
	std::map<Client_fd, Iprotocol*> IcProtocol_Map;
	std::map<Client_fd, Irole*> IRole_Map;

private:



private:
	bool SyZinxKernelExit = true;
};

#ifdef _USE_DATABASE_
//mysql数据库管理类,待实现(暂定非单例模式)
class SyZinxDatabase 
{
#define _HOST_      "192.168.12.217"
#define _USER_      "root"
#define _PASSWD_    "123456"
#define _DATABASE_  "mytest"

public:

	SyZinxDatabase();
	~SyZinxDatabase();

	//初始化一个数据库
	int SyZinxKernel_MysqlInit();

	//连接到数据库并设定编码
	int SyZinxKernel_ConnetMysql(char* _SetiCharacter);

	//连接到自定义数据库
	int SyZinxKernel_ConnetMysql(char *Host, char* User, char* Passwd , char *Database, char* _SetiCharacter);

	//关闭数据库
	int SyZinxKernel_CloseMysql();

private:
	//设值数据库编码
	int SyZinxKernel_MysqlSetCharacter(char* _iCharacter);


	MYSQL* mSyZinxSql;
	char  Host[16];
	char User[12];
	char Passwd[12];
	char Database[12];
	
};
#endif

//回收站功能将在后续添加   ----待实现----
class MemRecycled : public SyZinxKernel
{
public:
	MemRecycled() {}
	~MemRecycled();

private:
	MemRecycled(MemRecycled* _recycler) = delete;
	MemRecycled& operator=(MemRecycled* _recycler) = delete;
};

#endif
