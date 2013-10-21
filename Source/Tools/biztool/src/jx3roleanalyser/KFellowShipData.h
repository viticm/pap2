///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KFellowShipData.h
// Creator  :   liuzhibiao
// Date     :   2009-7-10
// Comment  :   v1.0        从数据库中查询角色好友信息
///////////////////////////////////////////////////////////////

#ifndef KFELLOWSHIPDATA_H
#define KFELLOWSHIPDATA_H

#include <vector>
using namespace std;
#include "Engine/EngineBase.h"
#include "KGPublic.h"
#include "ProtocolBasic.h"
class KPlayer{};
#include "KFellowship.h"

class KFellowShipData
{
public:
    KFellowShipData(void);
    ~KFellowShipData(void);

public:
    int PaserFellowShipData(unsigned char* pbyData, size_t uDataLen);

    vector<KG_FELLOWSHIP_ENTRY_V1*>             m_FellowshipArray;
    vector<KG_FELLOWSHIP_FOE_ENTRY_V1*>         m_FoeArray;
    vector<KG_FELLOWSHIP_BLACKLIST_ENTRY_V1*>   m_BlackArray;
};
#endif //KFELLOWSHIPDATA_H
