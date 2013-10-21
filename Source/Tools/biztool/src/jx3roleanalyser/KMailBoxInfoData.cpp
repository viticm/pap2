///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KMailBoxInfoData.cpp
// Creator  :   liuzhibiao
// Date     :   2009-7-10
// Comment  :   v1.0        从数据库中查询角色邮件信息
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KMailBoxInfoData.h"

KMailBoxInfoData::KMailBoxInfoData(void)
{
    m_ulNextMailID = 0;
    m_ulLastGlobalMailIndex = 0;
    m_usVersion = 0;
}

KMailBoxInfoData::~KMailBoxInfoData(void)
{
    for (size_t i = 0; i < m_MailList.size(); i++)
    {
        KG_DELETE_ARRAY(m_MailList[i]);
    }
    m_MailList.clear();
}

int KMailBoxInfoData::LoadVersion5(unsigned char* pbyData, size_t uDataLen)
{
    int                 nResult   = false;
    unsigned char*      pbyOffset = pbyData;
    //size_t              uLeftLen  = uDataLen;
    long long           llLeftLen  = uDataLen;
    KMail_DB_V5*        pSrcMail  = NULL;
    KMAIL_DB_NOW_USE*   pDstMail  = NULL;
    size_t              uMailLen  = 0;
    unsigned char*      pucItemData = NULL;
    int                 nItemCount = 0;
    size_t              uItemLen    = 0;
    
    while (llLeftLen > 0)
    {
        pSrcMail = (KMail_DB_V5*)pbyOffset;

        KG_PROCESS_ERROR(llLeftLen >= sizeof(KMail_DB_V5));

        uMailLen = sizeof(KMail_DB_V5) + pSrcMail->wTextLen;
        KG_PROCESS_ERROR(llLeftLen >= uMailLen);

        pDstMail = new KMAIL_DB_NOW_USE;
        KG_PROCESS_ERROR(pDstMail);

        pDstMail->ulMailID      = pSrcMail->dwMailID;
        pDstMail->ucType        = pSrcMail->byType;
        pDstMail->bRead         = pSrcMail->bRead;
        pDstMail->dwSenderID    = pSrcMail->dwSenderID;

        strncpy(pDstMail->szSenderName, pSrcMail->szSenderName, sizeof(pDstMail->szSenderName));
        pDstMail->szSenderName[sizeof(pDstMail->szSenderName) - 1] = '\0';

        strncpy(pDstMail->szTitle, pSrcMail->szTitle, sizeof(pDstMail->szTitle));
        pDstMail->szTitle[sizeof(pDstMail->szTitle) - 1] = '\0';

        pDstMail->nSendTime = pSrcMail->nSendTime;
        pDstMail->nRecvTime = pSrcMail->nRecvTime;
        pDstMail->nLockTime = pSrcMail->nLockTime;
        pDstMail->nMoney    = pSrcMail->nMoney;
        pDstMail->uTextLen  = pSrcMail->wTextLen;

        memset(pDstMail->szText, '\0', sizeof(pDstMail->szText));
        strncpy(pDstMail->szText, (char*)pSrcMail->byData, pDstMail->uTextLen);

        pucItemData = pSrcMail->byData + pSrcMail->wTextLen;

        nItemCount  = 0;

        for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
        {
            uMailLen += pSrcMail->ItemDesc[i].byDataLen;
            uItemLen += pSrcMail->ItemDesc[i].byDataLen;

            pDstMail->MailItem[i].bAcquired = pSrcMail->ItemDesc[i].bAcquired;
            pDstMail->MailItem[i].nPrice    = pSrcMail->ItemDesc[i].nPrice;
            pDstMail->MailItem[i].ucDataLen = pSrcMail->ItemDesc[i].byDataLen;

            if (pDstMail->MailItem[i].ucDataLen == sizeof(KCOMMON_ITEM_DATA))
            {
                pDstMail->MailItem[i].nIsEquip = false;
                memcpy(&(pDstMail->MailItem[i].commonEquipData), pucItemData, pDstMail->MailItem[i].ucDataLen);
                nItemCount++;
            }
            else if (pDstMail->MailItem[i].ucDataLen == sizeof(KCUSTOM_EQUI_DATA))
            {
                pDstMail->MailItem[i].nIsEquip = true;
                memcpy(&(pDstMail->MailItem[i].customEquipData), pucItemData, pDstMail->MailItem[i].ucDataLen);
                nItemCount++;
            }
            else
            {
                //KG_PROCESS_ERROR(false);
            }

            pucItemData += pSrcMail->ItemDesc[i].byDataLen;
        }

        pDstMail->nItemCount = nItemCount;

        m_MailList.push_back(pDstMail);

        pDstMail = NULL;

        pbyOffset += uMailLen;
        llLeftLen  -= uMailLen;
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE(pDstMail);
    }
    return nResult;
}

int KMailBoxInfoData::LoadVersion4(unsigned char* pbyData, size_t uDataLen)
{
    int             nResult   = false;
    unsigned char*  pbyOffset = pbyData;
    //size_t          uLeftLen  = uDataLen;
    long long       llLeftLen = uDataLen;
    KMail_DB_V4*    pSrcMail  = NULL;
    KMAIL_DB_NOW_USE*    pDstMail  = NULL;
    size_t          uMailLen  = 0;

    size_t          uItemLen    = 0;
    size_t          uTextLen    = 0;
    unsigned char*  pucItemData = NULL;
    int             nItemCount  = 0;

    assert(pbyData);

    while (llLeftLen > 0)
    {
        pSrcMail = (KMail_DB_V4*)pbyOffset;

        KG_PROCESS_ERROR(llLeftLen >= sizeof(KMail_DB_V4));

        uMailLen = sizeof(KMail_DB_V4) + pSrcMail->wTextLen;

        pDstMail = (KMAIL_DB_NOW_USE*)new KMAIL_DB_NOW_USE;
        KG_PROCESS_ERROR(pDstMail);

        //memcpy(pDstMail, pSrcMail, uMailLen);
        pDstMail->ulMailID  = pSrcMail->dwMailID;
        pDstMail->ucType    = pSrcMail->byType;
        pDstMail->bRead     = pSrcMail->bRead;
        pDstMail->dwSenderID = ERROR_ID; // 第5版加的

        strncpy(pDstMail->szSenderName, pSrcMail->szSenderName, sizeof(pDstMail->szSenderName));
        pDstMail->szSenderName[sizeof(pDstMail->szSenderName) - 1] = '\0';

        strncpy(pDstMail->szTitle, pSrcMail->szTitle, sizeof(pDstMail->szTitle));
        pDstMail->szTitle[sizeof(pDstMail->szTitle) - 1] = '\0';

        pDstMail->nSendTime = pSrcMail->nSendTime;
        pDstMail->nRecvTime = pSrcMail->nRecvTime;
        pDstMail->nLockTime = pSrcMail->nLockTime;
        pDstMail->nMoney    = pSrcMail->nMoney;
        pDstMail->uTextLen  = pSrcMail->wTextLen;
        
        memset(pDstMail->szText, '\0', sizeof(pDstMail->szText));
        strncpy(pDstMail->szText, (char*)pSrcMail->byData, pDstMail->uTextLen);

        pucItemData = pSrcMail->byData + pSrcMail->wTextLen;

        nItemCount  = 0;

        for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
        {
            uMailLen += pSrcMail->ItemDesc[i].byDataLen;
            uItemLen += pSrcMail->ItemDesc[i].byDataLen;

            pDstMail->MailItem[i].bAcquired = pSrcMail->ItemDesc[i].bAcquired;
            pDstMail->MailItem[i].nPrice    = pSrcMail->ItemDesc[i].nPrice;
            pDstMail->MailItem[i].ucDataLen = pSrcMail->ItemDesc[i].byDataLen;

            if (pDstMail->MailItem[i].ucDataLen == sizeof(KCOMMON_ITEM_DATA))
            {
                pDstMail->MailItem[i].nIsEquip = false;
                memcpy(&(pDstMail->MailItem[i].commonEquipData), pucItemData, pDstMail->MailItem[i].ucDataLen);
                nItemCount++;
            }
            else if (pDstMail->MailItem[i].ucDataLen == sizeof(KCUSTOM_EQUI_DATA))
            {
                pDstMail->MailItem[i].nIsEquip = true;
                memcpy(&(pDstMail->MailItem[i].customEquipData), pucItemData, pDstMail->MailItem[i].ucDataLen);
                nItemCount++;
            }
            else
            {
                //KG_PROCESS_ERROR(false);
            }

            pucItemData += pSrcMail->ItemDesc[i].byDataLen;
        }

        pDstMail->nItemCount = nItemCount;

        m_MailList.push_back(pDstMail);

        pDstMail = NULL;

        pbyOffset += uMailLen;
        llLeftLen -= uMailLen;
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE(pDstMail);
    }
    return nResult;
}

int KMailBoxInfoData::LoadVersion3(unsigned char* pbyData, size_t uDataLen)
{
    int             nResult         = false;
    unsigned char*  pbyOffset       = pbyData;
    //size_t          uLeftLen        = uDataLen;
    long long       llLeftLen = uDataLen;
    size_t          uContentDataLen = 0;
    size_t          uOldLen         = 0;
    size_t          uOldMailLen     = 0;
    size_t          uNewMailLen     = 0;
    KMail_DB_V3*    pSrcMail        = NULL;
    KMAIL_DB_NOW_USE*    pDstMail        = NULL;
    
    unsigned char*  pItemData       = NULL;

    while (llLeftLen > 0)
    {
        pSrcMail = (KMail_DB_V3*)pbyOffset;
        
        KG_PROCESS_ERROR(pSrcMail);

        uContentDataLen = pSrcMail->Content.wTextLen + pSrcMail->Content.wItemLen;

        uOldMailLen = sizeof(KMail_DB_V3) + uContentDataLen;
        KG_PROCESS_ERROR(llLeftLen >= uOldMailLen);

        uNewMailLen = sizeof(KMAIL_DB_NOW_USE) + uContentDataLen;

        pDstMail = new KMAIL_DB_NOW_USE;
        KG_PROCESS_ERROR(pDstMail);

        //memcpy(pDstMail, pSrcMail, uOldMailLen);
        pDstMail->ulMailID = pSrcMail->dwMailID;
        
        if (pSrcMail->byFlags & KMAIL_BIT_FLAG_SYSTEM)
        {
            pDstMail->ucType = eMailType_System;
        }
        else
        {
            pDstMail->dwSenderID = ERROR_ID;
            pDstMail->ucType = eMailType_Player;
        }
        
        pDstMail->bRead = !(pSrcMail->byFlags & KMAIL_BIT_FLAG_UNREAD);

        strncpy(pDstMail->szSenderName, pSrcMail->szSenderName, ROLE_NAME_LEN);
        pDstMail->szSenderName[sizeof(pDstMail->szSenderName) - 1] = '\0';

        strncpy(pDstMail->szTitle, pSrcMail->szTitle, MAIL_TITLE_LEN);
        pDstMail->szTitle[sizeof(pDstMail->szTitle) - 1] = '\0';

        pDstMail->nSendTime = pSrcMail->nReceiveTime;
        pDstMail->nRecvTime = pSrcMail->nReceiveTime;
        pDstMail->nLockTime = pSrcMail->nGetAttachmentLock;

        pDstMail->nMoney    = pSrcMail->Content.nMoney;
        pDstMail->uTextLen  = pSrcMail->Content.wTextLen;
        
        KG_PROCESS_ERROR(pDstMail->uTextLen < sizeof(pDstMail->szText));

        if (pDstMail->uTextLen > 0)
        {
            memset(pDstMail->szText, '\0', sizeof(pDstMail->szText));
            strncpy(pDstMail->szText, (char*)pSrcMail->Content.byData, pDstMail->uTextLen);

        }
        
        pItemData = pSrcMail->Content.byData + pDstMail->uTextLen;

        pDstMail->nItemCount = 0;

        if (pSrcMail->Content.wItemLen)
        {
            pDstMail->MailItem[0].ucDataLen = (BYTE)pSrcMail->Content.wItemLen;
            pDstMail->MailItem[0].bAcquired = false;
            pDstMail->MailItem[0].nPrice    = 0;
        }

        if (pDstMail->MailItem[0].ucDataLen == sizeof(KCOMMON_ITEM_DATA))
        {
            pDstMail->MailItem[0].nIsEquip = false;
            memcpy(&(pDstMail->MailItem[0].commonEquipData), pItemData, sizeof(pDstMail->MailItem[0].commonEquipData));
            pDstMail->nItemCount = 1;
        }
        else if (pDstMail->MailItem[0].ucDataLen == sizeof(KCUSTOM_EQUI_DATA))
        {
            pDstMail->MailItem[0].nIsEquip = true;
            memcpy(&(pDstMail->MailItem[0].customEquipData), pItemData, sizeof(pDstMail->MailItem[0].customEquipData));
            pDstMail->nItemCount = 1;
        }
        else
        {
            //KG_PROCESS_ERROR(false);
        }

        m_MailList.push_back(pDstMail);

        pDstMail = NULL;

        pbyOffset += uOldMailLen;
        llLeftLen  -= uOldMailLen;
    }
    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE(pDstMail);
    }
    return nResult;
}

int KMailBoxInfoData::PaserMailBoxInfo(unsigned char* pbyData, size_t uDataLen)
{
    int                nResult   = false;
    int                nRetCode  = false;
    unsigned char*     pbyOffset = pbyData;
    size_t             uLeftLen  = uDataLen;
    KMAILBOX_DB_HEAD*  pHead     = NULL;

    assert(pbyData);
    assert(uDataLen > 0);

    KG_PROCESS_ERROR(uLeftLen >= sizeof(KMAILBOX_DB_HEAD));
    pHead = (KMAILBOX_DB_HEAD*)pbyOffset;

    m_ulNextMailID = pHead->dwNextMailID;
    m_ulLastGlobalMailIndex = pHead->dwLastGlobalMailIndex;
    m_usVersion = pHead->wVersion;

    pbyOffset += sizeof(KMAILBOX_DB_HEAD);
    uLeftLen  -= sizeof(KMAILBOX_DB_HEAD);

    if (pHead->wVersion == 5)
    {
        nRetCode = LoadVersion5(pbyOffset, uLeftLen);
        KG_PROCESS_ERROR(nRetCode);
    }
    else if (pHead->wVersion == 4)// 临时使用,邮件版本3
    {
        nRetCode = LoadVersion4(pbyOffset, uLeftLen);
        KG_PROCESS_ERROR(nRetCode);
    } 
    else if (pHead->wVersion == 3)
    {
        nRetCode = LoadVersion3(pbyOffset, uLeftLen);
        KG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

