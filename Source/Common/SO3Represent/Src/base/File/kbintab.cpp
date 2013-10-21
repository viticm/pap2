#include "stdafx.h"
#include "./kbintab.h"

namespace
{
	struct KHeader
	{
		size_t nFourCC;
		size_t nDataSize;
		size_t nRow;
		size_t nColumn;
	};

	struct KField
	{
		union
		{
			int i;
			float f;
			char* s;
		};
	};

	int FileTimeStampCompare(LPCTSTR szLhs, LPCTSTR szRhs)
	{
		ASSERT(szLhs);
		ASSERT(szRhs);

		BOOL bExistLhs = g_IsFileExist(szLhs);
		BOOL bExistRhs = g_IsFileExist(szRhs);

		if (!bExistLhs && !bExistRhs)
			return 0;

		// TODO: 比较时间
		return 1;
	}

}

KBinTab::KBinTab()
: m_pData(NULL)
{
}

KBinTab::~KBinTab()
{
	SAFE_FREE(m_pData);
}

BOOL KBinTab::Load(LPCSTR szFilePath, LPCSTR szFormat)
{
    ASSERT(szFilePath);
    ASSERT(szFormat);
    ASSERT(sizeof(KField) == 4);

    size_t nFilePathLength = _tcslen(szFilePath);
    if (nFilePathLength < 4)
        return FALSE;

    if (stricmp(szFilePath + nFilePathLength - 4, ".txt") == 0 ||
        stricmp(szFilePath + nFilePathLength - 4, ".tab") == 0)
    {
        char szBinFilePath[MAX_PATH];

        ASSERT(nFilePathLength < MAX_PATH);

        strcpy(szBinFilePath, szFilePath);
        strcpy(szBinFilePath + nFilePathLength - 3, "kbt");

        if (FileTimeStampCompare(szFilePath, szBinFilePath))
            return LoadTxtTab(szFilePath, szFormat);
        else
            return LoadBinTab(szBinFilePath);
    }
    else
    {
        return LoadBinTab(szFilePath);
    }

    return TRUE;
}

BOOL KBinTab::Save(LPCSTR szFilePath)
{
	size_t nFilePathLength = _tcslen(szFilePath);
	if (nFilePathLength < 4)
		return FALSE;

	if (stricmp(szFilePath + nFilePathLength - 4, ".txt") == 0 ||
		stricmp(szFilePath + nFilePathLength - 4, ".tab") == 0)
	{
		return SaveTxtTab(szFilePath);
	}
	else
	{
		ASSERT(stricmp(szFilePath + nFilePathLength - 4, ".ktb") == 0);
		return SaveBinTab(szFilePath);
	}
}

void* KBinTab::Data(size_t& nRow)
{
	ASSERT(m_pData);

	KHeader* pHeader = (KHeader*)m_pData;
	KField* pField = (KField*)(m_pData + sizeof(KHeader));

	ASSERT(pHeader->nFourCC == ' TBK');
	ASSERT(pHeader->nRow > 0);

	nRow = pHeader->nRow;
	return pField + pHeader->nColumn;
}

BOOL KBinTab::ToStringOffset()
{
    BOOL bResult = FALSE;
	KHeader* pHeader = NULL;
	KField* pField = NULL;
	char* szText = NULL;

    KGLOG_PROCESS_ERROR(m_pData);

    pHeader = (KHeader*)m_pData;
	pField = (KField*)(m_pData + sizeof(KHeader));
	szText = (char*)(m_pData + sizeof(KHeader) + sizeof(KField) * (pHeader->nRow + 1) * pHeader->nColumn);

	for (size_t nRow = 0; nRow < pHeader->nRow; ++nRow)
	{
		for (size_t nColumn = 0; nColumn < pHeader->nColumn; ++nColumn)
		{
			KField& FieldType = pField[nColumn];
			KField& FieldCur = pField[nColumn + pHeader->nColumn * (nRow + 1)];

			if (FieldType.i == FIELD_FORMAT_STRING)
			{
                ASSERT(sizeof(char*) == sizeof(int));
				FieldCur.i = (int)(FieldCur.s - szText);
			}
		}
	}

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KBinTab::ToStringPointer()
{
    BOOL bResult = FALSE;
	KHeader* pHeader = NULL;
	KField* pField = NULL;
	char* szText = NULL;

    KGLOG_PROCESS_ERROR(m_pData);

    pHeader = (KHeader*)m_pData;
	pField = (KField*)(m_pData + sizeof(KHeader));
	szText = (char*)(m_pData + sizeof(KHeader) + sizeof(KField) * (pHeader->nRow + 1) * pHeader->nColumn);

	for (size_t nRow = 0; nRow < pHeader->nRow; ++nRow)
	{
		for (size_t nColumn = 0; nColumn < pHeader->nColumn; ++nColumn)
		{
			KField& FieldType = pField[nColumn];
			KField& FieldCur = pField[nColumn + pHeader->nColumn * (nRow + 1)];

			if (FieldType.i == FIELD_FORMAT_STRING)
			{
				FieldCur.s = FieldCur.i + szText;
			}
		}
	}

    bResult = TRUE;
Exit0:
    return bResult;
}

BOOL KBinTab::LoadBinTab(LPCSTR szFilePath)
{
    int nRetCode = false;
    BOOL bResult = FALSE;
	KHeader* pHeader = NULL;
	KField* pField = NULL;
	size_t nSize = 0;
	size_t nReaded = 0;
	KSmartFile pFile;
	PBYTE pData = NULL;

	KG_ASSERT_EXIT(szFilePath);

	pFile = g_OpenFile(szFilePath);
	KGLOG_PROCESS_ERROR(pFile);

	nSize = pFile->Size();

	pData = (PBYTE)realloc(m_pData, nSize);
	KGLOG_PROCESS_ERROR(pData);

	m_pData = pData;

	nReaded = (size_t)pFile->Read(m_pData, (unsigned long)nSize);
	KGLOG_PROCESS_ERROR(nReaded == nSize);

	pHeader = (KHeader*)m_pData;
	KG_ASSERT_EXIT(pHeader->nDataSize == nSize);

    nRetCode = ToStringPointer();
    KGLOG_PROCESS_ERROR(nRetCode);

	bResult = TRUE;
Exit0:
    if (!bResult)
    {
	    SAFE_FREE(m_pData);

        KGLogPrintf(KGLOG_ERR, "KBinTab::LoadBinTab(%s)\n", szFilePath);
    }
	return bResult;
}

BOOL KBinTab::SaveBinTab(LPCSTR szFilePath)
{
    int nRetCode = false;
	BOOL bResult = FALSE;
	KHeader* pHeader = NULL;
	KField* pField = NULL;
	char* szText = NULL;
	size_t nWritten = 0;
	IFile *piFile = NULL;
    size_t uDataSize = 0; 

	KG_ASSERT_EXIT(szFilePath);
	KG_ASSERT_EXIT(m_pData);

    nRetCode = g_IsFileExist(szFilePath);
    if (nRetCode)
    {
        piFile = g_OpenFile(szFilePath, FALSE, TRUE);
        KGLOG_PROCESS_ERROR(piFile);
    }
    else
    {
        piFile = g_CreateFile(szFilePath);
        KGLOG_PROCESS_ERROR(piFile);
    }

	pHeader = (KHeader*)m_pData;
    pField = (KField*)(m_pData + sizeof(KHeader));

    nRetCode = ToStringOffset();
    KGLOG_PROCESS_ERROR(nRetCode);
    
    uDataSize = pHeader->nDataSize - sizeof(KHeader) - pHeader->nColumn * sizeof(KField);
	nWritten = (size_t)piFile->Write(pField + pHeader->nColumn, (unsigned long)uDataSize);
	KGLOG_PROCESS_ERROR(nWritten == uDataSize);

	bResult = TRUE;
Exit0:
    SAFE_RELEASE(piFile);
	return bResult;
}

size_t KBinTab::GetTxtCellLength(char* szBuffer, size_t nSize, size_t& nDelimiter)
{
	for (size_t nOffset = 0; nOffset < nSize; ++nOffset) 
	{
		char c = szBuffer[nOffset];
		if (c == '\t')
		{
			szBuffer[nOffset] = '\0'; // NOTE: modified
			nDelimiter = 1;
			return nOffset + 1;
		}
		ASSERT(c != '\n');
		ASSERT(c != '\t');
	}
	nDelimiter = 0;
	return nSize;
}

size_t KBinTab::GetTxtRowLength(char* szBuffer, size_t nSize, size_t& nDelimiter)
{
	for (size_t nOffset = 0; nOffset < nSize; ++nOffset) 
	{
		char c = szBuffer[nOffset];
		if (c == '\n')
		{
			szBuffer[nOffset] = '\t'; // NOTE: modified
			nDelimiter = 1;
			return nOffset + 1;
		}
		if (c == '\r')
		{
			if (nOffset + 1 < nSize && szBuffer[nOffset + 1] == '\n')
			{
				szBuffer[nOffset] = '\t'; // NOTE: modified
				nDelimiter = 2;
				return nOffset + 2;
			} 
			else 
			{
				szBuffer[nOffset] = '\t'; // NOTE: modified
				nDelimiter = 1;
				return nOffset + 1;
			}
		}
	}
	nDelimiter = 0;
	return nSize;
}

size_t KBinTab::TripSlashes(char* szBuffer)
{
	//TODO:可能在解析某些中文字符时会出错!

	ASSERT(szBuffer);

	LPSTR szBegin = szBuffer;
	LPSTR szCurrent = NULL;

	szCurrent = szBuffer;
	while (true)
	{
		if (szBuffer[0] == '\\')
		{
			BOOL bTripped = TRUE;

			switch (szBuffer[1])
			{
			case 'n':
				szCurrent[0] = '\n';
				break;
			case 't':
				szCurrent[0] = '\t';
				break;
			case '\\':
				szCurrent[0] = '\\';
				break;
			case '\"':
				szCurrent[0] = '\"';
				break;
			default:
				bTripped = FALSE;
				break;
			}
			if (bTripped)
			{
				++szCurrent;
				szBuffer += 2;
				continue;
			}
		}
		if (szBuffer[0] == '\0')
			break;
		LPCSTR szNext = ::CharNextA(szBuffer);
		if (szNext == NULL)
			break;
		while (szNext != szBuffer)
		{
			*szCurrent++ = *szBuffer++;
		}
	}
	szCurrent[0] = '\0';

	return (size_t)(szCurrent - szBegin + 1);
}

BOOL KBinTab::LoadTxtCell(char* szBuffer, size_t nSize, size_t nColumn)
{
	KHeader* pHeader = (KHeader*)m_pData;
	KField* pFieldType = (KField*)(m_pData + sizeof(KHeader)); // type row
	KField*	pFieldCell = (KField*)(m_pData + sizeof(KHeader) + (pHeader->nColumn * (pHeader->nRow + 1) + nColumn) * sizeof(KField));

	if (nSize == 1 && pHeader->nRow != 0)
	{
		*pFieldCell = *(KField*)(m_pData + sizeof(KHeader) + (pHeader->nColumn + nColumn) * sizeof(KField)); // default row
	}
	else
	{
		switch (pFieldType[nColumn].i)
		{
		case FIELD_FORMAT_INT:
			pFieldCell->i = atoi(szBuffer);
			break;
		case FIELD_FORMAT_FLOAT:
			pFieldCell->f = (float)atof(szBuffer);
			break;
		case FIELD_FORMAT_STRING:
			if (nSize == 1)
			{
				pFieldCell->s = 0;
			}
			else
			{
				pFieldCell->s = strncpy((char*)m_pData + pHeader->nDataSize, szBuffer, nSize);
				pHeader->nDataSize += nSize;
			}
			break;
		case FIELD_FORMAT_SLASHES_STRING:
			if (nSize == 1)
			{
				pFieldCell->s = 0;
			}
			else
			{
				pFieldCell->s = strncpy((char*)m_pData + pHeader->nDataSize, szBuffer, nSize);
				pHeader->nDataSize += TripSlashes(pFieldCell->s);
			}
			break;
		default:
			KGLogPrintf(KGLOG_ERR, "SO3UI KBinTab::LoadTxtCell(%s, %u, %u) unsupport format %d\n", szBuffer, nSize, nColumn, pFieldType[nColumn].i);
			ASSERT(0);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL KBinTab::LoadTxtRow(char* szBuffer, size_t nSize)
{
	int nRetCode = false;
	size_t nColumn = 0;
	KHeader* pHeader = (KHeader*)m_pData;

	for (size_t nOffset = 0; nOffset < nSize;) {
		size_t nDelimiter = 0;
		size_t nRowLength = GetTxtCellLength(szBuffer + nOffset, nSize - nOffset, nDelimiter);

		nRetCode = LoadTxtCell(szBuffer + nOffset, nRowLength - nDelimiter + 1, nColumn++);
		KGLOG_PROCESS_ERROR(nRetCode);

		nOffset += nRowLength;
	}

	if (nColumn < pHeader->nColumn)
	{
		KField* pFieldDefault = (KField*)(m_pData + sizeof(KHeader) + (pHeader->nColumn + nColumn) * sizeof(KField));
		KField*	pFieldCell = (KField*)(m_pData + sizeof(KHeader) + (pHeader->nColumn * (pHeader->nRow + 1) + nColumn) * sizeof(KField));

		if (pHeader->nRow == 0)
		{
			memset(pFieldCell, 0, sizeof(KField) * (pHeader->nColumn - nColumn));
		}
		else
		{
			memcpy(pFieldCell, pFieldDefault, sizeof(KField) * (pHeader->nColumn - nColumn));
		}
	}

	++pHeader->nRow;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL KBinTab::LoadTxtTab(char* szBuffer, size_t nSize, LPCSTR szFormat) 
{
	int nRetCode = false;
    BOOL bResult = FALSE;
	KHeader* pHeader = NULL;
	KField* pField = NULL;
	size_t nRow = 0;
	size_t nColumn = 0;
	PBYTE pData = NULL;
	size_t nDelimiter = 0;
	char* szBlank = NULL;

	// calc table column
	for (size_t nOffset = 0; nOffset < nSize; ++nOffset) 
	{
		char c = szBuffer[nOffset];

		if (c == '\r' || c == '\n') 
		{
			++nColumn;
			break;
		}

		if (c == '\t')
		{
			++nColumn;
		}
	}

	KGLOG_PROCESS_ERROR(nColumn == strlen(szFormat));

	// calc row
	for (size_t nOffset = 0; nOffset < nSize;)
    {
		char c = szBuffer[nOffset];
		if (c == '\n' || c == '\0')
		{
			++nRow;
			++nOffset;
		} 
		else if (c == '\r')
		{
			if (nOffset + 1 < nSize && szBuffer[nOffset + 1] == '\n')
				nOffset += 2;
			else 
				++nOffset;

			++nRow;
		}
		else
		{
			++nOffset;
		}
	}
	if (nSize > 0)
	{
		char c = szBuffer[nSize - 1];
		if (c != '\n' && c != '\r' && c != '\0')
		{
			++nRow;
		}
	}

	pData = (PBYTE)realloc(m_pData, sizeof(KHeader) + nColumn * nRow * sizeof(KField) + nSize);
	KGLOG_PROCESS_ERROR(pData);

	m_pData = pData;
    pData = NULL;

	pHeader = (KHeader*)m_pData;
	pHeader->nFourCC = ' TBK';
	pHeader->nColumn = nColumn;
	pHeader->nRow = 0;	// NOTE: row 0 is format row
	pHeader->nDataSize = sizeof(KHeader) + nColumn * nRow * sizeof(KField);

	// row 0:
	pField = (KField*)(m_pData + sizeof(KHeader));
	for (size_t nIndex = 0; nIndex < nColumn; ++nIndex)
	{
		pField[nIndex].i = szFormat[nIndex];
	}

	// row >= 1:
	// strip row 0 of title
	for (size_t nOffset = GetTxtRowLength(szBuffer, nSize, nDelimiter); nOffset < nSize;)
    {
		size_t nRowLength = GetTxtRowLength(szBuffer + nOffset, nSize - nOffset, nDelimiter);

		nRetCode = LoadTxtRow(szBuffer + nOffset, nRowLength - nDelimiter + 1);
		KGLOG_PROCESS_ERROR(nRetCode);

		nOffset += nRowLength;
	}

	// adjust blank string
	if (pHeader->nDataSize == sizeof(KHeader) + nColumn * nRow * sizeof(KField))
	{
		szBlank = (char*)m_pData + pHeader->nDataSize;
		szBlank[0] = '\0';
		++pHeader->nDataSize;
	}
	else
	{
		szBlank = (char*)m_pData + pHeader->nDataSize - 1;
	}

	for (size_t nColumn = 0; nColumn < pHeader->nColumn; ++nColumn)
	{
		KField* pFieldType = (KField*)(m_pData + sizeof(KHeader));
		if (pFieldType[nColumn].i == FIELD_FORMAT_SLASHES_STRING)
			pFieldType[nColumn].i = FIELD_FORMAT_STRING;
		if (pFieldType[nColumn].i == FIELD_FORMAT_STRING)
		{
			for (size_t nRow = 0; nRow < pHeader->nRow; ++nRow)
			{
				KField*	pFieldCell = (KField*)(m_pData + sizeof(KHeader) + (pHeader->nColumn * (nRow + 1) + nColumn) * sizeof(KField));
				if (pFieldCell->s == 0)
				{
					pFieldCell->s = szBlank;
				}
			}
		}
	}

	if (pHeader->nDataSize != sizeof(KHeader) + nColumn * nRow * sizeof(KField) + nSize)
	{
 		ASSERT(pHeader->nDataSize < sizeof(KHeader) + nColumn * nRow * sizeof(KField) + nSize);

        nRetCode = ToStringOffset();
        KGLOG_PROCESS_ERROR(nRetCode);

		pData = (PBYTE)realloc(m_pData, pHeader->nDataSize);
		KGLOG_PROCESS_ERROR(pData);

		m_pData = pData;
        pData = NULL;

        nRetCode = ToStringPointer();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

	bResult = TRUE;
Exit0:
    if (!bResult)
    {
        SAFE_FREE(pData);
        SAFE_FREE(m_pData);
    }
	return bResult;
}

BOOL KBinTab::LoadTxtTab(LPCSTR szFilePath, LPCSTR szFormat)
{
	int nRetCode = false;
	BOOL bResult = FALSE;
	char* szText = NULL;
	size_t nSize = 0;
	size_t nReaded = 0;
	IFile* pFile = NULL;

	KG_ASSERT_EXIT(szFilePath);

	pFile = g_OpenFile(szFilePath);
	KGLOG_PROCESS_ERROR(pFile);

	nSize = pFile->Size();
	KGLOG_PROCESS_ERROR(nSize > 0);

	szText = (char*)malloc(nSize);
	KGLOG_PROCESS_ERROR(szText);

	nReaded = (size_t)pFile->Read(szText, (unsigned long)nSize);
	KGLOG_PROCESS_ERROR(nReaded == nSize);

	nRetCode = LoadTxtTab(szText, nSize, szFormat);
	KGLOG_PROCESS_ERROR(nRetCode);

	bResult = TRUE;
Exit0:
	SAFE_FREE(szText);
	SAFE_RELEASE(pFile);

	if (!bResult)
	{
		SAFE_FREE(m_pData);

		KGLogPrintf(KGLOG_ERR, "JX3Represent KBinTab::LoadTxtTab(%s, %s)\n", szFilePath, szFormat);
	}

	return bResult;
}

BOOL KBinTab::SaveTxtTab(LPCSTR szFilePath)
{
	// TODO:
	return FALSE;
}

