#pragma once
#include "SyDef.h"
class SyinxConfig {
public:
	SyinxConfig();
	~SyinxConfig();

public:
	//配置json文件
	void Make_Msgconfig();
	//读取json文件
	void Reader_Msgconfig();
private:
	//写入json文件
	void Writer_MSgconfig();
public:
	SyinxLog mLog;
};
