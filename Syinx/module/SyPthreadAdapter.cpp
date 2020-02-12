#include "../Syinx/SyInc.h"
#include "SyPthreadPool.h"
#include "../Syinx/Syinx.h"
#include "../Syinx/SyResAdapter.h"
#include "SyPthreadPool.h"
#include "SyPthreadAdapter.h"
SyinxAdapterPth::SyinxAdapterPth(uint32_t PthPoolNum, uint32_t TaskMaxNum) :m_PthPoolNum(PthPoolNum), m_TaskMaxNum(TaskMaxNum)
{
	m_sSyinxPthPool = nullptr;
}

SyinxAdapterPth::~SyinxAdapterPth()
{
	m_PthPoolNum = 0;
	m_TaskMaxNum = 0;

	m_sSyinxPthPool = nullptr;
}


bool SyinxAdapterPth::SyinxAdapterPth_Init()
{
	m_sSyinxPthPool = g_SyinxPthPool.threadpool_create(m_PthPoolNum, m_TaskMaxNum);
	if (m_sSyinxPthPool == nullptr)
	{
		return false;
	}
	return true;
}

uint32_t SyinxAdapterPth::SyinxAdapterPth_Add(void* (*taskfunc)(void*), void* arg)
{
	return g_SyinxPthPool.threadpool_add(g_SyinxPthPool.GetPthreadPool(), taskfunc, arg);
}



uint32_t SyinxAdapterPth::SyinxAdapterPth_destroy()
{
	return g_SyinxPthPool.threadpool_destroy(g_SyinxPthPool.GetPthreadPool());
}
