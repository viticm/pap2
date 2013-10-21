////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KItemManager.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2008-03-05 15:42:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KITEMMANAGER_H_
#define _KITEMMANAGER_H_
#include "KEnchantLib.h"

class KItemLib;
class KItem;
class KItemInfo;
struct KAttribute;

class KItemManager
{
public:
	KItemManager();
	~KItemManager();
	BOOL        Init();
    void        UnInit();

    KItem*      GenerateItem(
        int     nVersion,           DWORD dwTabType,        DWORD  dwTabIndex, 
        DWORD   dwId = ERROR_ID,    DWORD dwRandSeed = 0,   int    nMagicNum = 0, 
        int     nQuality = 0,       int   nMagic[] = NULL,  time_t nGenTime = 0
    );

    BOOL        GenerateItemTo(
        KItem* pDestItem, 
        int   nVersion,        DWORD dwTabType,      DWORD dwTabIndex, 
        DWORD dwId = ERROR_ID, DWORD dwRandSeed = 0, int nMagicNum = 0, 
        int   nQuality = 0,    int nMagic[] = NULL,  time_t nGenTime = 0
    );
    
    KItem*      GenerateItemFromBinaryData(DWORD dwItemID, void* pvBuffer, size_t uBufferSize);

    KItem*      CloneItem(const KItem* pItem);
	KItemInfo*  GetItemInfo(int nVersion, DWORD dwTabType, DWORD dwIndex);
	BOOL        GetSetAttrib(KAttribute& pAttr, DWORD dwSetID, int nSetNum);
    BOOL        FreeItem(KItem* pItem);

public:
	KItemLib*   m_pItemLib;
    KEnchantLib m_EnchantLib;
};
#endif
