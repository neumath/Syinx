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


	//初始化线程池
	int SyMysqlConPool_Init(int num, std::string _HOST_, std::string _USER_, std::string _PASSWD_, std::string _DATABASE_);

	//单例
	//获取一个连接
	static MYSQL* GetMysqlHandle();

	//放回一个连接
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
