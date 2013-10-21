#include "StdAfx.h"
#include <math.h>
#include "KTrackList.h"
#include "KMath.h"

#define TRACK_DIR "track"
#define COL_X      "X"
#define COL_Y      "Y"
#define COL_Z      "Z"

#define COL_ID      "ID"
#define COL_MAP     "Map"
#define COL_DESC    "Desc"

#define DEFAULT_FLY_SPEED   28

KTrackList::KTrackList()
{
}

BOOL KTrackList::Load()
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    ITabFile*   piTabFile   = NULL;
    int         nTabHeight  = 0;
    char        szFileName[MAX_PATH];

    snprintf(szFileName, sizeof(szFileName), "%s/%s/track.tab", SETTING_DIR, TRACK_DIR);
    szFileName[sizeof(szFileName) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFileName);
    KGLOG_PROCESS_ERROR(piTabFile);

    nTabHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabHeight >= 2);

    for (int nLine = 2; nLine <= nTabHeight; nLine++)
    {
        DWORD       dwID        = 0;
        KTRACK*     pTrack      = NULL;
        KTRACK_INFO TrackInfo;
        pair<KTRACK_TABLE::iterator, bool> IntResult;
        
        nRetCode = piTabFile->GetInteger(nLine, COL_ID, 0, (int*)&dwID);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(nLine, COL_MAP, 0, (int*)&TrackInfo.dwMapID);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetString(nLine, COL_DESC, "?", TrackInfo.szDesc, sizeof(TrackInfo.szDesc));
        KGLOG_PROCESS_ERROR(nRetCode);

        IntResult = m_TrackTable.insert(KTRACK_TABLE::value_type(dwID, TrackInfo));
        KGLOG_PROCESS_ERROR(IntResult.second);
        pTrack = &IntResult.first->second.Track;
        
        nRetCode = LoadTrack(dwID, pTrack);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = CalculateCriticalFrame(pTrack);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_TrackTable.clear();
    }
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KTrackList::Save()
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    ITabFile*   piTabFile   = NULL;
    int         nLine       = 0;
    char        szFileName[MAX_PATH];

    snprintf(szFileName, sizeof(szFileName), "%s/%s/track.tab", SETTING_DIR, TRACK_DIR);
    szFileName[sizeof(szFileName) - 1] = '\0';

    piTabFile = g_CreateTabFile();
    KGLOG_PROCESS_ERROR(piTabFile);

    nRetCode = piTabFile->InsertNewCol(COL_ID);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    nRetCode = piTabFile->InsertNewCol(COL_MAP);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    nRetCode = piTabFile->InsertNewCol(COL_DESC);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    nLine = 2;

    for (KTRACK_TABLE::iterator it = m_TrackTable.begin(); it != m_TrackTable.end(); ++it)
    {
        DWORD           dwID        = it->first;
        KTRACK_INFO*    pTrackInfo  = &it->second;
        
        nRetCode = piTabFile->WriteInteger(nLine, COL_ID, dwID);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->WriteInteger(nLine, COL_MAP, pTrackInfo->dwMapID);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->WriteString(nLine, COL_DESC, pTrackInfo->szDesc);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = SaveTrack(dwID, pTrackInfo->Track);
        KGLOG_PROCESS_ERROR(nRetCode);

        nLine++;
    }

    nRetCode = piTabFile->Save(szFileName);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KTrackList::GetTrackPoint(KTRACK_POINT* pPoint, DWORD dwTrackID, int nFrame)
{
    BOOL                    bResult         = false;
    KTRACK_TABLE::iterator  TabIter         = m_TrackTable.find(dwTrackID);
    KTRACK*                 pTrack          = NULL;
    size_t                  uTrackSize      = 0;
    KCRITICAL_POINT*        pLeft           = NULL;
    KCRITICAL_POINT*        pRight          = NULL;
    KCRITICAL_POINT*        pNode           = NULL;
    int                     nDeltaFrame     = 0;
    int                     nDeltaFrameL    = 0;
    int                     nDeltaFrameR    = 0;

    KGLOG_PROCESS_ERROR(TabIter != m_TrackTable.end());
    pTrack = &TabIter->second.Track;

    uTrackSize = pTrack->size();
    KGLOG_PROCESS_ERROR(uTrackSize >= 2);

    pLeft   = &(*pTrack)[0];
    pRight  = pLeft + uTrackSize - 1;

    KGLOG_PROCESS_ERROR(nFrame >= pLeft->nFrame);

    // 二分查找,找到一个节点满足条件: nFrame >= pNode->nFrame, 并且下一个节点的满足 nFrame <= pNode->nFrame

    while (pLeft->nFrame <= nFrame && pRight->nFrame > nFrame && pRight - pLeft > 1)
    {
        pNode = pLeft + (pRight - pLeft) / 2;

        if (pNode->nFrame <= nFrame)
        {
            pLeft = pNode;
            continue;
        }

        pRight = pNode;
    }

    // 如果没找到,或者已经过了终点,则返回false

    if (pRight->nFrame == nFrame)
    {
        pLeft = pRight - 1;

        pPoint->nX          = pRight->nX;
        pPoint->nY          = pRight->nY;
        pPoint->nZ          = pRight->nZ;
        pPoint->nDirection  = g_GetDirection(pLeft->nX, pLeft->nY, pRight->nX, pRight->nY);
        goto Exit1;
    }

    KG_PROCESS_ERROR(pRight->nFrame > nFrame);

    // 插值计算nFrame对应的坐标

    nDeltaFrame     = pRight->nFrame - pLeft->nFrame;
    nDeltaFrameL    = nFrame - pLeft->nFrame;
    nDeltaFrameR    = pRight->nFrame - nFrame;

    pPoint->nX         = (pLeft->nX * nDeltaFrameR + pRight->nX * nDeltaFrameL) / nDeltaFrame;
    pPoint->nY         = (pLeft->nY * nDeltaFrameR + pRight->nY * nDeltaFrameL) / nDeltaFrame;
    pPoint->nZ         = (pLeft->nZ * nDeltaFrameR + pRight->nZ * nDeltaFrameL) / nDeltaFrame;
    pPoint->nDirection = g_GetDirection(pLeft->nX, pLeft->nY, pRight->nX, pRight->nY);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}


BOOL KTrackList::SaveTrack(DWORD dwID, const KTRACK& crTrack)
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    ITabFile*   piTabFile   = NULL;
    int         nLineIndex  = 0;
    char        szFileName[MAX_PATH];

    snprintf(szFileName, sizeof(szFileName), "%s/%s/track%u.tab", SETTING_DIR, TRACK_DIR, dwID);
    szFileName[sizeof(szFileName) - 1] = '\0';

    piTabFile = g_CreateTabFile();
    KGLOG_PROCESS_ERROR(piTabFile);

    nRetCode = piTabFile->InsertNewCol(COL_X);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    nRetCode = piTabFile->InsertNewCol(COL_Y);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    nRetCode = piTabFile->InsertNewCol(COL_Z);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    nLineIndex = 2;

    for (KTRACK::const_iterator it = crTrack.begin(); it != crTrack.end(); ++it)
    {
        nRetCode = piTabFile->WriteInteger(nLineIndex, COL_X, it->nX);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->WriteInteger(nLineIndex, COL_Y, it->nY);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->WriteInteger(nLineIndex, COL_Z, it->nZ);
        KGLOG_PROCESS_ERROR(nRetCode);

        nLineIndex++;
    }

    nRetCode = piTabFile->Save(szFileName);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KTrackList::LoadTrack(DWORD dwID, KTRACK* pTrack)
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    ITabFile*   piTabFile   = NULL;
    int         nTabHeight  = 0;
    char        szFileName[MAX_PATH];

    snprintf(szFileName, sizeof(szFileName), "%s/%s/track%u.tab", SETTING_DIR, TRACK_DIR, dwID);
    szFileName[sizeof(szFileName) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFileName);
    KGLOG_PROCESS_ERROR(piTabFile);

    nTabHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTabHeight >= 3);

    for (int nLine = 2; nLine <= nTabHeight; nLine++)
    {
        KCRITICAL_POINT point;
        
        nRetCode = piTabFile->GetInteger(nLine, COL_X, 0, &point.nX);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, COL_Y, 0, &point.nY);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = piTabFile->GetInteger(nLine, COL_Z, 0, &point.nZ);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        point.nFrame = 0; // 稍后计算

        pTrack->push_back(point);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KTrackList::CalculateCriticalFrame(KTRACK* pTrack)
{
    BOOL                bResult     = false;
    size_t              uSize       = pTrack->size();
    KCRITICAL_POINT*    pPreNode    = NULL;
    KCRITICAL_POINT*    pNode       = NULL;
    KCRITICAL_POINT*    pLast       = NULL;

    KGLOG_PROCESS_ERROR(uSize >= 2);

    pPreNode = &(*pTrack)[0];
    pNode    = &(*pTrack)[1];
    pLast    = &(*pTrack)[uSize - 1];

    pPreNode->nFrame = 0;

    while (pNode <= pLast)
    {
        float fDeltaX   = (float)(pNode->nX - pPreNode->nX);
        float fDeltaY   = (float)(pNode->nY - pPreNode->nY);
        float fDeltaZ   = (float)ZPOINT_TO_XYPOINT(pNode->nZ - pPreNode->nZ);
        float fDistance = sqrt(fDeltaX * fDeltaX + fDeltaY * fDeltaY + fDeltaZ * fDeltaZ);

        pNode->nFrame = pPreNode->nFrame + (int)(fDistance / DEFAULT_FLY_SPEED);

        pPreNode = pNode;
        pNode++;
    }

    bResult = true;
Exit0:
    return bResult;
}
