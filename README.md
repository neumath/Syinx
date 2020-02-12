

![miku](https://github.com/OnlyloveSY/ResPic/blob/master/screenshots/miku.jpeg)

![Syinx](https://github.com/OnlyloveSY/ResPic/blob/master/screenshots/Syinx.png)

------

[![Syinx](https://github.com/OnlyloveSY/ResPic/blob/master/screenshots/goto.svg)](https://www.jianshu.com/u/3f6b82b7329d)

[![Syinx](https://github.com/OnlyloveSY/ResPic/blob/master/screenshots/bhu.svg)](https://www.zhihu.com/people/lee-78-27-78/activities)

基于libevent的跨平台轻量级高并发适用于游戏业务的服务器,利用任务队列的线程池模式以及事件队列来对业务进行高并发处理

Libevent-based lightweight high concurrency is applicable to the game business server, using the task queue thread pool pattern and event queue to carry out high concurrency processing of the business

> 版本:
>
> C/C++:0.5.0
>
> 适用平台:WIndows,Linux
>
> 开发: SiCen Li

# 前言

​		自己一个人写一个小的游戏框架实在是难,自己测试,自己解决bug,以及新的概念引用学习等问题都会导致开发时间被拉得过于长,我读过一些开源的服务器框架,他们写的都非常棒,比如Zinx C/C++版本,以及Zinx Golang版本,我觉得框架就应该是学习起来简单,使用者调用一些函数就可以完成整个基本功能,而不是基于某个别人写好的开发,那就成了二次开发,或者是自己写的库了

​		我也想把服务器写的跟框架一样,简单,通过函数调用完成大量功能,就比如Golang语言(正在初学中),虽然他的语法我觉得比较怪异,作为C/C++工程师来说就觉得这完全跟我们C/C++反着来,但是其实非常简单,我感觉他的package包真的强大,通过函数调用完成的大量功能,比如

```go
 con,err :=net.Listen("tcp", "127.0.0.1:8855")
 con.Accept()
```

可以省略C/C++几乎50行初始化工作的函数(六部曲 sockaddr_int 初始化 -> socket -> setsockopt->bind ->listen ->accept )

我这不是在打广告,只是说比较简单的调用而已,反正作为个人开发尽量将Syinx写个更强大

# 如何测试(how to test?)

该框架无需安装,可以直接在代码中编辑

编译测试,编译全部代码需要用到 Libevent pthread mysqlclient 第三方库

Linux:

`cd Syinx/`

`cmake .`

`make`

Windows:

`创建Windows C++ WIN32项目`

`直接将代码拷贝到项目中,并包含代码`

`下载对应的libevent  pthread的Windows版编译好的成lib文件,并添到依赖中`



Windows中需要提前添加的预处理宏

`WIN32`														添加该宏确保在Windows环境下运行

`LIBEVENT_IOCP`										使用Windows下的IOCP网络模型

`POSIX_PTHREAD`										使用posix标准的pthread库

`HAVE_STRUCT_TIMESPEC`				 		 在Windows环境中使用户pthread库需要包含该宏,防止timespe的MSVC编译器的重定义报错

`_CRT_SECURE_NO_WARNINGS`					在Windows环境中使用sprintf等字符串处理函数防止报错

`_WINSOCK_DEPRECATED_NO_WARNINGS`    在Windows中使用windows提供的网络库函数包含该宏防止出现重定义报错,或者出现未定义(未添加Ws2_32.lib依赖库也会出现这个问题)



找到bin目录的Clickme.sh脚本文件

`./Clickme.sh`             测试可能需要的动态库文件,成功生成一个main.cpp文件

`cmake .`                       执行cmake生成make文件

`make`                             执行make进行编译,如果没有执行第一步请回滚重新开始

该框架无需安装

**命令参数**

`./Syinx  run`             启动并运行框架

`./Syinx -s`                 显示当前服务器的连接数

`./Syinx -v  `                  显示当前Syinx版本

`./Syinx -c/close`     关闭当前框架并释放资源

  如果乱码请用VS打开

# 依赖库(Dependent libraries)

**libevent :安装与访问**

`sudo apt-get install libevent`

`https://github.com/libevent/libevent`

**pthread:安装与访问**

`sudo apt-get install glibc-doc`

`sudo apt-get install manpages-posix-dev`

`https://computing.llnl.gov/tutorials/pthreads/`



框架使用了第三方HerdOnly库

感谢作者的优秀的工具,方便他人开发

easyloggingpp:

`https://github.com/amrayn/easyloggingpp`

rapidjson:

`https://github.com/Tencent/rapidjson`



# 概述(overview)

![1570001828106](https://github.com/OnlyloveSY/ResPic/blob/master/screenshots/Syinx2.png)

###     一: 简介

   常见的适用于游戏的服务器,采用libevent作为开发源, 采用线程池任务队列进行操作,客户端连接服务器后,文件描述被epoll所监听,当有IO事件时,才会将事件函数加入到任务队列,然后被线程所调用



### 	二: 事件队列概念(事件轮询)

​	![1570001828106](https://github.com/OnlyloveSY/ResPic/blob/master/screenshots/Event_queue.png)

​	时间触发的事件任务以及周期触发事件均可以被设置,这也是大部分网络游戏在使用的一个固定频率任务处理的游戏事件轮询机制

​	每一个轮子即节点, 都会绑定一个queue或者是list, 你可以将某个你想要指定时间触发,或者是固定时间周期触发的任务挂在某个节点的queue或者list上,每次轮询时候都会来检测并且做相应处理,该完成什么样的任务将由你来决定	

​	在Syinx中可以创建一个固定周期(sec+msec为一周期)运行的时间驱动轮秒与毫秒可以自行设定,且时间驱动轮不为单例模式,可以初始化多次

### 	三: 常见的游戏数据流协议

​		游戏业务流程建议为 -->IO框架层   ->  通道层  ->协议层  ->  业务层  ->  逻辑处理层 (可有可无)		

​		框架遵循ASN.1协议格式:

​		发送接收数据保存的格式:TLV格式

![1570166126792](https://github.com/OnlyloveSY/ResPic/blob/master/screenshots/2dc6b61a242fae4d85c08743314afc98.png)



### 	 四: 如何修改配置文件

配置文件根据`Syinx.h`头文件中的`GAME_CONFIG_PATH`宏确定你的配置文件位置,配置文件使用`rapidjson`第三方库进行配置

在`Syinx.h`中的`SyinxKernelReadconfig`函数中配置Json返序列化的初始化配置

# 帮助(Help)

```c++
#include "Syinx.h"                //包含框架核心头文件
#include "SyTaskAdapter.h"        //需要包含任务流程处理头文件
```

main.cpp函数已经确定,无需修改,只需修改`el::Configurations conf`中的配置文件路径即可

```c++
#include <stdio.h>
#include <iostream>
using namespace std;
#include "./Syinx/Syinx.h"
#include "./Sylog/easylogging++.h"
#define ELPP_THREAD_SAFE
INITIALIZE_EASYLOGGINGPP;
int main()
{
	// Load configuration from file
	el::Configurations conf("log功能配置文件路径");
#ifdef WIN32
	conf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
#endif
	// Actually reconfigure all loggers instead
	el::Loggers::reconfigureAllLoggers(conf);

	if (!g_pSyinx.Initialize())
		return -1;

	g_pSyinx.SyinxKernel_Run();

	g_pSyinx.SyinxKernel_Close();
}
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



Syzinx -dev 0.1.5

​			修复线程当前状态查看`./Syinx -s`命令可查看

​			修复计时器,每个线程不在无限调用计时器timerfd的回调函数,而是修改为第一次开始1s,周期1s

​			--后续将为计时器添加消息队列以及心跳包

​			--读写配置文件(未完成)

​			--多线程基于数据库安全问题(未完成)

​			--错误日志输出(未完成)



Syzinx -dev 0.1.8   该版本已经能够被使用或者用来学习

​			大量的代码被重置,删除了不必要的元素

​    		不在为服务器分成多个epoll树

​			添加线程池

​			添加log文件,一切的log日志信息在log文件夹下查看`./log/Syderlog.log`

​			添加读写配置文件

​			--后续将为计时器事件队列

Syzinx -dev 0.2.0

​			基本功能均完成

​			修复log文件写入问题

​			修复数据库连接问题

​			线程池,数据库,连接池,事件计时器均可以单独拿出来使用



Syzinx -dev 0.5.0 该版本修改较为庞大,几乎摈弃了之前很多的无用功能,代码的稳定性更加高,现在此版本正在被待开中的游戏所使用

​			根据之前的0.2.0修复大量bug

​			新增状态机模式

​			新增Reactor响应模式

​			新增跨平台,现在可以再Windows以及linux中编译

​			

​		





