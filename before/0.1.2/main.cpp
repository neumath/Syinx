#include "Syinx.h"
//
//#include <cppconn/driver.h>
#include <iostream>
using namespace std;

int main()
{
	
	/*SyinxConfig config;
	config.Make_Msgconfig();*/
	SyinxKernel::SyinxKernel_Init(8855);

	cout << SyinxKernel::mSyinx->mSyPth << endl;
	SyinxKernel::SyinxKernel_Run();

	
	SyinxKernel::SyinxKernel_Close();

	

	getchar();
}
