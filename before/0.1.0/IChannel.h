#include "SyZinx.h"
#ifndef _CHANNEL_H_
#define _CHANNEL_H_
#define BUFFSIZE           1024    
class UserData
{
public:
	UserData() {}
	virtual~UserData() {}
	virtual void InString(std::string& _istring) = 0;
	virtual void OutString(std::string& _ostring) = 0;
};

//从客户端接收的数据
class ByteString : public UserData
{
public:
	ByteString(int client_fd) : mClient_Fd(client_fd){}
	~ByteString() {}

	//通道层缓存机制函数,当某些特殊原因导致读写失败,则将数据报放入缓存  ----待实现----
	virtual void InString(std::string &_istring) override;
	virtual void OutString(std::string &_ostring) override;

	//将string转成无符号char
	char* StringToUnsignedChar(std::string& _outstring);
	//打印当前字符串
	void PutInString();
	void PutOutString(std::string &_outString);
	int mClient_Fd;
public:
	std::string _instring;
	std::string _outstring;
};


//zinx基类,用于多态
class IZinxSys
{

public:
	IZinxSys() {}
	virtual~IZinxSys() {}
};

class SysIODirection : public IZinxSys
{

public:
	enum IO_DIC {
		IN = 1,//传入
		OUT  //传出
	}mio_dic; //IO_DIC mio_dic;
	SysIODirection(IO_DIC _dic) :mio_dic(_dic) {}

};

class SyZinxHandle :public IZinxSys
{

public:
	SyZinxHandle() {}
	virtual ~SyZinxHandle() {}

public:
	/*信息处理函数，开发者重写该函数实现信息的处理，当有需要一个环节继续处理时，应该创建新的信息对象（堆对象）并返回指针*/
	virtual IZinxSys* InternalHandle(UserData& _Input) = 0;

	/*获取下一个处理环节函数，开发者重写该函数，可以指定当前处理环节结束后下一个环节是什么，通过参数信息对象，可以针对不同情况进行分别处理*/
	virtual SyZinxHandle* GetNextHandler() = 0;
};


class Ichannel : public SyZinxHandle
{
	friend class SyZinxTcpFactory;
	friend class Iprotocol;
public:
	Ichannel(const int _iClient_fd, bufferevent* _ibufferevent);
	~Ichannel();

public:

	//当需要对玩家进行初始化操作是调用该函数
	void Ichannel_DataInit();

	//当有可读事件触发是要调用此函数用来接收可读的字符串
	void Ichannel_RecvBuffer(std::string& _istring);

	//服务器有数据发送到客户端时调用
	void Ichannel_SendBuffer(std::string& _istring);

	

	//信息处理函数当接收完数据时将数据发送到协议层解析
	virtual IZinxSys* InternalHandle(UserData& _Input)override;

	/*获取下一个处理环节函数，开发者重写该函数，可以指定当前处理环节结束后下一个环节是什么，通过参数信息对象，可以针对不同情况进行分别处理*/
	virtual SyZinxHandle* GetNextHandler()override;
private:

	//保存当前客户端bufferevent
	struct bufferevent* mClient_buffer;

	//客户端套接字
	int mClient_Fd;

	//保存数据的类
	ByteString* mClient_ByteString;
	//保存协议层
	Iprotocol* mLinkProtocol;
	//保存业务层
	Irole* mLinkRole;
};


class SyZinxTcpFactory
{
public:
	SyZinxTcpFactory(const int _iClient_fd, struct bufferevent* _ibufferevent);
	~SyZinxTcpFactory();

private:
	//用于创建一系列的通道 协议 业务层
	 int SyZinxTcpFactory_Init(const int _iClient_fd, struct bufferevent* _ibufferevent);


};

#endif 