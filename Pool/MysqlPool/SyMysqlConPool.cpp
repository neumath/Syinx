#include "../Syinx/Syinx.h"
#include "SyMysqlConPool.h"
#include "../Sylog/SyLog.h"
using namespace std;

SyMysqlConPool* SyMysqlConPool::Mysqlpool = nullptr;
MYSQL* SyMysqlConPool::GetMysqlHandle()
{
	return SyMysqlConPool::Mysqlpool->GetMysql();
}

int SyMysqlConPool::PutMysqlHandle(MYSQL* mysql_t)
{
	return SyMysqlConPool::Mysqlpool->PutMysql(mysql_t);
}

int SyMysqlConPool::SyMysqlConPool_Init(int connum, string _HOST_, string _USER_, string _PASSWD_, string _DATABASE_)
{
	if (this->Work)
	{
		return MysqlInitErr;
	}
	SyMysqlConPool::Mysqlpool = new SyMysqlConPool;
	Mysqlpool->mysqlpool_t = new MysqlConPool_t;

	//初始化结构体信息
	Mysqlpool->mysqlpool_t->_HOST_ = _HOST_;
	Mysqlpool->mysqlpool_t->_USER_ = _USER_;
	Mysqlpool->mysqlpool_t->_PASSWD_ = _PASSWD_;
	Mysqlpool->mysqlpool_t->_DATABASE_ = _DATABASE_;
	Mysqlpool->mysqlpool_t->count = 0;

	/*cout << connum << endl;
	cout << "[" << _HOST_.c_str() << "]" << endl;
	cout << "[" << _USER_.c_str() << "]" << endl;
	cout << "[" << _PASSWD_.c_str() << "]" << endl;
	cout << "[" << _DATABASE_.c_str() << "]" << endl;*/

	//初始化锁
	if (pthread_mutex_init(&Mysqlpool->mysqlpool_t->mutex, NULL) != 0)
	{
		return MysqlMutexInitErr;
	}

	//初始化连接池
	//一个初始化失败或者连接失败全部回滚
	for (int i = 0; i < connum; ++i)
	{
		auto Mysql_t = mysql_init(NULL);
		if (Mysql_t == NULL)
		{
			return MysqlInitErr;
		}
		Mysql_t = mysql_real_connect(Mysql_t, _HOST_.c_str(), _USER_.c_str(), _PASSWD_.c_str(), _DATABASE_.c_str(), 0, NULL, 0);
		if (Mysql_t == NULL)
		{
			return MysqlConErr;
		}
		char WriteLog[64] = { 0 };
		sprintf(WriteLog, "The [%d]th attempt to connect was successful", i);
		SyinxLog::mLog.Log(__FILE__, __LINE__, SyinxLog::INFO, 0, WriteLog);
		Mysqlpool->mysqlpool_t->MysqlPoolList.push_back(Mysql_t);
		Mysqlpool->mysqlpool_t->MysqlMaxnum += 1;
		Mysqlpool->mysqlpool_t->count += 1;
	}
	this->Work = true;
	return MysqlSuccess;
}

int SyMysqlConPool::SyMysqlConPool_destroy()
{

	//释放锁
	if (pthread_mutex_destroy(&this->mysqlpool_t->mutex) != 0)
	{
		return MysqlMutexdesErr;
	}

	//关闭连接
	for (auto _it : this->mysqlpool_t->MysqlPoolList)
	{
		mysql_close(_it);
	}

	delete mysqlpool_t;
	return MysqlSuccess;
}

MYSQL* SyMysqlConPool::GetMysql()
{
	//锁住本结构体
	if (pthread_mutex_lock(&mysqlpool_t->mutex) != 0)
	{
		return NULL;
	}
	if (mysqlpool_t->count == 0 || mysqlpool_t->MysqlPoolList.size() == 0)
	{
		pthread_mutex_unlock(&mysqlpool_t->mutex);
		return NULL;
	}
	//获取末尾元素
	auto _itlist = mysqlpool_t->MysqlPoolList.back();
	mysqlpool_t->count -= 1;

	//删除末尾元素
	mysqlpool_t->MysqlPoolList.pop_back();


	if (pthread_mutex_unlock(&mysqlpool_t->mutex) != 0)
	{
		return NULL;
	}
	return _itlist;

}

int SyMysqlConPool::PutMysql(MYSQL* _in)
{
	//锁住本结构体
	if (pthread_mutex_lock(&mysqlpool_t->mutex) != 0)
	{
		return MysqlLockErr;
	}
	//头部插入一个mysql
	mysqlpool_t->MysqlPoolList.push_front(_in);
	mysqlpool_t->count += 1;

	if (pthread_mutex_unlock(&mysqlpool_t->mutex) != 0)
	{
		return MysqlUnLockErr;
	}
	return MysqlSuccess;
}
