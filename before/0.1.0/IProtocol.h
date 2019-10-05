#include "IChannel.h"
#include "ProtocolMsg.h"

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

//class UserData;

class Iprotocol : public SyZinxHandle
{

public:
	Iprotocol() {}
	~Iprotocol() {}

	//协议层该函数获得通道层发来的消息
	void Iprotocol_RecvString(UserData& _iData);

	//---可调用---该函数需要调用上面的函数来接收并且进行一个反序列化,然后将任务链表发送到业务层
	void Iprotocol_RecvDecode_ByteString(std::string &_iputs);

	//---可调用---该函数需要序列化结构体后发送给客户端
	void Iprotocol_SendEncode_ByteString(GameMsg* _getmsg);

	//协议层解析完数据之后需要将解码后的结构体发送到业务层进行处理
	virtual IZinxSys* InternalHandle(UserData& _Input) { return nullptr; }

	/*获取下一个处理环节函数，开发者重写该函数，可以指定当前处理环节结束后下一个环节是什么，通过参数信息对象，可以针对不同情况进行分别处理*/
	virtual SyZinxHandle* GetNextHandler()override;
public:
	//保存字节序的类
	ByteString* mClient_ByteString;

	//用来保存客户端发来的
	std::string mLastBuffer;

public:
	Ichannel* mLinkChannel;
	Irole* mLinkRole;

	//保存当前客户端bufferevent
	struct bufferevent* mClient_buffer;

	//客户端套接字
	int mClient_Fd;
};

#endif
