#include "stdafx.h"
#include "./tblock.h"

KBlockPool* KBlockPool::ms_pSelf = NULL;

BOOL KBlockPool::Init()
{
	ASSERT(ms_pSelf == NULL);

	ms_pSelf  = new(std::nothrow) KBlockPool;
	KGLOG_PROCESS_ERROR(ms_pSelf);

	return TRUE;
Exit0:
	return FALSE;
}

void KBlockPool::Exit()
{
	SAFE_DELETE(ms_pSelf);
}

KBlockPool& KBlockPool::GetSelf()
{
	ASSERT(ms_pSelf);
	return *ms_pSelf;
}

KBlockPool::KBlockPool()
{
	m_Pool32.Init(32, 128);
	m_Pool64.Init(64, 64);
	m_Pool128.Init(128, 32);
	m_Pool256.Init(256, 16);
}

KBlockPool::~KBlockPool()
{
	Purge();
}

void* KBlockPool::Alloc(size_t nSize)
{
	if (nSize > 256)
		return malloc(nSize);

	if (nSize > 128)
		return m_Pool256.Alloc();

	if (nSize > 64)
		return m_Pool128.Alloc();

	if (nSize > 32)
		return m_Pool64.Alloc();

	return m_Pool32.Alloc();
}

void KBlockPool::Free(void* pData, size_t nSize)
{
	if (nSize > 256)
	{
        free(pData);
		return;
	}

	if (nSize > 128)
	{
		m_Pool256.Free(pData);
		return;
	}
	if (nSize > 64)
	{
		m_Pool128.Free(pData);
		return;
	}
	if (nSize > 32)
	{
		m_Pool64.Free(pData);
		return;
	}

	m_Pool32.Free(pData);
	return;
}

void KBlockPool::Purge()
{
	m_Pool32.Purge();
	m_Pool64.Purge();
	m_Pool128.Purge();
	m_Pool256.Purge();
}
