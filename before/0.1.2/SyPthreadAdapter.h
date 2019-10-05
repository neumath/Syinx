

#include "SyPthreadPool.h"
class SyinxAdapter;
//线程传递参数
struct PthTransmitMsg
{
	//传递资源适配器map地址
	std::multimap<SOCKETS, bufferevent*>* sChannelMap;
	struct event_base* sBase;
};

//线程适配器
class SyinxAdapterPth :public SyinxAdapter
{
	friend class SyinxKernel;
	friend class SyinxAdapterResource;
public:
	SyinxAdapterPth();
	SyinxAdapterPth(int PthNum) : PthNum(PthNum) {}
	~SyinxAdapterPth();

	//初始化线程池
	int SyinxAdapterPth_Init();
	//工作长线程与短线程分开

   //向线程池添加一个通道层
	int SyinxAdapter_Pth_Add();

	//销毁线程池
	int SyinxAdapter_Pth_destroy();

	//获取当前任务线程数量
	int getPthNum() const;
public:

	//保存服务器的线程数量
	int PthNum;//测试为5

private:
	//绑定资源适配器
	SyinxAdapterResource* mPthRes;

	//绑定框架地址
	SyinxKernel* mSyinx;

	//保存线程池属性结构体
	threadpool_t* mSyinxPthPool;

};