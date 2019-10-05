#include "SyZinx.h"
//
//#include <cppconn/driver.h>
#include <iostream>
using namespace std;

int main()
{
	
	/*SyZinxConfig config;
	config.Make_Msgconfig();*/
	SyZinxKernel::SyZinxKernel_Init(8855);
	SyZinxKernel::mSyZinx->mSyPth->SyZinxAdapter_Pth_Add();
	cout << SyZinxKernel::mSyZinx->mSyPth << endl;
	SyZinxKernel::SyZinxKernel_Run();

	
	SyZinxKernel::SyZinxKernel_Close();

	

	getchar();
}
