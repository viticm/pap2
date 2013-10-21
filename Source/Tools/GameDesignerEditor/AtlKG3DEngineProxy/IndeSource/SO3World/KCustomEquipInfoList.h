////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KCustomEquipInfoList.h
//  Version     : 1.0
//  Creator     : Zhao Chunfeng
//  Create Date : 2008-07-22 11:24:42
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KCUSTOMEQUIPINFOLIST_H_
#define _KCUSTOMEQUIPINFOLIST_H_

#include <map>
#include "KItemInfo.h"

class KCustomEquipInfoList
{
public:
    BOOL                 Load(const char cszFileName[]);
    void                 Clear();
    KCustomEquipInfo*    GetEquipInfo(DWORD dwID);
    
    BOOL LoadLine(
        ITabFile* piTabFile, int nLine, KCustomEquipInfo* pItemInfo, const KCustomEquipInfo& crDefaultInfo
    );

	typedef std::map<DWORD, KCustomEquipInfo>	KCUSTOM_EQUIP_INFO_LIST;
	KCUSTOM_EQUIP_INFO_LIST    m_EquipInfoList;
};

#endif
