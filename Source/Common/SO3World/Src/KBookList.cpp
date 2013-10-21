#include "stdafx.h"
#include "KBookList.h"
#include "KPlayer.h"
#include "KPlayerServer.h"
#include "KRecipe.h"

BOOL KBookList::Init(KPlayer* pPlayer)
{
    BOOL bResult = false;

	KGLOG_PROCESS_ERROR(pPlayer);

	memset(m_byBookList, 0, sizeof(m_byBookList));
	m_pSelf = pPlayer;

	bResult = true;
Exit0:
	return bResult;
}

void KBookList::UnInit()
{
    m_pSelf = NULL;
	return;
}

BOOL KBookList::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL        bResult     = false;
    KBookDB*    pBookDB     = NULL;
    size_t      uLeftSize   = uBufferSize;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KBookDB));
    
    pBookDB = (KBookDB*)pbyBuffer;
    uLeftSize -= sizeof(KBookDB);

    pBookDB->uDataLen = MAX_READ_BOOK_ID;
    
    KGLOG_PROCESS_ERROR(uLeftSize >= pBookDB->uDataLen);
    memcpy(pBookDB->byData, m_byBookList, pBookDB->uDataLen);

    uLeftSize -= pBookDB->uDataLen;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBookList::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL        bResult     = false;
    KBookDB*    pBookDB     = NULL;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KBookDB) + MAX_READ_BOOK_ID);
    pBookDB = (KBookDB*)pbyData;

    KGLOG_PROCESS_ERROR(pBookDB->uDataLen == MAX_READ_BOOK_ID);

    memcpy(m_byBookList, pBookDB->byData, pBookDB->uDataLen);

#ifdef _SERVER
    g_PlayerServer.DoSyncAllBookState(m_pSelf->m_nConnIndex, m_pSelf);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BYTE KBookList::GetBookState(DWORD dwBookID)
{
	BYTE byResult = 0;

	KGLOG_PROCESS_ERROR(dwBookID > 0);
	KGLOG_PROCESS_ERROR(dwBookID <= MAX_READ_BOOK_ID);

	byResult = m_byBookList[dwBookID - 1];
Exit0:
	return byResult;
}

BOOL KBookList::SetBookState(DWORD dwBookID, BYTE byState)
{
	BOOL bResult  = false;

	KGLOG_PROCESS_ERROR(m_pSelf);
	KGLOG_PROCESS_ERROR(dwBookID > 0);
	KGLOG_PROCESS_ERROR(dwBookID <= MAX_READ_BOOK_ID);

	m_byBookList[dwBookID - 1] = byState;

#ifdef _SERVER
	g_PlayerServer.DoSyncBookState(m_pSelf->m_nConnIndex, dwBookID, byState);
#endif

	bResult = true;
Exit0:
	return bResult;
}

BOOL KBookList::AddBook(DWORD dwBookID, DWORD dwSubID)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	BYTE byState = 0;

	KGLOG_PROCESS_ERROR(dwBookID > 0);
	KGLOG_PROCESS_ERROR(dwBookID <= MAX_READ_BOOK_ID);
    KGLOG_PROCESS_ERROR(dwSubID > 0);
	KGLOG_PROCESS_ERROR(dwSubID <= MAX_READ_BOOK_SUB_ID);

	byState = GetBookState(dwBookID);
	
	byState |= (1 << (dwSubID - 1));

	bRetCode = SetBookState(dwBookID, byState);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KBookList::IsBookMemorized(DWORD dwBookID, DWORD dwSubID)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	KGLOG_PROCESS_ERROR(dwBookID > 0);
	KGLOG_PROCESS_ERROR(dwBookID <= MAX_READ_BOOK_ID);
	KGLOG_PROCESS_ERROR(dwSubID <= MAX_READ_BOOK_SUB_ID);

	bRetCode = (1 << (dwSubID - 1)) & (m_byBookList[dwBookID - 1]);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KBookList::IsBookMemorized(int nRecipeID)
{
    BOOL    bResult  = false;
    BOOL    bRetCode = false;
	DWORD   dwBookID = 0;
    DWORD   dwSubID  = 0;

    bRetCode = RecipeID2BookID(nRecipeID, dwBookID, dwSubID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = IsBookMemorized(dwBookID, dwSubID);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

void KBookList::Clear()
{
    memset(m_byBookList, 0, sizeof(m_byBookList));
}
