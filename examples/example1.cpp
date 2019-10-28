#include "Syinx/Syinx.h"
#include "Syinx/SyTaskAdapter.h"
#include <iostream>
using namespace std;

//通道层在客户端连接到服务器时会调用一次该构造函数
IChannel::IChannel()
{

}
//当客户端释放连接时该析构函数会被调用一次
IChannel::~IChannel()
{

}


//在默认的线程处理函数之后会自动调用IChannelWork函数,用来给玩家指定任务队列加入其它自定义任务或者lua脚本
int IChannel::ICannelWork()
{

}
//void* IChannelTaskProcessing(void* arg) arg为传入参数,当客户端发来一个数据报文时会调用该函数,该函数被自动压入任务队列
//任务队列会在线程池里被处理

void* IChannelTaskProcessing(void* arg)
{
	auto mIC = (IChannel*)arg;

}
int main(int argc, char* argv[])
{
	SyinxKernelWork sy(argc, argv);
                                   

                                   
	sy.SyinxWork();
	return sy.SyinxKernelExit;
}
