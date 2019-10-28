# MysqlConPool

### 一: 简介

> 当前版本:0.1

数据库连接池,配合Syinx开发,也可以单独拿出来使用,整个的MysqlConpool连接池设计为单例模式,只能被初始化一次,调用共享的类static函数进行获取与放回(获取一个连接必须放回! 获取放回连接的mysql句柄API默认加锁,不想加锁可以再源码中移除)

### 二: 如何使用

```c++
/*
	初始化Mysql连接池,线程池结构体为单例模式,只能被初始化一次
	num:  初始化连接池数量
	HOST: Mysql远端ip
	User: 远端用户名
	Passwd: 密码
	database: 数据库名
	失败返回errno 成功返回success
	*/
	int SyMysqlConPool_Init(int num, std::string _HOST_, std::string _USER_, std::string _PASSWD_, std::string _DATABASE_);

	/*
	获取一个连接
	失败返回NULL
	*/
	static MYSQL* GetMysqlHandle();

	/*
	放回一个连接,失败返回errno
	*/
	static int PutMysqlHandle(MYSQL*);
```

### 三: 错误枚举

```c++
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
```



