#ifndef _SYCONFIG_H_
#define _SYCONFIG_H_ 
class SyinxLog;
struct SyinxConfMsg {

	std::string Host;                //ip
	std::string Port;                //端口
	int    PthNum;             //线程池容量
	int    TaskNum;             //设置任务队列数量


	int    Timerinterval;       //设置计时器周期时间
	int    Timervalue;          //计时器第一次延迟事件

};
class SyinxConfig {
public:
	SyinxConfig();
	~SyinxConfig();

public:
	//读取配置文件
	SyinxConfMsg* Read_Msgconfig();

};

#endif
