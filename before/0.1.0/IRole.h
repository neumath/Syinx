
#ifndef _ROLE_H_
#define _ROLE_H_
#include "IChannel.h"
#include "IProtocol.h"
#include "PlayerState.h"

#define _ROLE_BEHAVIOR_    public:
#define _ROLE_BATTLE_	   public:
#define _ROLE_SOCIAL_      public:

class Irole : public SyZinxHandle
{

public:
	Irole() {}
	~Irole() {}
	//初始化玩家信息
	void Init_Player();

	//业务层收到一个msg包后进行任务处理
	void Irole_RecvMsgFromProtocol(GameMsg &_iMSg);

	//业务层将msg包发送到协议层进行封装  -现协议层拥有函数可以直接调用接收数据-
	void Irole_SendMsgToProtocol(GameMsg& _iMSg);

	/*信息处理函数，开发者重写该函数实现信息的处理，当有需要一个环节继续处理时，应该创建新的信息对象（堆对象）并返回指针*/
	inline virtual IZinxSys* InternalHandle(UserData& _Input) { return nullptr; }

	/*获取下一个处理环节函数，开发者重写该函数，可以指定当前处理环节结束后下一个环节是什么，通过参数信息对象，可以针对不同情况进行分别处理*/
	inline virtual SyZinxHandle* GetNextHandler() { return nullptr; }

public: /*------------业务处理函数-----------------*/
_ROLE_BEHAVIOR_ /*行为*/
	//将玩家新玩家位置遍历发送给老玩家
	void MakePlayerNewPosToAny();

	/*创建上线时的id和姓名消息*/
	GameMsg* MakeLogonSyncPid();

	/*创建广播聊天消息*/
	GameMsg* MakeTalkBroadcast(std::string _talkContent);

	/*创建广播出生位置消息*/
	GameMsg* MakeInitPosBroadcast();

	/*创建广播移动后新位置消息*/
	GameMsg* MakeNewPosBroadcast();

	/*创建下线时的id和姓名消息*/
	GameMsg* MakeLogoffSyncPid();

	/*创建周围玩家位置消息*/
	GameMsg* MakeSurPlays();

public://玩家属性函数
	//设置玩家随机出生点
	void ProcRandPos();
	//玩家移动后实时设置玩家的xyzv
	void ProcNewPos(float _x, float _y, float _z, float _v);
	//聊天信息  创建玩家聊天信息的函数
	void ProcTalkContent(std::string szContent);
public://保存玩家信息
	//后续将会划分玩家状态为单独一个类
	float x;
	float y;
	float z;
	float v;
	int id;
	std::string name;

public:
	Ichannel* mLinkChannel;
	Iprotocol* mLinkProtocol;

	//保存当前客户端bufferevent
	struct bufferevent* mClient_buffer;

	//客户端套接字
	int mClient_Fd;
};

#endif
