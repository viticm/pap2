#include "stdafx.h"
#include "KDllHolder.h"

KDllHolder::KDllHolder()
{
	m_hMod = NULL;
}

KDllHolder::KDllHolder(LPCSTR lpstrDllName)
{
	m_hMod = NULL;
	Init(lpstrDllName);
}

KDllHolder::~KDllHolder()
{
	UnInit();
}

BOOL KDllHolder::Init(LPCSTR lpstrDllName)
{
    int nResult = false;

	KGLOG_PROCESS_ERROR(lpstrDllName);

	UnInit();

	m_hMod = ::LoadLibraryA(lpstrDllName);
	KGLOG_PROCESS_ERROR(m_hMod);

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(KGLOG_ERR, "Load [%s]\n", lpstrDllName);
    }
	return nResult;
}

void KDllHolder::UnInit()
{
	if (m_hMod)
	{
		::FreeLibrary(m_hMod);
		m_hMod = NULL;
	}
}

HMODULE KDllHolder::GetDllHandle()
{
	return m_hMod;
}

