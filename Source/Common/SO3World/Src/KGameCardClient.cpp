#include "stdafx.h"
#include "KGameCardClient.h"

#ifdef _CLIENT

KGameCardClient::KGameCardClient()
{
    m_nTotalCount = 0;
}

KGameCardClient::~KGameCardClient()
{
    m_CardList.clear();
}

BOOL KGameCardClient::UpdateList(int nTotalCount, int nCount, KGAME_CARD* pList)
{
    BOOL bResult = false;

    assert(pList);

    m_nTotalCount = nTotalCount;

    Clear();

    for (int i = 0; i < nCount; i++)
    {
        m_CardList.push_back(pList[i]);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGameCardClient::Clear()
{
    m_CardList.clear();
    return true;
}

#endif // _CLIENT
