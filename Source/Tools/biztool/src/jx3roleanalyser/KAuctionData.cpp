///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KAuctionData.cpp
// Creator  :   liuzhibiao
// Date     :   2009-7-10
// Comment  :   v1.0        从数据库中查询寄卖行信息
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KAuctionData.h"

KAuctionData::KAuctionData(void)
{
    memset(&m_ItemData, 0, sizeof(m_ItemData));
    m_IsEqui = false;
}

KAuctionData::~KAuctionData(void)
{
}

int KAuctionData::PaserAuctionItemData(unsigned char* pbyData, size_t uDataLen)
{
    int                 nResult     = false;
    KCOMMON_ITEM_DATA*  pItemData   = (KCOMMON_ITEM_DATA*)pbyData;
    KCUSTOM_EQUI_DATA*  pEquiData   = (KCUSTOM_EQUI_DATA*)pbyData;
    assert(pItemData);

    KG_PROCESS_ERROR(uDataLen >= sizeof(KCOMMON_ITEM_DATA));

    if (uDataLen == sizeof(KCOMMON_ITEM_DATA))
    {
        m_IsEqui = false;
        m_ItemData.byBind         = pItemData->byBind;
        m_ItemData.byTabType      = pItemData->byTabType;
        m_ItemData.byVersion      = pItemData->byVersion;
        m_ItemData.nGenTime       = pItemData->nGenTime;
        m_ItemData.wDurability    = pItemData->wDurability;
        m_ItemData.wTabIndex      = pItemData->wTabIndex;
    }
    else if (uDataLen == sizeof(KCUSTOM_EQUI_DATA))
    {
        m_IsEqui = true;
        m_ItemData.byBind         = pEquiData->byBind;
        m_ItemData.byTabType      = pEquiData->byTabType;
        m_ItemData.byVersion      = pEquiData->byVersion;
        m_ItemData.nGenTime       = pEquiData->nGenTime;
        m_ItemData.wDurability    = pEquiData->wDurability;
        m_ItemData.wTabIndex      = pEquiData->wTabIndex;

        m_ItemData.dwRandSeed                         = pEquiData->dwRandSeed;
        m_ItemData.wLeftEnchantTime                   = pEquiData->wLeftEnchantTime;
        m_ItemData.byColorID                          = pEquiData->byColorID;
        m_ItemData.wEnchant[eiPermanentEnchant]       = pEquiData->wEnchant[eiPermanentEnchant];
        m_ItemData.wEnchant[eiTemporaryEnchant]       = pEquiData->wEnchant[eiTemporaryEnchant];
        m_ItemData.wEnchant[eiMount1]                 = pEquiData->wEnchant[eiMount1];
        m_ItemData.wEnchant[eiMount2]                 = pEquiData->wEnchant[eiMount2];
        m_ItemData.wEnchant[eiMount3]                 = pEquiData->wEnchant[eiMount3];
        m_ItemData.wEnchant[eiMount4]                 = pEquiData->wEnchant[eiMount4];
    }
    else
    {
        KG_PROCESS_ERROR(false);
    }

    nResult = true;
Exit0:
    return nResult;
}
