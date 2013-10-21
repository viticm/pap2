///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KAuctionData.h
// Creator  :   liuzhibiao
// Date     :   2009-7-10
// Comment  :   v1.0        从数据库中查询寄卖行信息
///////////////////////////////////////////////////////////////

#ifndef KAUCTIONDATA_H
#define KAUCTIONDATA_H


#define KGLOG_PROCESS_ERROR(Condition) \
    do  \
    {   \
        if (!(Condition))   \
            goto Exit0;     \
    } while (false)

#include "Engine/EngineBase.h"
#include "Engine/FileType.h"
#include "SO3GlobalDef.h"
#include "KGPublic.h"
#include "KItemDef.h"

class KAuctionData
{
public:
    KAuctionData(void);
    ~KAuctionData(void);
public:
    int PaserAuctionItemData(unsigned char* pbyData, size_t uDataLen);
    KCUSTOM_EQUI_DATA m_ItemData;
    int               m_IsEqui;
};

#endif // KAUCTIONDATA_H
