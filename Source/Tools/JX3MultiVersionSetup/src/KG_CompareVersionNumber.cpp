////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_CompareVersionNumber.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-10-21  20:16:44
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Used to parse version number.
const unsigned KGU_VERSION_NUMBER_SECTION_COUNT = 4;
struct _KGU_VERSION_NUMBER
{
    unsigned uSectionValueArrray[KGU_VERSION_NUMBER_SECTION_COUNT];
    TCHAR    chPartition;
};

int _ParseVersionNumber(const TCHAR cszVersionNumber[], _KGU_VERSION_NUMBER *pRetVersionNumber);
int _GetPartitionChar(const TCHAR cszVersionNumber[], TCHAR *pchRetPartitionChar);

////////////////////////////////////////////////////////////////////////////////////////////////////
int KG_CompareVersionNumber(const TCHAR cszLeft[], const TCHAR cszRight[], int *pnRetResult)
{
    int nResult  = false;
    int nRetCode = false;
    _KGU_VERSION_NUMBER LeftVersionNumber;
    _KGU_VERSION_NUMBER RightVersionNumber;

    KGLOG_PROCESS_ERROR(cszLeft);
    KGLOG_PROCESS_ERROR(cszLeft[0]);
    KGLOG_PROCESS_ERROR(cszRight);
    KGLOG_PROCESS_ERROR(cszRight[0]);
    KGLOG_PROCESS_ERROR(pnRetResult);

    nRetCode = _ParseVersionNumber(cszLeft, &LeftVersionNumber);
    KGLOG_PROCESS_ERROR(nRetCode && "Left is not a version number.");

    nRetCode = _ParseVersionNumber(cszRight, &RightVersionNumber);
    KGLOG_PROCESS_ERROR(nRetCode && "Right is not a version number.");

    for (unsigned i = 0; i < KGU_VERSION_NUMBER_SECTION_COUNT; i++)
    {
        if (LeftVersionNumber.uSectionValueArrray[i] > RightVersionNumber.uSectionValueArrray[i])
        {
            *pnRetResult = 1;
            KG_PROCESS_SUCCESS(true);
        }

        if (LeftVersionNumber.uSectionValueArrray[i] < RightVersionNumber.uSectionValueArrray[i])
        {
            *pnRetResult = -1;
            KG_PROCESS_SUCCESS(true);
        }
    }

    *pnRetResult = 0;   // Here indicate two version number equals.
Exit1:
    nResult = true;
Exit0:
    if (!nResult && cszLeft && cszRight)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to compare two version number, Left is `%s`, Right is `%s`.",
            cszLeft, cszRight
        );
    }
    return nResult;
}

int _ParseVersionNumber(const TCHAR cszVersionNumber[], _KGU_VERSION_NUMBER *pRetVersionNumber)
{
    int nResult  = false;
    int nRetCode = false;
    TCHAR szVersionNumber[256];
    TCHAR chPartition = _T('\0');
    TCHAR *pchSection = NULL;
    TCHAR *pchPartition = NULL;

    ASSERT(cszVersionNumber);
    ASSERT(cszVersionNumber[0]);
    ASSERT(pRetVersionNumber);

    nRetCode = _GetPartitionChar(cszVersionNumber, &chPartition);
    KGLOG_PROCESS_ERROR(nRetCode && "_GetPartitionChar()");

    pRetVersionNumber->chPartition = chPartition;

    _tcsncpy(szVersionNumber, cszVersionNumber, _countof(szVersionNumber));
    KGLOG_PROCESS_ERROR(szVersionNumber[_countof(szVersionNumber) - 1] == _T('\0'));

    pchSection = szVersionNumber;
    for (unsigned i = 0; i < KGU_VERSION_NUMBER_SECTION_COUNT - 1; i++)
    {
        pchPartition = _tcschr(pchSection, chPartition);
        KGLOG_PROCESS_ERROR(pchPartition);
        *pchPartition = _T('\0');

        pRetVersionNumber->uSectionValueArrray[i] = _tstoi(pchSection);
        pchSection = pchPartition + 1;
    }    
    pRetVersionNumber->uSectionValueArrray[KGU_VERSION_NUMBER_SECTION_COUNT - 1] = _tstoi(pchSection);

    nResult = true;
Exit0:
    return nResult;
}

int _GetPartitionChar(const TCHAR cszVersionNumber[], TCHAR *pchRetPartitionChar)
{
    const TCHAR *pchIndex = NULL;
    const TCHAR PARTITION[] = {_T('.'), _T('-'), _T(',')};

    ASSERT(cszVersionNumber);
    ASSERT(cszVersionNumber[0]);
    ASSERT(pchRetPartitionChar);

    for (unsigned i = 0; i < _countof(PARTITION); i++)
    {
        pchIndex = _tcschr(cszVersionNumber, PARTITION[i]);
        if (pchIndex)
        {
            *pchRetPartitionChar = PARTITION[i];
            return true;
        }
    }

    return false;   // Here indicate the cszVersionNumber not include any partition char.
}