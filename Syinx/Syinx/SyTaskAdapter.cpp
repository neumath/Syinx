#include "../Syinx/SyInc.h"
#include "Syinx.h"
#include "SyResAdapter.h"
#include "SyTaskAdapter.h"
#include "../Sylog/SyLog.h"
#include "../Sylog/easylogging++.h"

IChannel::IChannel()
{
	IChStatus = 0;
	m_ClientID = 0;
	m_buffer = nullptr;
	m_Socket = 0;
	m_pPlayer = nullptr;
	DoNotBindParse = false;
	m_ClientBuffer.clear();
}

IChannel::~IChannel()
{
	IChStatus = 0;
	m_ClientID = 0;
	m_buffer = nullptr;
	m_pPlayer = nullptr;
	m_Socket = 0;
	DoNotBindParse = false;
	m_ClientBuffer.clear();
}


bool IChannel::RecvValuesFromString(string& _InStr, uint32_t& _OutLen, uint32_t& _OutType, std::string& _OutStr)
{
	if (_InStr.size() >= 8)
	{
		unsigned int ValuesLen = 0;
		unsigned int ValuesType = 0;
		if (!g_pSyinx.GetEndian())
		{
			ValuesLen =
				_InStr[0] |
				_InStr[1] << 8 |
				_InStr[2] << 16 |
				_InStr[3] << 24;

			ValuesType =
				_InStr[4] |
				_InStr[5] << 8 |
				_InStr[6] << 16 |
				_InStr[7] << 24;
		}
		else
		{
			ValuesLen = _InStr[0] |
				_InStr[1] |
				_InStr[2] |
				_InStr[3];
			ValuesType = _InStr[4] |
				_InStr[5] |
				_InStr[6] |
				_InStr[7];
		}

		_OutStr = _InStr.substr(8, ValuesLen);
		_OutLen = ValuesLen;
		_OutType = ValuesType;

		_InStr.erase(0, 8 + ValuesLen);
		return true;
	}
	else if (_InStr.size() <= 8)
	{
		_InStr.clear();
		//包不全
		return false;
	}
	return false;
}

bool IChannel::RecvClientPack(uint32_t& _InLen, uint32_t& _InType, std::string& _InStr)
{
	LOG(INFO) << "Recv Pack";
	return true;
}



bool IChannel::SendValuesToString(uint32_t _InLen, uint32_t _InType, std::string& _InStr)
{
	std::string _SendStr;
	_SendStr.reserve(_InLen + 8);
	int _Num = 0;
	if (_InLen > 0)
	{
		_SendStr.push_back(_InLen & 0xff);
		_SendStr.push_back((_InLen >> 8) & 0xff);
		_SendStr.push_back((_InLen >> 16) & 0xff);
		_SendStr.push_back((_InLen >> 24) & 0xff);

		//type
		_SendStr.push_back(_InType & 0xff);
		_SendStr.push_back((_InType >> 8) & 0xff);
		_SendStr.push_back((_InType >> 16) & 0xff);
		_SendStr.push_back((_InType >> 24) & 0xff);

		_SendStr.append(_InStr);
		int iRet = bufferevent_write(m_buffer, _SendStr.c_str(), _SendStr.size());
		if (iRet < 0)
		{
			return false;
		}
		return true;
	}
	else
		return false;
}

void IChannel::SetupFrameInputFunc()
{

	BindParse(&IChannel::RecvValuesFromString, &IChannel::RecvClientPack);
}

void IChannel::GetClientFrameOnBuffer(std::string& _instr)
{
	m_ClientBuffer.push_back(_instr);
}

inline bool IChannel::GetWhetherBindParseFunc()
{
	return DoNotBindParse;
}

bool IChannel::Initialize()
{
	SetupFrameInputFunc();
	if (!GetWhetherBindParseFunc())
	{
		//没有绑定
		LOG(ERROR) << "Not Bind Parse";
		return false;
	}

	/* 再次添加初始化的函数 */
	return true;
}

int IChannel::OnClientConnect(int _fd, bufferevent* _buffer, int _id)
{
	m_Socket = _fd;
	if (nullptr != _buffer)
	{
		m_buffer = _buffer;
	}

	m_ClientID = _id;
	return 1;
}

void IChannel::OnStatusDoAction()
{
	for (auto Iter : m_ClientBuffer)
	{
		uint32_t PackLen = 0;
		uint32_t PackType = 0;
		string OutStr;
		while ((this->*ParseFunc)(Iter, PackLen, PackType, OutStr))
		{
			(this->*CallFunc)(PackLen, PackType, OutStr);
		}
	}
	m_ClientBuffer.clear();
}

bool IChannel::Clear()
{
	m_Socket = 0;
	IChStatus = CLIENT_LOGOUT;
	if (nullptr !=m_buffer)
	{
		bufferevent_free(m_buffer);
		m_buffer = nullptr;
	}
	m_ClientID = 0;
	m_ClientBuffer.clear();

	/* 再次添加 */
	return true;
}


int IChannel::GetSocket() const
{
	return m_Socket;
}

bufferevent* IChannel::GetBuffer()
{
	return m_buffer != nullptr ? m_buffer : nullptr;
}

int IChannel::GetUniqueID() const
{
	return m_ClientID;
}

CPlayer* IChannel::GetCPlayer()
{
	return m_pPlayer;
}

void IChannel::BindParse(bool(IChannel::* RecvBufferParse)(std::string& _Instr, uint32_t& _OutLen, uint32_t& _OutType, std::string& _OutStr), bool(IChannel::* Callalbe)(uint32_t& _InLen, uint32_t& _InType, std::string& _InStr))
{
	if (RecvBufferParse != NULL && Callalbe != NULL)
	{
		ParseFunc = RecvBufferParse;
		CallFunc = Callalbe;
		DoNotBindParse = true;
		return;
	}
	DoNotBindParse = false;
	return;
}

