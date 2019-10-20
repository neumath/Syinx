#include <fstream>
#include <istream>
#include <iostream>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include "SyLog.h"
#include "SyConfig.h"
using namespace std;

SyinxConfig::SyinxConfig()
{
}
SyinxConfig::~SyinxConfig()
{
}
SyinxConfMsg* SyinxConfig::Read_Msgconfig()
{
	fstream _fconf;
	_fconf.open("./conf/Syinx-server.conf", ios::in);
	if (!_fconf.is_open())
	{
		cout << "conf is not find" << endl;
		return NULL;
	}

	mkdir("log", S_IRWXU | S_IRGRP | S_IROTH);
	if (access("./log/Syderlog.log", F_OK))
	{
		unlink("./log/Syderlog.log");
	}

	//配置文件写入日志
	string line;
	SyinxConfMsg* sMsg = new SyinxConfMsg;


	while (!_fconf.eof())
	{
		getline(_fconf, line);
		if (line[0] == '#')
			continue;
		if (line[0] == ' ')
			continue;
		if (line.size() == 0 || !(line.size() - 1))
			continue;

		string _Key;
		string _Value;

		_Key = line.substr(0, line.find(':'));

		_Value = line.substr(line.find('\"') + 1, line.rfind('\"') - line.find('\"') - 1);
		char WriteLog[64] = { 0 };
		sprintf(WriteLog, "======>%s:%s \n", _Key.c_str(), _Value.c_str());
		SyinxLog::mLog.Log(__FILE__, __LINE__, SyinxLog::INFO, 0, WriteLog);


		if (_Key == "Host")
			sMsg->Host = _Value;
		if (_Key == "Port")
			sMsg->Port = _Value;
		if (_Key == "SetPthNum")
			sMsg->PthNum = stoi(_Value);
		if (_Key == "SetTaskMax")
			sMsg->TaskNum = stoi(_Value);
		if (_Key == "SetTimeinterval")
			sMsg->Timerinterval = stoi(_Value);
		if (_Key == "SetTimevalue")
			sMsg->Timervalue = stoi(_Value);

		//sql
		if (_Key == "SetMysqlPoolNum")
			sMsg->MysqlPoolNum = stoi(_Value);
		if (_Key == "SetMysqlHost")
			sMsg->MysqlHost =  _Value ;
		if (_Key == "SetMysqlUser")
			sMsg->MysqlUser =  _Value ;
		if (_Key == "SetMysqlPasswd")
			sMsg->MysqlPasswd = _Value ;
		if (_Key == "SetMysqlDatabase")
			sMsg->MysqlDatabase =_Value ;
	}
	_fconf.close();

#ifdef CONFTEST
	cout << sMsg->Host << sMsg->Host.size() << endl;
	cout << sMsg->Port << endl;
	cout << sMsg->PthNum << endl;
	cout << sMsg->TaskNum << endl;

	cout << sMsg->Timerinterval << endl;
	cout << sMsg->Timervalue << endl;

	cout << sMsg->MysqlPoolNum << endl;
	cout << sMsg->MysqlHost.c_str() - 1 << endl;
	cout << sMsg->MysqlUser.size() << endl;
	cout << sMsg->MysqlPasswd.size() << endl;
	cout << sMsg->MysqlDatabase << endl;
#endif
	return sMsg;
}
