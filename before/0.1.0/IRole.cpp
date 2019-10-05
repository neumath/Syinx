#include "IRole.h"
#include "IProtocol.h"
#include "ProtocolMsg.h"

#include<iostream>
#include <memory>
#include <random>
using namespace std;

static int namepid = 0;
//定义一个随机引擎(均匀分布)
static default_random_engine gRandomEngine(time(nullptr));	//构造参数是一个随机种子
void Irole::Init_Player()
{
	//生成随机位置
	ProcRandPos();
	/*创建上线时的id和姓名消息*/
	auto gMsg = this->MakeLogonSyncPid();
	this->mLinkProtocol->Iprotocol_SendEncode_ByteString(gMsg);
	usleep(100);
	/*创建广播出生位置消息*/
	gMsg = this->MakeInitPosBroadcast();
	this->mLinkProtocol->Iprotocol_SendEncode_ByteString(gMsg);
	
	MakePlayerNewPosToAny();
}

/*
GAME_MSG_LOGON_SYNCPID = 1,	//同步玩家id和名字  s->c
GAME_MSG_TALK_CONTENT = 2,	//聊天信息
GAME_MSG_NEW_POSTION = 3,	//同步玩家位置
GAME_MSG_BROADCAST = 200,   //广播消息
GAME_MSG_LOGOFF_SYNCPID = 201,//玩家离线消息   
GAME_MSG_SUR_PLAYER = 202,  //同步周边玩家消息  s->c
*/
void Irole::Irole_RecvMsgFromProtocol(GameMsg& _iMsg)
{
	GameMsg& tmpMsg = _iMsg;
	//遍历每一个tlv包
	for (auto& it : tmpMsg.mGameMsgList)
	{
		switch (it->m_MsgType)
		{
		case GameSingleTLV::GAME_MSG_TALK_CONTENT:
		{
			auto iPlayTalk = dynamic_cast<pb::Talk*>(it->m_pbMsg);
			cout << iPlayTalk->content() << endl;
			this->ProcTalkContent(iPlayTalk->content());
			break;
		}
		case GameSingleTLV::GAME_MSG_NEW_POSTION:
		{
			auto iPlayPos = dynamic_cast<pb::Position*>(it->m_pbMsg);
			cout << "x:" << iPlayPos->x() << " y:" << iPlayPos->y() << " z:" << iPlayPos->z() << " v:" << iPlayPos->v() << endl;
			this->ProcNewPos(iPlayPos->x(), iPlayPos->y(), iPlayPos->z(), iPlayPos->v());
			break;
		}
		default:
			;
		}
	}
	//遍历完后清除所有包
	_iMsg.GameMsg_FreePackage();

}

//将玩家新玩家位置遍历发送给老玩家
void Irole::MakePlayerNewPosToAny()
{
	auto poSyzinx = SyZinxKernel::mSyZinx;
	//广播附近玩家
	for (auto& it : poSyzinx->IRole_Map)
	{
		if (it.second == this)
		{
			continue;
		}
		auto poRole = it.second;
		/*创建广播出生位置消息*/
		auto gMsg = MakeInitPosBroadcast();
		poRole->mLinkProtocol->Iprotocol_SendEncode_ByteString(gMsg);
	}
#if 0
	
	for (auto& it : poSyzinx->IRole_Map)
	{
		if (it.second == this)
		{
			continue;
		}
		usleep(500000);
		auto poRole = it.second;
		/*创建广播出生位置消息*/
		GameMsg* tmpMsg = new GameMsg;
		auto gMsg = poRole->MakeInitPosBroadcast();
		this->mLinkProtocol->Iprotocol_SendEncode_ByteString(gMsg);
	}
	
#else

	//还需要将旧玩家发送给新玩家
	//

	if (poSyzinx->IRole_Map.size() > 0)
	{
		pb::SyncPlayers* players = new pb::SyncPlayers;
		for (auto& it : poSyzinx->IRole_Map)
		{
			if (it.second == this)
			{
				continue;
			}
			std::cout << "old to new" << std::endl;
			auto poRole = it.second;
			//往protobuf 的数组中添加成员,使用add_xxx  会创建一个成员对象并且返回该对象的指针
			auto player = players->add_ps();
			player->set_pid(poRole->id);
			player->set_username(poRole->name);
			//创建一个position对象,返回该对象的指针
			auto pos = player->mutable_p();
			pos->set_x(poRole->x);
			pos->set_y(poRole->y);
			pos->set_z(poRole->z);
			pos->set_v(poRole->v);
			std::cout << poRole->x << std::endl;
			std::cout << poRole->y << std::endl;
			std::cout << poRole->z << std::endl;
			std::cout << poRole->v << std::endl;
		}
		GameMsg* _igMsg = new GameMsg;
		GameSingleTLV* singTlv = new GameSingleTLV(GameSingleTLV::GAME_MSG_SUR_PLAYER, players);
		_igMsg->mGameMsgList.push_back(singTlv);

		/*创建广播出生位置消息*/
		this->mLinkProtocol->Iprotocol_SendEncode_ByteString(_igMsg);
	}

#endif
}

/*创建上线时的id和姓名消息*/
GameMsg* Irole::MakeLogonSyncPid()
{
	
	pb::SyncPid* sync = new pb::SyncPid;
	sync->set_pid(this->id);
	sync->set_username(this->name);
	auto singTlv = new GameSingleTLV(GameSingleTLV::GAME_MSG_LOGON_SYNCPID, sync);
	auto gMsg = new GameMsg;
	gMsg->mGameMsgList.push_back(singTlv);
	return gMsg;
}
/*创建广播聊天消息*/
GameMsg* Irole::MakeTalkBroadcast(std::string _talkContent)
{
	pb::BroadCast* PlayerrTalk = new pb::BroadCast;
	PlayerrTalk->set_pid(this->id);
	PlayerrTalk->set_username(this->name);
	PlayerrTalk->set_tp(1);
	PlayerrTalk->set_content(_talkContent);
	GameSingleTLV* singTlv = new GameSingleTLV(GameSingleTLV::GAME_MSG_BROADCAST, PlayerrTalk);
	auto gMsg = new GameMsg;
	gMsg->mGameMsgList.push_back(singTlv);
	return gMsg;

}
/*创建广播出生位置消息*/
GameMsg* Irole::MakeInitPosBroadcast()
{
	
	pb::BroadCast* PlayerPos = new pb::BroadCast;
	PlayerPos->set_pid(this->id);
	PlayerPos->set_username(this->name);
	PlayerPos->set_tp(2);
	auto pos = PlayerPos->mutable_p();
	pos->set_x(this->x);
	pos->set_y(this->y);
	pos->set_z(this->z);
	pos->set_v(this->v);
	GameSingleTLV* singTlv = new GameSingleTLV(GameSingleTLV::GAME_MSG_BROADCAST, PlayerPos);
	auto gMsg = new GameMsg;
	gMsg->mGameMsgList.push_back(singTlv);
	return gMsg;
}

/*创建广播移动后新位置消息*/
GameMsg* Irole::MakeNewPosBroadcast()
{
	//当玩家移动时发送移动消息
	pb::BroadCast* bc = new pb::BroadCast;
	bc->set_pid(this->id);
	bc->set_username(this->name);
	bc->set_tp(4);
	//创建一份pos
	auto pos = bc->mutable_p();
	pos->set_x(this->x);
	pos->set_y(this->y);
	pos->set_z(this->z);
	pos->set_v(this->v);
	GameSingleTLV* singTlv = new GameSingleTLV(GameSingleTLV::GAME_MSG_BROADCAST, bc);
	auto gMsg = new GameMsg;
	gMsg->mGameMsgList.push_back(singTlv);
	return gMsg;

}
/*创建下线时的id和姓名消息*/
GameMsg* Irole::MakeLogoffSyncPid()
{

}
/*创建周围玩家位置消息*/
GameMsg* Irole::MakeSurPlays()
{

}

void Irole::ProcRandPos()
{
	this->x = gRandomEngine() % 20 + 100;
	this->y = 0;
	this->z = gRandomEngine() % 20 + 100;
	this->v = 0;
	this->id = namepid += 1;
	this->name = "player"+to_string(this->id);
}

void Irole::ProcNewPos(float _x, float _y, float _z, float _v)
{
	auto poSyzinx = SyZinxKernel::mSyZinx;
	this->x = _x;
	this->y = _y;
	this->z = _z;
	this->v = _v;

	//把位置发送给除了自己以外所有人
	if (poSyzinx->IRole_Map.size() > 0)
	{
		for (auto& it : poSyzinx->IRole_Map)
		{
			if (it.second == this)
				continue;

			auto poRole = it.second;
			/*创建玩家移动消息*/
			auto gMsg = this->MakeNewPosBroadcast();
			poRole->mLinkProtocol->Iprotocol_SendEncode_ByteString(gMsg);

		}
	}
}
//聊天信息  创建玩家聊天信息的函数
void Irole::ProcTalkContent(std::string szContent)
{
	auto poSyzinx = SyZinxKernel::mSyZinx;
	//遍历
	for (auto& it : poSyzinx->IRole_Map)
	{
		auto poRole = it.second;
		auto gMsg = this->MakeTalkBroadcast(szContent);
		poRole->mLinkProtocol->Iprotocol_SendEncode_ByteString(gMsg);
	}
}
