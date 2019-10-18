#include <fstream>
#include <istream>
#include <iostream>
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
	string line;
	SyinxConfMsg* sMsg = new SyinxConfMsg;


	while (!_fconf.eof())
	{
		getline(_fconf, line);
		if (line[0] == '#')
			continue;
		if (line[0] == ' ')
			continue;
		if (line.size() == 0)
			continue;
		string _Key = line.substr(0, line.find(':'));
		string _Value = line.substr(line.find(':') + 1, line.size() - line.find(':'));
		

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
	}

#ifdef CONFTEST
	cout << sMsg->Host << endl;
	cout << sMsg->Port << endl;
	cout << sMsg->TreeNum << endl;
	cout << sMsg->TreeCap << endl;
	cout << sMsg->loadbalancing << endl;
	cout << sMsg->Timerinterval << endl;
	cout << sMsg->Timervalue << endl;
#endif
	return sMsg;
}

