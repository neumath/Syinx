#include "Syinx.h"
#include "SyTaskAdapter.h"

#include <iostream>
using namespace std;

//用户可以指定该构造函数需要初始化或者用来创建哪些东西
IChannel::IChannel()
{
}
//用户可以指定该析构构造函数需要释放哪些东西
IChannel::~IChannel()
{
}
/*
@  -用户需要在该初始化函数,作为客户端的初始化工作
@每当客户端连接服务器的时都会默认调用该函数
*/
int IChannel::IChannelTaskInit()
{
	std::string _send = "hello world";
	this->SendAllDataToString(_send);
}
/*
@  -用户需要在该任务流程处理函数内写入你所后续规划的其他函数
@每当客户端发来数据时都会调用该函数
*/
int IChannel::IChannelTaskProcessing()
{
	string Test;
	this->RecvAllDataToString(Test);
	cout << Test << endl;
	
}
int main()
{
	SyinxKernelWork a(8855);
	
	return a.SyinxClose();
}
