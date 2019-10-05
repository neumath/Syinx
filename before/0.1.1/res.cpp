#include "Adapter.h"
#include "SyZinx.h"
SyZinxAdapterResource::SyZinxAdapterResource()
{
	
}

SyZinxAdapterResource::~SyZinxAdapterResource()
{
}


int SyZinxAdapterResource::SyZinxAdapterResource_Init(int _PthNum)
{
	
	if (_PthNum == 0)
	{
		mLog.Log(__FILE__, __LINE__, SyZinxLog::ERROR, -1, "getPthNum is 0 ");
	}
	this->mSyChannelVec.reserve(_PthNum);
	this->mSyBaseVec.reserve(_PthNum);
	

	for (size_t i = 0; i < _PthNum; i++)
	{
		this->mSyBaseVec[i] = event_base_new();
		std::cout << this->mSyBaseVec[i] <<i<< std::endl;	
	}

	return 0;
}

int SyZinxAdapterResource::getallClientNum()
{

}

int SyZinxAdapterResource::deleteClient()
{
	return 0;
}

