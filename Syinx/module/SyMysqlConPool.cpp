#include "../Syinx/SyInc.h"
#include "../Syinx/Syinx.h"
#include "../Sylog/easylogging++.h"
#include "SyMysqlConPool.h"
#include "../Sylog/SyLog.h"
using namespace std;

SyMysqlConPool& g_pMysqlPool = SyMysqlConPool::MakeSingleton();


SyMysqlConPool::SyMysqlConPool()
{
	m_MysqlMaxnum = 0;
	m_Count = 0;
	m_Capacity = 0;
	m_vecReslut.clear();
}

SyMysqlConPool::~SyMysqlConPool()
{
	m_MysqlMaxnum = 0;
	m_Count = 0;
	m_Capacity = 0;
	m_vecReslut.clear();
}

SyMysqlConPool& SyMysqlConPool::MakeSingleton()
{
	// TODO: 在此处插入 return 语句
	static SyMysqlConPool mysql;
	return mysql;
}

bool SyMysqlConPool::Initialize(int connum, string _HOST_, string _USER_, string _PASSWD_, string _DATABASE_)
{

	//初始化结构体信息
	m_strHost = _HOST_;
	m_strUser = _USER_;
	m_strPasswd = _PASSWD_;
	m_strDatabase = _DATABASE_;
	m_Capacity = connum;
	//初始化锁
	if (pthread_mutex_init(&m_mutex, NULL) != 0)
	{
		return false;
	}

	//初始化连接池
	//一个初始化失败或者连接失败全部回滚
	for (int i = 0; i < connum; ++i)
	{
		auto Mysql_t = mysql_init(NULL);
		if (Mysql_t == NULL)
		{
			return false;
		}
		if (mysql_real_connect(Mysql_t, _HOST_.c_str(), _USER_.c_str(), _PASSWD_.c_str(), _DATABASE_.c_str(), 3306, NULL, 0)!=nullptr)
		{
			m_queueMysql.push(Mysql_t);
		}
		else
		{
			int i = mysql_errno(Mysql_t);
			const char* s = mysql_error(Mysql_t);
			LOG(ERROR) << "ERROR : " << s << " ERRON: " << i;
		}
		
	}
	if (m_queueMysql.size() == connum)
	{
		LOG(INFO) << m_queueMysql.size() << " Connect Mysql:[" << _HOST_.c_str() << "] Success";
		return true;
	}
	else
	{
		LOG(INFO) << m_queueMysql.size() << " Connect Mysql:[" << _HOST_.c_str() << "] Failed";
		return false;
	}
}

MYSQL* SyMysqlConPool::GetMysqlHandle()
{
	if (m_queueMysql.empty())
	{
		return nullptr;
	}
	auto Mysql = m_queueMysql.front();
	m_queueMysql.pop();
	return Mysql;
}

bool SyMysqlConPool::PutMysqlHandle(MYSQL*Mysql)
{
	if (Mysql == nullptr)
		return false;

	m_queueMysql.push(Mysql);
	return true;
}

void SyMysqlConPool::close()
{
	auto MysqlSize = m_queueMysql.size();
	for (int Index = 0; Index < MysqlSize; ++Index)
	{
		auto Mysql = m_queueMysql.front();
		if (Mysql)
			mysql_close(Mysql);
		m_queueMysql.pop();
	}
}

bool SyMysqlConPool::MysqlCarryOutQuery(MYSQL* mysql, const char* sql, bool IsSelect)
{
	if (mysql == nullptr)
	{
		LOG(ERROR) << "Mysql Handle is null";
		return false;
	}
	auto ret = mysql_query(mysql, sql);
	if (ret)
	{
		LOG(ERROR) << mysql_error(mysql);
		return false;
	}
	if (IsSelect) {
		auto Reslut = mysql_store_result(mysql);
		if (Reslut == nullptr)
		{
			LOG(ERROR) << "mysql_store_result failed";
			return false;
		}
		long int Rows = 0;
		unsigned int Fields = 0;

		MYSQL_ROW rows = NULL;

		int Index = 0;
		Rows = mysql_num_rows(Reslut);
		Fields = mysql_num_fields(Reslut);

		while (true)
		{
			rows = mysql_fetch_row(Reslut);
			if (NULL == rows)
			{
				break;
			}
			vector<string> tarData;
			for (Index = 0; Index < Fields; ++Index)
			{
				if (rows[Index] != nullptr)
					tarData.push_back(rows[Index]);
				else
				{
					string str;
					tarData.push_back(str);
				}
			}
			m_vecReslut.push_back(tarData);
		}
	}
	return true;
}

void SyMysqlConPool::GetInt(uint32_t row, uint32_t col, int* out)
{
	if ((row > 0 && row <= GetRows()) && (col > 0 && col <= GetCols()))
	{
		*out = atoi(m_vecReslut[row-1][col-1].c_str());
		return;
	}
	*out = -1;
}

void SyMysqlConPool::GetString(uint32_t row, uint32_t col, string* out)
{
	if ((row > 0 && row <= GetRows()) && (col > 0 && col <= GetCols()))
	{
		*out = m_vecReslut[row - 1][col - 1];
	}
	
}

void SyMysqlConPool::GetFloat(uint32_t row, uint32_t col, float* out)
{
	if ((row > 0 && row <= GetRows()) && (col > 0 && col <= GetCols()))
	{
		*out = atof(m_vecReslut[row - 1][col - 1].c_str());
		return;
	}
	*out = -0.1f;
}

void SyMysqlConPool::GetDouble(uint32_t row, uint32_t col, double* out)
{
	
}

int SyMysqlConPool::GetRows()
{
	return m_vecReslut.size();
}

int SyMysqlConPool::GetCols()
{
	if (m_vecReslut.empty())
	{
		return 0;
	}
	return m_vecReslut.begin()->size();
}

void SyMysqlConPool::Clear()
{
	for (auto Iter : m_vecReslut)
	{
		Iter.clear();
	}
	m_vecReslut.clear();
}
