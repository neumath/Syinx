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
@  -用户需要在该任务流程处理函数内写入你所后续规划的其他函数
@每当客户端发来数据时都会调用该函数
*/
void* IChannelTaskProcessing(void* arg)
{
	auto mIC = (IChannel*)arg;
	string str;
	mIC->RecvAllDataToString(str);
	cout << "str:" << str << endl;
}
int main(int argc, char* argv[])
{
	SyinxKernelWork a(argc, argv);

	return a.SyinxKernelExit;
}
