#include"SyZinx.h"
#ifdef _USE_DATABASE_

SyZinxDatabase::SyZinxDatabase()
{
}

SyZinxDatabase::~SyZinxDatabase()
{
}


int SyZinxDatabase::SyZinxKernel_MysqlInit()
{
	this->mSyZinxSql = mysql_init(NULL);
	return 1;
}

int SyZinxDatabase::SyZinxKernel_ConnetMysql(char* _SetiCharacter)
{
	this->mSyZinxSql = mysql_real_connect(this->mSyZinxSql, _HOST_, _USER_, _PASSWD_, _DATABASE_, 0, NULL, 0);
	if (NULL == this->mSyZinxSql)
	{
		return -1;
	}
	this->SyZinxKernel_MysqlSetCharacter(_SetiCharacter);
	return 1;
}

int SyZinxDatabase::SyZinxKernel_ConnetMysql(char* Host, char* User, char* Passwd, char* Database, char* _SetiCharacter)
{
	this->mSyZinxSql = mysql_real_connect(this->mSyZinxSql, Host, User, Passwd, Database, 0, NULL, 0);
	if (NULL == this->mSyZinxSql)
	{
		return -1;
	}
	memcpy(this->Host, Host, strlen(Host));
	memcpy(this->User, User, strlen(User));
	memcpy(this->Passwd, Passwd, strlen(Passwd));
	memcpy(this->Database, Database, strlen(Database));
	this->SyZinxKernel_MysqlSetCharacter(_SetiCharacter);
	return 1;

}

int SyZinxDatabase::SyZinxKernel_CloseMysql()
{
	mysql_close(this->mSyZinxSql);
	return 0;
}

int SyZinxDatabase::SyZinxKernel_MysqlSetCharacter(char* _iCharacter)
{
	mysql_set_character_set(this->mSyZinxSql, _iCharacter);
	return 0;
}
#endif 