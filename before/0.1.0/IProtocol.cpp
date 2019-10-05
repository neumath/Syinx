#include "IProtocol.h"
#include "IRole.h"
#include "IChannel.h"
#include <memory>
void Iprotocol::Iprotocol_RecvString(UserData& _iData)
{
	ByteString& IpRecvString = dynamic_cast<ByteString&>(_iData);
	Iprotocol_RecvDecode_ByteString(IpRecvString._instring);
}

void Iprotocol::Iprotocol_RecvDecode_ByteString(std::string& _iputs)
{
	this->mLastBuffer.append(_iputs);

	GameMsg* _igMsg = new GameMsg;

	//std::cout << "Iprotocol_Decode_ByteString Recv Data : " << mLastBuffer<< mLastBuffer.size() << std::endl;
	while (mLastBuffer.size() >= 8)
	{
		unsigned int msgLen =
			mLastBuffer[0] |
			mLastBuffer[1] << 8 |
			mLastBuffer[2] << 16 |
			mLastBuffer[3] << 24;

		unsigned int msgType =
			mLastBuffer[4] |
			mLastBuffer[5] << 8 |
			mLastBuffer[6] << 16 |
			mLastBuffer[7] << 24;

		std::cout << "msgLen:"<< msgLen <<" msgType:"<< msgType << std::endl;
		if (mLastBuffer.size() >= 8 + msgLen)
		{
			//说明有数据报
			auto SingleTlv = new GameSingleTLV((GameSingleTLV::GameMsgType)msgType, mLastBuffer.substr(8, msgLen));
			//入任务链
			_igMsg->mGameMsgList.push_back(SingleTlv);
			mLastBuffer.erase(0, 8 + msgLen);
		}
		else
		{
			break;
		}
	}
	std::cout << "mGameMsg recv " << _igMsg->mGameMsgList.size() << " package" << std::endl;

	this->mLinkRole->Irole_RecvMsgFromProtocol(*_igMsg);

	//free
	if (_igMsg != NULL)
	{
		delete _igMsg;
		_igMsg = nullptr;
	}
}

void Iprotocol::Iprotocol_SendEncode_ByteString(GameMsg* _getmsg)
{
#if 1
	if (_getmsg == NULL)
		return;
	std::string _ret;
	
	std::string* buf = &(mClient_ByteString->_outstring);
	//std::string buf;
	//遍历每一个传出包将包序列化
	for (auto& it : _getmsg ->mGameMsgList)
	{
		if (it->m_pbMsg != nullptr)
			it->m_pbMsg->SerializePartialToString(&_ret);
		unsigned int msgLen = (unsigned int)_ret.size();
		unsigned int msgType = (unsigned int)it->m_MsgType;
		
		std::cout << "msgLen:" << msgLen << " msgType:" << msgType << std::endl;

		buf->push_back((char)(msgLen & 0xff));
		buf->push_back((char)(msgLen >> 8 & 0xff));
		buf->push_back((char)(msgLen >> 16 & 0xff));
		buf->push_back((char)(msgLen >> 24 & 0xff));
							  
		buf->push_back((char)(msgType & 0xff));
		buf->push_back((char)((msgType >> 8) & 0xff));
		buf->push_back((char)((msgType >> 16) & 0xff));
		buf->push_back((char)((msgType >> 24 )& 0xff));
		buf->append(_ret);
		this->mLinkChannel->Ichannel_SendBuffer(*buf);
		buf->erase(0, 8 + msgLen);
	}
	if (_getmsg != NULL)
	{
		delete _getmsg;
		_getmsg = nullptr;
	}

#else //测试直接发送
	int iRet = 0;
	if (_getmsg == NULL)
		return;
	std::string _ret;

	std::string* buf = new std::string;
	//std::string buf;
	//遍历每一个传出包将包序列化
	for (auto& it : _getmsg->mGameMsgList)
	{
		if (it->m_pbMsg != nullptr)
			it->m_pbMsg->SerializePartialToString(&_ret);
		unsigned int msgLen = (unsigned int)_ret.size();
		unsigned int msgType = (unsigned int)it->m_MsgType;

		std::cout << "msgLen:" << msgLen << " msgType:" << msgType << std::endl;

		buf->push_back((char)(msgLen & 0xff));
		buf->push_back((char)(msgLen >> 8 & 0xff));
		buf->push_back((char)(msgLen >> 16 & 0xff));
		buf->push_back((char)(msgLen >> 24 & 0xff));

		buf->push_back((char)(msgType & 0xff));
		buf->push_back((char)((msgType >> 8) & 0xff));
		buf->push_back((char)((msgType >> 16) & 0xff));
		buf->push_back((char)((msgType >> 24) & 0xff));
		buf->append(_ret);
		char* ch = new char[BUFFSIZE];
		memset(ch, 0, BUFFSIZE);

		ch = const_cast<char*>(buf->c_str());
		//写入
		iRet = send(this->mClient_Fd, ch, buf->size(), 0);
		std::cout << "iRet :" << iRet << std::endl;

	}
	if (_getmsg != NULL)
	{
		delete _getmsg;
		_getmsg = nullptr;
	}

#endif 

}

SyZinxHandle* Iprotocol::GetNextHandler()
{
	return this->mLinkRole;
}
