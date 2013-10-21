#include "StdAfx.h"
#include "KLastError.h"
#include "UpdateDefine.h"

int KLastError::m_nErrorCode = UPDATE_ERR_OK;

KLastError::KLastError(void)
{
}

KLastError::~KLastError(void)
{
}

int KLastError::GetErrorCode()
{
	return m_nErrorCode;
}

void KLastError::SetErrorCode(int nErrorCode)
{
	if (m_nErrorCode == UPDATE_ERR_OK)
		m_nErrorCode = nErrorCode;
}