#include "SyAdapter.h"

#ifndef _SYTASKADAPTER_H_
#define _SYTASKADAPTER_H_
class SyinxAdapterResource;
struct IChannelMsg
{
	
	int             Socket;                  /*保存套接字*/
	bufferevent*    buffer;                  /*保存buffer*/

	int             ClientID;                /*客户端唯一id*/
    
};
//任务处理类
class SyinxAdapterMission : public SyinxAdapter
{
public:
	SyinxAdapterMission() {}
	virtual ~SyinxAdapterMission() {}
public:
	
	//留好接收数据的坑
	virtual void RecvData() { return; }
	virtual void SendData() { return; }

};

//创建通道层工厂类用于一系列初始化
class IChannelFactory
{
public:
	IChannelFactory() {}
	~IChannelFactory() {}
};

//通道层数据流对应缓存
struct StringByte 
{
	std::string _InStr;
	int _InSize;

};
class IChannel : public SyinxAdapterMission
{
	friend class SyinxAdapterResource;
public:
	IChannel();
	virtual ~IChannel();
	//读取一条数据放置到string---已废弃
	virtual void RecvData() override;
	//发送一条数据---------------已废弃
	virtual void SendData() override;
public:
	
	/*
	@   -读取当前全部数据数据到string
	@arg:不以任何协议格式将全部数据读取string并清空占存区
	@成功返回读取到的数据长度,失败返回-1 
	*/
	int RecvAllDataToString(std::string &arg);

	/*
	@   -以协议的格式接收数据到string   len  type  values
	@arg:以任何协议格式将values的数据读取string并清空占存区
	@OutLen:成功读取到的数据的长度
	@Type:成功读取到的数据类型
	@str:成功读取到的数据
	@成功将返回1且此时说明仍然有数据可读
	@返回0时说明无剩余数据可读
	@失败返回-1
	*/
	int RecvValuesToString(unsigned int* _OutLen, unsigned int* _OutType, std::string& _OutStr);


	/*
	@   -直接发送当前string到该客户端
	@instr:直接将当前字符串发送到当前客户端
	@成功返回发送的数据长度,失败返回-1
	*/
	int SendAllDataToString(std::string &_InStr);

	/*
	@   -按协议格式转换正Asn.1的string数据包并发送 ( len  type  values)
	@Inlen:发送数据包的长度
	@InType:发送数据报的类型(自行指定可有可无)
	@InStr:需要发送的数据报
	@成功返回发送数据包的长度,失败返回-1
	*/
	int SendValuesToString(unsigned int _InLen, unsigned int _InType, std::string& _InStr);


	//保存下一个工作节点的指针
	void* _NextNode;

	/*mysql处理函数*/
#ifdef SYINXMOD_ADD_MYSQL

#endif 

	//数据缓存结构体
	StringByte* StrByte;

	//保存通道层的属性
	IChannelMsg* mICMsg;
private:
	//初始化通道层
	int ICannel_Init(IChannelMsg* Info);
	//销毁通道层
	void IChannel_free();

	//将读取到的数据放到缓存区
	int DatatoInStrByte(char* buf);

	
private://绑定资源管理器
	SyinxAdapterResource* mICnSaveRes;
};

#endif