#include "IChannel.h"
#include "IProtocol.h"
#include "IRole.h"
#include<memory>
#include<stdio.h>
SyZinxTcpFactory::SyZinxTcpFactory(const int _iClient_fd, bufferevent* _ibufferevent)
{
	this->SyZinxTcpFactory_Init(_iClient_fd, _ibufferevent);
}

SyZinxTcpFactory::~SyZinxTcpFactory()
{
}

int SyZinxTcpFactory::SyZinxTcpFactory_Init(const int _iClient_fd, bufferevent* _ibufferevent)
{
	//初始化任务链
	Ichannel* poChannel = new Ichannel(_iClient_fd, _ibufferevent);
	Iprotocol* poProtocol = new Iprotocol;
	Irole* poRole = new Irole;
	

	//互相绑定
	poChannel->mLinkProtocol = poProtocol;
	poChannel->mLinkRole = poRole;

	poProtocol->mLinkChannel = poChannel;
	poProtocol->mLinkRole = poRole;

	poRole->mLinkChannel = poChannel;
	poRole->mLinkProtocol = poProtocol;

	//每一个用户创建后都有一块缓存,互相绑定
	poChannel->mClient_ByteString = new ByteString(_iClient_fd);
	poProtocol->mClient_ByteString = poChannel->mClient_ByteString;

	

	//绑定协议层,业务层的buffer以及fd
	poProtocol->mClient_buffer = _ibufferevent;
	poProtocol->mClient_Fd = _iClient_fd;
	poRole->mClient_buffer = _ibufferevent;
	poRole->mClient_Fd = _iClient_fd;

	poChannel->Ichannel_DataInit();

	//将客户端任务链上框架树
	SyZinxKernel::mSyZinx->Ichannel_Map.insert(std::make_pair(_iClient_fd, poChannel));
	SyZinxKernel::mSyZinx->IcProtocol_Map.insert(std::make_pair(_iClient_fd, poProtocol));
	SyZinxKernel::mSyZinx->IRole_Map.insert(std::make_pair(_iClient_fd, poRole));
	std::cout << "client factory complete" << std::endl;

	return 1;
}

Ichannel::Ichannel(const int _iClient_fd, bufferevent* _ibufferevent)
{
	this->mClient_Fd = _iClient_fd;
	this->mClient_buffer = _ibufferevent;
	
}

Ichannel::~Ichannel()
{
}

void Ichannel::Ichannel_RecvBuffer(std::string &_istring)
{
	//将数据追加到bytestring
	this->mClient_ByteString->_instring = _istring;
	//打印收到的信息
	this->mClient_ByteString->PutInString();
	//将数据发送到协议层
	InternalHandle(*mClient_ByteString);
}

void Ichannel::Ichannel_SendBuffer(std::string& _istring)
{
	this->mClient_ByteString->PutOutString(_istring);
	int iRet = 0;

	unsigned char buf[BUFSIZ] = { 0 };
	memcpy((char*)buf, _istring.c_str(),_istring.size());
#if 1
	std::cout << std::endl;
	//写入
	iRet = bufferevent_write(this->mClient_buffer, buf, _istring.size());
	if (-1 == iRet)
	{
		printf("write...file\n");
		return;
	}
	std::cout << "size :" << _istring.size() << std::endl;
#else//写入fd
	iRet = send(this->mClient_Fd, buf, _istring.size(), 0);
	std::cout << "iRet :" << iRet << std::endl;
#endif 
	
}

void Ichannel::Ichannel_DataInit()
{
	this->mLinkRole->Init_Player();
}


IZinxSys* Ichannel::InternalHandle(UserData& _Input)
{
	this->mLinkProtocol->Iprotocol_RecvString(_Input);
}

SyZinxHandle* Ichannel::GetNextHandler()
{
	return nullptr;
}

void ByteString::InString(std::string &_istring)
{
	this->_instring = _istring;
}

void ByteString::OutString(std::string &_ostring)
{
	this->_outstring = _ostring;
}

char* ByteString::StringToUnsignedChar(std::string& _outstring)
{
	char* pcTemp = (char*)calloc(1UL, _outstring.size() * 3 + 1);
	int iCurPos = 0;

	if (NULL != pcTemp)
	{
		for (int i = 0; i < _outstring.size(); ++i)
		{
			iCurPos += sprintf(pcTemp + iCurPos, "%02X ", (unsigned char)_outstring[i]);
		}
		pcTemp[iCurPos] = 0;
	}

	return pcTemp;
}

void ByteString::PutInString()
{
	std::cout << "<----------------------------------------->" << std::endl;
	std::cout << "recv from : " << this->mClient_Fd << std::endl;
	for (auto &c : this->_instring)
	{
		printf("%02X ", (uint8_t)c);

	}
	printf("\n");
	std::cout << "<----------------------------------------->" << std::endl;
	
}

void ByteString::PutOutString(std::string& _outString)
{
	std::cout << "<----------------------------------------->" << std::endl;
	std::cout << "send to : " << this->mClient_Fd << std::endl;
	for (auto &c : _outString)
	{
		printf("%02X ", (uint8_t)c);

	}
	printf("\n");
	std::cout << "<----------------------------------------->" << std::endl;
}
