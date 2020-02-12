#ifndef _SYMYSQLCONPOOL_H_
#define _SYMYSQLCONPOOL_H_

class SyMysqlConPool
{
private:
	pthread_mutex_t    m_mutex;					/* 用于锁住本结构体 */

	std::queue<MYSQL*>	m_queueMysql;

	uint32_t m_MysqlMaxnum;                      /* mysql数据库连接池最大数量 */

	uint32_t m_Count;                            /*	当前连接池还剩多少个连接句柄	*/
	uint32_t m_Capacity;
	std::string  m_strHost;
	std::string  m_strUser;
	std::string  m_strPasswd;
	std::string  m_strDatabase;
public:
	SyMysqlConPool();
	~SyMysqlConPool();
	static SyMysqlConPool& MakeSingleton();

	// sql handle tool
	bool Initialize(int num, std::string _HOST_, std::string _USER_, std::string _PASSWD_, std::string _DATABASE_);
	 MYSQL* GetMysqlHandle();
	 bool PutMysqlHandle(MYSQL*);
	 void close();


	 // Get sql query tool
	 bool MysqlCarryOutQuery(MYSQL* mysql, const char* sql, bool IsSelect= false);
	 void GetInt(uint32_t row, uint32_t col, int* out);
	 void GetString(uint32_t row, uint32_t col, string* out);
	 void GetFloat(uint32_t row, uint32_t col, float* out);
	 void GetDouble(uint32_t row, uint32_t col, double* out);
	 int  GetRows();
	 int  GetCols();
	 void Clear();

private:
	vector<vector<std::string>> m_vecReslut;

};

extern SyMysqlConPool& g_pMysqlPool;


#endif
