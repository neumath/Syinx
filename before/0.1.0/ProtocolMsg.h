#include "SyZinx.h"
#include "msg.pb.h"
#include <list>
#ifndef _PROTOCLOLMSG_H_
#define _PROTOCLOLMSG_H_

//多个消息的一个组合 protocol 解析协议之后就将多个消息放到这里
class GameSingleTLV;

class GameMsg
{
public:
	GameMsg();
	virtual ~GameMsg();
	//手动释放链表将在后续9/18后摒弃
	void GameMsg_FreePackage();
	void GameMsg_FreeOutPackage();
	std::list<GameSingleTLV*> mGameMsgList;

};
//单个TLV消息
class GameSingleTLV : public GameMsg
{
public:
	//定义消息ID的内容
	enum GameMsgType {
		GAME_MSG_LOGON_SYNCPID = 1,	//同步玩家id和名字
		GAME_MSG_TALK_CONTENT = 2,	//聊天信息
		GAME_MSG_NEW_POSTION = 3,	//同步玩家位置
		GAME_MSG_BROADCAST = 200,   //广播消息
		GAME_MSG_LOGOFF_SYNCPID = 201,//玩家离线消息
		GAME_MSG_SUR_PLAYER = 202,  //同步周边玩家消息
	};
	GameMsgType m_MsgType;
	//保存一个protobuf的对象指针,protocol协议层解析完协议之后产生出来的对象
	google::protobuf::Message* m_pbMsg;

	//用于序列化为字符串的函数
	std::string Serialize();

	//传入消息ID 以及 content ,构造函数自动将string 反序列化为 protobuf对象
	GameSingleTLV(GameMsgType type, std::string content);

	//传入结构体将结构体进行序列化
	GameSingleTLV(GameMsgType type, ::google::protobuf::Message* poObject);
	~GameSingleTLV();

};






#endif