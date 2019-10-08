# Syinx for C++/Go



基于libevent的轻量级适用于游戏的服务器,使用多线程,对不同base事件树进行遍历访问,而不是对单个任务进行线程处理

Libevent-based lightweight is suitable for game servers where multiple threads are used to traverse different base event trees instead of threading individual tasks

> 版本:
>
> C/C++:0.1.4
>
> Go:NULL
>
> 

# 如何测试(how to test?)

**编译与测试**

`cmake .`

`make`

`./Syinx  run`

`nc   你的IP地址   8855`

**命令参数**

`./Syinx  run`             启动并运行框架

`./Syinx -s`                 显示当前每个工作线程的连接数与状态

`./Syinx -v  `                  显示当前Syinx版本

`./Syinx -c/close`     关闭当前框架并释放资源

  

# 依赖库(Dependent libraries)

**libevent :安装与访问**

`sudo apt-get install libevent`
`https://github.com/libevent/libevent`

**pthread:安装与访问**

`sudo apt-get install glibc-doc`
`sudo apt-get install manpages-posix-dev`
`https://computing.llnl.gov/tutorials/pthreads/`



# 概述(overview)

![1570001828106](https://github.com/OnlyloveSY/Syinx/blob/master/screenshots/b1b193957bd55749b99d03b77b8f8f7f.png)

​        通过配置文件读取用户设置的服务器基础参数以及当前主要工作线程数量来进行初始化，并为框架设定好回调函数，以4主要线程为例，当有客户端连接服务器时，触发listener回调函数，返回客户端的文件描述符（Socketfd），此时委托resource adapter类的初始化客户端的bufferevent读事件回调函数，写事件回调函数，错误事件回调函数，并且对后续业务设定一个ichannel通道层，此时Socketfd会被添加到resource adapter的map容器，并且在此之前会根据不同的要求添加到不同的base事件树上

​		框架初始化时就已经为每一个线程的工作函数设定为遍历base事件树，线程只独享自己的base，之间并不共享，当有可读事件触发时，事件树返回触发事件的bufferevent，并且调用相应的回调函数，将数据保存到通道层，根据不同的游戏业务，客户可以对通道层做后续开发。

​		游戏业务流程建议为 -->IO框架层   ->  通道层  ->协议层  ->  业务层  ->  逻辑处理层 (可有可无)		

​		框架遵循ASN.1协议格式:

​		发送接收数据保存的格式:TLV格式

![1570166126792](https://github.com/OnlyloveSY/Syinx/blob/master/screenshots/2dc6b61a242fae4d85c08743314afc98.png)



# 帮助(Help)

```c++
#include "Syinx.h"                //包含框架核心头文件
#include "SyTaskAdapter.h"        //需要包含任务流程处理头文件
```



```c++
int main()
{
	SyinxKernelWork a(8855);    //指定框架绑定的端口号(默认为8855),框架会默认绑定 0.0.0.0 IP地址
	
	return a.SyinxClose();		
}
```



```c++

//用户可以指定该构造函数需要初始化或者用来创建哪些东西  当有新的客户端连接时会调用该默认构造
IChannel::IChannel()
{
}
//用户可以指定该析构构造函数需要释放哪些东西  当客户端退出时会调用该析构
IChannel::~IChannel()
{
}
/*
@  -用户需要在该初始化函数,作为客户端的初始化工作  可写可无
@调用时机:主动  每当客户端连接服务器的时都会默认调用该函数
*/
int IChannel::IChannelTaskInit()
{

}
/*
@  -用户需要在该任务流程处理函数内写入你所后续规划的其他函数
@调用时机:被动 每当客户端发来数据时都会调用该函数
*/
int IChannel::IChannelTaskProcessing()
{

	
}
```



数据接收以及发送函数

```c++
/*
	@   -读取当前全部数据数据到string
	@arg:不以任何协议格式将全部数据读取string并清空占存区
	@成功返回读取到的数据长度,失败返回-1 
	*/
	int RecvAllDataToString(std::string &arg);

	/*
	@   -以协议的格式接收数据到string   len  type  values
	@arg:以任何协议格式将values的数据读取string并清空占存区
	@OutLen:成功读取到的数据的长度
	@Type:成功读取到的数据类型
	@str:成功读取到的数据
	@成功将返回1且此时说明仍然有数据可读
	@返回0时说明无剩余数据可读
	@失败返回-1
	*/
	int RecvValuesToString(unsigned int* _OutLen, unsigned int* _OutType, std::string& _OutStr);


	/*
	@   -直接发送当前string到该客户端
	@instr:直接将当前字符串发送到当前客户端
	@成功返回发送的数据长度,失败返回-1
	*/
	int SendAllDataToString(std::string &_InStr);

	/*
	@   -按协议格式转换正Asn.1的string数据包并发送 ( len  type  values)
	@Inlen:发送数据包的长度
	@InType:发送数据报的类型(自行指定可有可无)
	@InStr:需要发送的数据报
	@成功返回发送数据包的长度,失败返回-1
	*/
	int SendValuesToString(unsigned int _InLen, unsigned int _InType, std::string& _InStr);
```



# 历史版本日志(Revisions logs)

待dev版本开发完毕后上传至github

Syzinx -dev 0.1.1

​	--当前代码较为臃肿bug较多,多线程问并没解决,类之间分工并不明确

​	--线程类,资源适配类,任务类待完善其他核心模块不变Syzinx -dev 0.1.1



Syzinx -dev 0.1.2

​	--线程类,救援适配类,任务类进行划分

​	--修改头文件包含问题

​	--优化线程池,线程池分为主线程和次线程

当前版本bug:I/O主线程没有事件时会退出



Syzinx -dev 0.1.3

​		--管理线程(未完成)

​		--读写配置文件(未完成)

​		--多线程基于数据库安全问题(未完成)

​		--错误日志输出(未完成)



Syzinx -dev 0.1.4

​		  新增CMake文件

​		  新增命令参数输入(详细见上面文档)

​		  框架现为守护进程,不再是主进程并占用当前控制台

​		--读写配置文件(未完成)

​		--多线程基于数据库安全问题(未完成)

​		--错误日志输出(未完成)