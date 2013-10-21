#include "stdafx.h"
#include "Global.h"
#include "KEnchantLib.h"
#include "KAttribute.h"
#include "StrMap.h"
#include "Engine/KMemory.h"

#define	MAX_ENCHANT_ATTR_COUNT	4

KEnchantLib::KEnchantLib()
{
}

KEnchantLib::~KEnchantLib()
{
}

BOOL KEnchantLib::Init()
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    ITabFile*   piFile          = NULL;
    int         nHeight         = 0;
    KAttribute* pAttributeList  = NULL;
    char        szFile[MAX_PATH];

	snprintf(szFile, sizeof(szFile), "%s/%s", ITEM_SETTING_DIR, "Enchant.tab");
    szFile[sizeof(szFile) - 1] = '\0';
   
    piFile = g_OpenTabFile(szFile);
	KGLOG_PROCESS_ERROR(piFile);

    nHeight = piFile->GetHeight();  
	KGLOG_PROCESS_ERROR(nHeight >= 1);

    for (int nIndex = 2; nIndex <= nHeight; nIndex++)
    {
        int         nID              = 0;
        int         nAttributeID     = 0;
        int         nAttributeValue1 = 0;
        int         nAttributeValue2 = 0;
        KENCHANT    EnchantInfo;
        std::pair<KENCHANT_MAP::iterator, bool> InsRet;

        bRetCode = piFile->GetInteger(nIndex, "ID", 0, &nID);
		KGLOG_PROCESS_ERROR(bRetCode && nID > 0);

        // 属性链表为空是正常的,附魔可能只有表现效果而已
        pAttributeList = LoadLineAttribute(piFile, nIndex);

        bRetCode = piFile->GetInteger(nIndex, "RepresentIndex", -1, &EnchantInfo.nRepresentIndex);
		KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piFile->GetInteger(nIndex, "RepresentID", 0, &EnchantInfo.nRepresentID);
		KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piFile->GetInteger(nIndex, "Time", 0, &EnchantInfo.nTime);
		KGLOG_PROCESS_ERROR(bRetCode && EnchantInfo.nTime < USHRT_MAX);

		bRetCode = piFile->GetInteger(nIndex, "DestItemSubType", -1, &EnchantInfo.nDestItemSubType);
		KGLOG_PROCESS_ERROR(bRetCode);
        
        InsRet = m_Enchant.insert(std::make_pair(nID, EnchantInfo));
		KGLOG_PROCESS_ERROR(InsRet.second);

        InsRet.first->second.pAttribute = pAttributeList;
        pAttributeList = NULL;
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        while (pAttributeList)                                    
        {                                                  
            KAttribute* pTmp = pAttributeList;                    
            pAttributeList = pAttributeList->pNext;                      
            KMemory::Delete(pTmp);
            pTmp = NULL;
        }

        for (KENCHANT_MAP::iterator it = m_Enchant.begin(); it != m_Enchant.end(); ++it)
        {
            KAttribute* pAttribute = it->second.pAttribute;
            while (pAttribute)
            {
                KAttribute* pNext = pAttribute->pNext;
                KMemory::Delete(pAttribute);
                pAttribute = pNext;
            }
        }
        m_Enchant.clear();
    }
    KG_COM_RELEASE(piFile);
    return bResult;
}

void KEnchantLib::UnInit()
{
    for (KENCHANT_MAP::iterator it = m_Enchant.begin(); it != m_Enchant.end(); ++it)
    {
        KAttribute* pAttribute = it->second.pAttribute;
        while (pAttribute)
        {
            KAttribute* pNext = pAttribute->pNext;
            KMemory::Delete(pAttribute);
            pAttribute = pNext;
        }
    }
    m_Enchant.clear();
}

KENCHANT* KEnchantLib::GetEnchantInfo(int nEnchantID)
{   
    KENCHANT_MAP::iterator it = m_Enchant.find(nEnchantID);
    if (it != m_Enchant.end())
    {
        return &it->second;
    }
 
    return NULL;
}


KAttribute* KEnchantLib::LoadLineAttribute(ITabFile* piFile, int nLine)
{
    KAttribute* pResult             = NULL;
    int         nRetCode            = 0;
    int         nAttributeID        = 0;
    int         nAttributeValue1    = 0;
    int         nAttributeValue2    = 0;
    KAttribute* pAttributeList      = NULL;
    KAttribute* pNewAttribute       = NULL;
    char        szKey[32];
    char        szTempValue[KATTRIBUTE_STRING_LEN];

    for (int i = 0; i < MAX_ENCHANT_ATTR_COUNT; i++)
    {
        sprintf(szKey, "Attribute%dID", i + 1);

        nRetCode = piFile->GetString(nLine, szKey, "atInvalid", szTempValue, sizeof(szTempValue));
        KGLOG_PROCESS_ERROR(nRetCode);

        MAP_STRING_EXTERN(ATTRIBUTE_TYPE, szTempValue, nAttributeID);

        if (nAttributeID == atInvalid)
            break;
        
        sprintf(szKey, "Attribute%dValue1", i + 1);
        nRetCode = piFile->GetInteger(nLine, szKey, 0, &nAttributeValue1);
        KGLOG_PROCESS_ERROR(nRetCode);

        sprintf(szKey, "Attribute%dValue2", i + 1);
        nRetCode = piFile->GetInteger(nLine, szKey, 0, &nAttributeValue2);
        KGLOG_PROCESS_ERROR(nRetCode);

        pNewAttribute = KMemory::New<KAttribute>();
        KGLOG_PROCESS_ERROR(pNewAttribute);
        
        pNewAttribute->nKey    = nAttributeID;
        pNewAttribute->nValue1 = nAttributeValue1;
        pNewAttribute->nValue2 = nAttributeValue2;
        pNewAttribute->pNext   = NULL;

        ADD_ATTRIBUTE_TO_END(pAttributeList, pNewAttribute);
        
        pNewAttribute = NULL;
    }

    pResult = pAttributeList;
Exit0:
    if (pResult == NULL)
    {
        if (pNewAttribute != NULL)
        {
            KMemory::Delete(pNewAttribute);
            pNewAttribute = NULL;
        }

        while (pAttributeList)                                    
        {                                                  
            KAttribute* pTmp = pAttributeList;                    
            pAttributeList = pAttributeList->pNext;                      
            KMemory::Delete(pTmp);
        }
    }
    return pResult;
}
