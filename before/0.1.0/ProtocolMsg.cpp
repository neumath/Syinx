#include "ProtocolMsg.h"

GameMsg::GameMsg()
{
}

GameMsg::~GameMsg()
{
	for (auto& it : this->mGameMsgList)
	{
		auto _freeTlv = it;
		delete _freeTlv;
	}

}

//业务层遍历完链表的包后都要进行清空名
void GameMsg::GameMsg_FreePackage()
{

}

void GameMsg::GameMsg_FreeOutPackage()
{

}


GameSingleTLV::GameSingleTLV(GameMsgType type, std::string content)
{
	this->m_MsgType = type;
	switch (type)
	{
	case GameSingleTLV::GAME_MSG_LOGON_SYNCPID:  //同步玩家名字id
		this->m_pbMsg = new pb::SyncPid;
		break;

	case GameSingleTLV::GAME_MSG_TALK_CONTENT:   //玩家聊天信息
		this->m_pbMsg = new pb::Talk;
		break;
	case GameSingleTLV::GAME_MSG_NEW_POSTION:   //同步玩家位置
		this->m_pbMsg = new pb::Position;
		break;
	case GameSingleTLV::GAME_MSG_BROADCAST:     //广播消息
		this->m_pbMsg = new pb::BroadCast;
		break;
	case GameSingleTLV::GAME_MSG_LOGOFF_SYNCPID: //玩家离线消息
		this->m_pbMsg = new pb::SyncPid;
		break;
	case GameSingleTLV::GAME_MSG_SUR_PLAYER:    //同步周边玩家消息
		this->m_pbMsg = new pb::SyncPlayers;
		break;
	default:
		;
	}

	//将content反序列化
	if (m_pbMsg != NULL)
	{
		this->m_pbMsg->ParseFromString(content);
	}
}

GameSingleTLV::GameSingleTLV(GameMsgType type, ::google::protobuf::Message* poObject)
{
	this->m_MsgType = type;
	this->m_pbMsg = poObject;
}

GameSingleTLV::~GameSingleTLV()
{
	if (m_pbMsg != nullptr)
	{
		delete m_pbMsg;
		m_pbMsg = nullptr;
	}
}
