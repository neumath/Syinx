#include "SyZinx.h"

#include <iostream>

using namespace std;

int main()
{
#ifdef _USE_DATABASE_
	SyZinxDatabase *database = new SyZinxDatabase;
	database->SyZinxKernel_MysqlInit();
	database->SyZinxKernel_ConnetMysql("utf8");
#endif

	SyZinxKernel::SyZinxKernel_Init(5555);

	
	SyZinxKernel::SyZinxKernel_Run();


	SyZinxKernel::SyZinxKernel_Close();
	return 0;
}