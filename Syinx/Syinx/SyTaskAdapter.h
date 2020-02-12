#ifndef _SYTASKADAPTER_H_
#define _SYTASKADAPTER_H_
#include <vector>
class SyinxLog;
class SyinxAdapterResource;
class IChannel;
class CPlayer;
struct bufferevent;

class IChannel 
{
	friend void SyinxKernel_Recv_Cb(struct bufferevent* bev, void* ctx);
	friend class SyinxAdapterResource;
public:
	enum ICNANNEL_LINK_STATUS
	{
		CLIENT_LOGOUT = 1,
		CLIENT_LOGIN = 2,
	};
private:
	/*
	* 当前ICh被分配的一个Socket套接字
	*/
	int					 m_Socket;

	/*
	* 被分配的一个客户端ID
	*/
	int					 m_ClientID;

	/*
	* 给客户端分配的一个libevent  bufferevent 缓存
	*/
	bufferevent*		 m_buffer;

	/*
	* 玩家的指针
	*/
	CPlayer* m_pPlayer;

	/*
	* 玩家的Frame缓存
	*/
	std::vector<string>	 m_ClientBuffer;

	typedef bool (IChannel::*RecvBufferParse)(std::string& _Instr, uint32_t& _OutLen, uint32_t& _OutType, std::string& _OutStr);
	RecvBufferParse ParseFunc;

	typedef bool (IChannel::*Callalbe)(uint32_t& _InLen, uint32_t& _InType, std::string& _InStr);
	Callalbe		CallFunc;

	bool DoNotBindParse;
public:
	IChannel();
	~IChannel();
	/*
	*初始化一个通道层
	*/
	bool Initialize();

	/*
	*当客户端连接时分配一个buffer
	*/
	int  OnClientConnect(int _fd, bufferevent* buffer, int m_ClientID = 0);

	/*
	* 读取做逻辑处理
	*/
	void OnStatusDoAction();

	/*
	*	必须告诉框架,绑定接收到的逻辑帧做如何的解析 
	*
	*/
	void SetupFrameInputFunc();

	/*
	* 获取客户端发来的Frame放入到缓存中
	*/
	void GetClientFrameOnBuffer(std::string& _instr);

	/*
	* 查看是否用户绑定了解析函数
	*/
	inline bool GetWhetherBindParseFunc();



	//协议解析
	bool RecvValuesFromString(string& _InStr, uint32_t& _OutLen, uint32_t& _OutType, std::string& _OutStr);
	bool RecvClientPack(uint32_t& _InLen, uint32_t& _InType, std::string& _InStr);

	bool SendValuesToString(uint32_t _InLen, uint32_t _InType, std::string& _InStr);

	
	/*
	* 当客户端退出时调用该函数用来清理当前的缓存
	*/
	bool Clear();

	/*
	
	*/
	int					GetUniqueID()const;
	int					GetSocket()const;
	bufferevent*		GetBuffer();
	CPlayer*			GetCPlayer();
	int					IChStatus;
private:
	/*
	*绑定处理的或者解析的函数需要用户自己定义如何去解析
	bool RecvBufferParse(std::string& _Instr, uint32_t& _OutLen, uint32_t& _OutType, std::string& _OutStr);
	_InStr是一个传入的原始字符串
	_OutLen是解析完后传出的长度
	_OutType是解析完后传出的类型
	_OutStr是解析完后传出的IO包

	返回值处理:IO传输过程中可能会发现粘包,如果一个InStr中包含两个逻辑帧,那么在解析完最后一个帧后必须告诉框架返回false,以做防粘包处理
	否则肯定会发生死循环

	bool Callalbe(uint32_t& _InLen, uint32_t& _InType, std::string& _InStr)
	//当解析玩IO后将会调用Callalbe根据之前的调用顺序进行传入

	*/
	void BindParse(bool (IChannel::*RecvBufferParse)(std::string& _Instr, uint32_t& _OutLen, uint32_t& _OutType, std::string& _OutStr), bool (IChannel::* Callalbe)(uint32_t& _InLen, uint32_t& _InType, std::string& _InStr));
	
};

#endif
