////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_Statistic3.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2005-09-22 14:51:35
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_Statistic3.h"

KG_Statistic3::KG_Statistic3() :
    m_ulRefCount(1)
{
}

KG_Statistic3::~KG_Statistic3()
{
}

////////////////////////////////////////////////////////////////////////////////
int KG_Statistic3::Begin(void *pvContext)
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////
int KG_Statistic3::OnceStatiticOperation(
    unsigned int uRoleDataSize, const void *pcvRoleData
)
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////
int KG_Statistic3::End(void *pvContext)
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////
int KG_Statistic3::GetStatisticData(
    KG_STATISTIC_DATA_TYPE eDataType, IKG_Buffer **ppiStatistic
)
{
    char *pszBuffer = NULL;

    ASSERT(ppiStatistic);
    *ppiStatistic = KG_MemoryCreateBuffer(100);
    ASSERT(*ppiStatistic);
    pszBuffer = (char *)(*ppiStatistic)->GetData();
    sprintf(pszBuffer, "统计类型：%d, 统计代码还没有实现呢", eDataType);

    return true;
}
