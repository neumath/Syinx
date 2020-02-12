#include "SyInc.h"
#include "../module/SyPthreadAdapter.h"
#include "../module/SyPthreadPool.h"
#include "../Sylog/SyLog.h"
#include "SyResAdapter.h"
#include "../Sylog/easylogging++.h"
#include "SyTaskAdapter.h"
#include "Syinx.h"


#include "../module/rapidjson/document.h"
#include "../module/rapidjson/stringbuffer.h"
#include "../module/rapidjson/prettywriter.h"
using namespace rapidjson;
using rapidjson::Document;
using rapidjson::Value;
using namespace std;


SyinxKernel& g_pSyinx = SyinxKernel::MakeSingleton();

uint64_t	g_nGameServerSecond = 0;

tm	g_tmGameServerTime;
SyinxKernel& SyinxKernel::MakeSingleton()
{
	static SyinxKernel syinx;
	return syinx;
}

SyinxKernel::SyinxKernel()
{
	m_Port = 0;
	m_ClientContentNum = 0;
	m_PthPoolNum = 0;
	m_TaskNum = 0;
	m_TimerSec = 0;
	m_endian = 0;
	mSyinxBase = nullptr;
	mSyinxListen = nullptr;
	mSyPth = nullptr;
	mSyResource = nullptr;
	mUsePthreadPool = false;
	m_DBServer = nullptr;
	m_nWorkStatus = SYINX_LINK_CLOSE;

	memset(m_SyinxStatusFunc, 0, sizeof(m_SyinxStatusFunc));
	m_SyinxStatusFunc[SYINX_LINK_WORK] = &SyinxKernel::OnStatusDoAction;
	m_SyinxStatusFunc[SYINX_LINK_CLOSE] = &SyinxKernel::OnStatusDoClose;

}
SyinxKernel::~SyinxKernel()
{
	m_Port = 0;
	m_ClientContentNum = 0;
	m_PthPoolNum = 0;
	m_TaskNum = 0;
	m_TimerSec = 0;
	m_endian = 0;
	mSyinxBase = nullptr;
	mSyinxListen = nullptr;
	mSyPth = nullptr;
	mSyResource = nullptr;
	m_DBServer = nullptr;
	m_nWorkStatus = SYINX_LINK_CLOSE;
	SyinxKernel_Close();
}

//读出回调
void SyinxKernel_Recv_Cb(struct bufferevent* bev, void* ctx)
{
	auto ICh = GETICHANNEL;
	if (ICh == nullptr)
		return;

	char _buffer[READBUFFER] = { 0 };
	int iRet = bufferevent_read(bev, _buffer, READBUFFER);
	if (iRet < 0)
		return;
	string _str(_buffer, iRet);
	ICh->GetClientFrameOnBuffer(_str);

}
//写事件回调
void SyinxKernel_Send_Cb(struct bufferevent* bev, void* ctx)
{

}

//事件回调
void SyinxKernel_Event_Cb(struct bufferevent* bev, short what, void* ctx)
{
	auto mIC = GETICHANNEL;
	auto Res = g_pSyinx.GetResource();
	if (what & BEV_EVENT_EOF) // Client端关闭连接 
	{
		Res->SocketFdDel(mIC);
	}
	else if (what & BEV_EVENT_ERROR) // 连接出错 
	{
		Res->SocketFdDel(mIC);
	}
	else if (what & BEV_EVENT_WRITING)//写入时发生做错误
	{
		Res->SocketFdDel(mIC);
	}
}

void SIG_HANDLE(int Sig)
{
	switch (Sig)
	{
	case SIGINT:
		g_pSyinx.SyinxKernel_Close();
		break;
#ifdef __linux__
	case SIGQUIT:
		g_pSyinx.SyinxKernel_Close();
		break;
#endif
	default:
		break;
	}
}



//如果有客户端连接
void SyinxKernel_Listen_CB(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* arg)
{
	int iRet = 0;
	auto _base = g_pSyinx.GetEventBase();
	if (nullptr == _base)
	{
		//log
		return;
	}
	struct bufferevent* buffer = NULL;
	buffer = bufferevent_socket_new(_base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (nullptr == buffer)
	{
		LOG(ERROR) << "bufferevent_socket_new is failed";
		return;
	}
	
	//将将新来的客户端委托资源管理器来管理
	auto res = g_pSyinx.GetResource();
	res->AllocationIChannel(buffer, fd);
	return;
}

bool SyinxKernel::Initialize()
{
	m_endian = JudgeSystem();

	//再次添加初始化模块
	if (!SyinxKernelReadconfig())
	{
		LOG(ERROR) << "Read Config Failed";
		return false;
	}
	if (!SyinxKernelInitAdapter())
	{
		LOG(ERROR) << "SyinxKernel_InitAdapter() Failed";
		return false;
	}

	//初始化结构体
	struct sockaddr_in _Serveraddr;
	memset(&_Serveraddr, 0, sizeof(_Serveraddr));
	_Serveraddr.sin_family = AF_INET;
	_Serveraddr.sin_port = htons(m_Port);

#ifdef WIN32
	WORD wVersionRequested = 0;
	WSADATA wsaData;
	int err;
	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		LOG(ERROR) << "WSAStartup failed with error: %d\n" << err;
		return false;
	}

#endif

	//创建句柄
	if (!mSyinxBase)
	{
		struct event_config* cfg = event_config_new();
		if (cfg) {
			struct event_config* cfg = event_config_new();
			/*
			To access base security , unallocable use thread call base
			*/
#if _WIN32
			evthread_use_windows_threads();
			event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
#elif defined (__linux__)
			event_config_set_flag(cfg, EVENT_BASE_FLAG_NOLOCK);
#endif
			mSyinxBase = event_base_new_with_config(cfg);
			if (mSyinxBase == nullptr)
			{
				LOG(ERROR) << "event_base_new() Failed";
				return false;
			}
			event_config_free(cfg);
		}
	}

	//bind

	mSyinxListen = evconnlistener_new_bind(mSyinxBase, SyinxKernel_Listen_CB, nullptr,
		SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING, 10, (const sockaddr*)&_Serveraddr, sizeof(_Serveraddr));
	if (nullptr == mSyinxListen)
	{
		LOG(ERROR) << "Create Base  : evconnlistener_new_bind is failed";
		return false;
	}


	m_nWorkStatus = SYINX_LINK_WORK;
	return true;
}

bool SyinxKernel::SyinxKernelReadconfig()
{
	ifstream in;
	string line;

	string str;
	in.open(GAME_CONFIG_PATH, ifstream::in);
	if (!in.is_open())
	{
		LOG(ERROR) << "Syinx-Server.config.json NOT FIND";
		return false;
	}
	while (getline(in, line))
	{
		if (*line.begin() == '#')
			continue;
		str.append(line + "\n");
	}
	in.close();

	//parserJson for Syinx-Server.config.json
	Document  doc;
	doc.Parse<0>(str.c_str());
	if (doc.HasParseError())
	{
		LOG(ERROR) << "HasParseError";
		return false;
	}
	Value& GameServer = doc["GameServer"];
	if (!GameServer["Port"].IsInt())
	{
		return false;
	}
	m_Port = GameServer["Port"].GetInt();

	m_ClientContentNum = GameServer["ConnectNum"].GetInt();

	m_TimerSec = GameServer["Timer-Sec"].GetInt();

	Value& Pthv = GameServer["PthreadPool"];

	m_PthPoolNum = Pthv["PthNum"].GetInt();

	m_TaskNum = Pthv["TaskNum"].GetInt();

	mUsePthreadPool = GameServer["UsePthreadPool"].GetBool();

	Value& ConnectServer = GameServer["ConnectServer"];
	m_DBPort = ConnectServer["DBServerPort"].GetInt();


	return true;
}

bool SyinxKernel::SyinxKernelInitAdapter()
{
	//初始化线程管理器
	if (m_PthPoolNum > 1 && mUsePthreadPool)
	{
		mSyPth = new SyinxAdapterPth(m_PthPoolNum, m_TaskNum);
		if (nullptr == mSyPth)
		{
			//log
			LOG(ERROR) << "new SyinxAdapterPth failed...";
			return false;
		}
		if (mSyPth->SyinxAdapterPth_Init())
		{
			LOG(INFO) << "SyinxAdapterPth_Init Success";
		}
		else
		{
			LOG(ERROR) << "new SyinxAdapterPth failed...";
			return false;
		}
	}
	else
		LOG(INFO) << "Pthread is not Init/ set Pool num = 0 or UsePthreadPool = false";
	


	//初始化资源管理器
	mSyResource = new SyinxAdapterResource(m_ClientContentNum);
	if (nullptr == mSyResource)
	{
		//log
		LOG(ERROR) << "new SyinxAdapterResource failed...";
		return false;
	}
	if (mSyResource->Initialize())
	{
		LOG(INFO) << "SyinxAdapterResource_Init Success";
	}
	else
	{
		LOG(ERROR) << "SyinxAdapterResource_Init failed...";
		return false;
	}
	return true;
}

bool SyinxKernel::RegisterSignal()
{
	signal(SIGINT, SIG_HANDLE);
#ifdef __linux__
	signal(SIGQUIT, SIG_HANDLE);
#endif
	return true;
}

void SyinxKernel::OnStatusDoAction()
{
	mSyResource->GameServerDoAction();
}

void SyinxKernel::OnStatusDoClose()
{
	SyinxKernel_Close();
}

void SyinxKernel::SyinxKernel_Run()
{
	int iRet = 0;
	uint64_t BeginMeslTime = 0;
	uint64_t NextMeslTime = 0;

	while (m_nWorkStatus)
	{
		iRet = event_base_loop(mSyinxBase, EVLOOP_NONBLOCK);
		if (-1 == iRet)
		{
			return;
		}
		BeginMeslTime = GetMselTime();
		if (BeginMeslTime < NextMeslTime)
		{
			Skipping(1);
			continue;
		}
		NextMeslTime = BeginMeslTime + 1000 / GAME_SERVER_FRAME_NUMS;

		g_nGameServerSecond = time(nullptr);

		time_t tmGameServerTime = g_nGameServerSecond;

#if defined(WIN32) || defined(WIN64)
		localtime_s(&g_tmGameServerTime, &tmGameServerTime);
#elif defined(__linux)
		localtime_r(&tmGameServerTime, &g_tmGameServerTime);
#endif
		(this->*m_SyinxStatusFunc[m_nWorkStatus])();
		

	}
}

void SyinxKernel::SyinxKernel_Close()
{
	if (!m_nWorkStatus)
		return;
	m_nWorkStatus = SYINX_LINK_CLOSE;

	if (mSyPth != nullptr)
	{
		mSyPth->SyinxAdapterPth_destroy();
		delete mSyPth;
		mSyPth = nullptr;
	}

	if (mSyResource != nullptr)
	{
		mSyResource->Close();
		delete mSyResource;
		mSyResource = nullptr;
	}
	
	if (mSyinxBase != nullptr)
	{
		event_base_free(mSyinxBase);
#ifdef WIN32
		WSACleanup();
#endif 
	}
	puts("Syinx is close! \nPress any key to continue!...");
	getchar();
	exit(0);
}

int SyinxKernel::JudgeSystem(void)
{
	int a = 1;
	//如果是小端则返回1，如果是大端则返回0
	return *(char*)&a;
}

inline SyinxAdapterPth* SyinxKernel::GetPth()
{
	return mSyPth;
}
inline SyinxAdapterResource* SyinxKernel::GetResource()
{
	return mSyResource;
}
inline event_base* SyinxKernel::GetEventBase()
{
	return mSyinxBase != nullptr ? mSyinxBase : nullptr;
}
inline evconnlistener* SyinxKernel::GetListener()
{
	return mSyinxListen != nullptr ? mSyinxListen : nullptr;
}




inline int SyinxKernel::GetPthreadPoolNum() const
{
	return m_PthPoolNum;
}

inline int SyinxKernel::GetPthreadTaskNum() const
{
	return m_TaskNum;
}

int SyinxKernel::GetEndian()
{
	return m_endian;
}

uint64_t GetMselTime()
{
#ifdef WIN32
	return  GetTickCount();
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

void Skipping(const int mesltime)
{
#ifdef WIN32
	Sleep(mesltime);
#else
	usleep(mesltime*1000);
#endif
}
