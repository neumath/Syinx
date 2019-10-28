#ifndef _SYMYSQLCONPOOL_H_
#define _SYMYSQLCONPOOL_H_
#include <list>
#include <iostream>
#include <pthread.h>
#include <mysql/mysql.h>
struct  MysqlConPool_t
{
	pthread_mutex_t    mutex;             /*用于锁住本结构体*/

	std::list <MYSQL*> MysqlPoolList;

	int MysqlMaxnum;                      /*mysql数据库连接池最大数量*/

	int count;                            /*当前连接池还剩多少个连接句柄*/


	std::string  _HOST_;
	std::string  _USER_;
	std::string  _PASSWD_;
	std::string  _DATABASE_;
};
enum SyMysqlConPoolErrno
{
	//init
	MysqlSuccess = 1,
	MysqlInitErr,
	MysqlConErr,

	MysqlMutexInitErr,
	MysqlMutexdesErr,

	MysqlLockErr,
	MysqlUnLockErr,


};
class SyMysqlConPool
{
	friend class SyinxKernel;
public:
	SyMysqlConPool() {}
	~SyMysqlConPool() {}


	/*
	初始化Mysql连接池,线程池结构体为单例模式,只能被初始化一次
	num:  初始化连接池数量
	HOST: Mysql远端ip
	User: 远端用户名
	Passwd: 密码
	database: 数据库名
	*/
	int SyMysqlConPool_Init(int num, std::string _HOST_, std::string _USER_, std::string _PASSWD_, std::string _DATABASE_);

	/*
	获取一个连接
	失败返回NULL
	*/
	static MYSQL* GetMysqlHandle();

	/*
	放回一个连接
	*/
	static int PutMysqlHandle(MYSQL*);

private:

	bool Work = false;
	bool shutdown = false;

	//释放线程池
	int SyMysqlConPool_destroy();

	MYSQL* GetMysql();

	int PutMysql(MYSQL*);

	//单例模式
	static SyMysqlConPool* Mysqlpool;

	MysqlConPool_t* mysqlpool_t;
};


#endif
