#include "stdafx.h"
#include <limits.h>
#include "SO3GlobalDef.h"
#include "KSO3World.h"
#include "KPathFinder.h"
#include "Engine/KG_Time.h"
#include "KCharacter.h"
#include "KScene.h"
#include "KRegion.h"
#include "KCell.h"

#define OVER_RATE 32

enum K_PATHFINDER_DIRECTION
{
    pfdNorth    = 0,
    pfdSouth,
    pfdEast,
    pfdWest,
    pfdNorthEast,
    pfdNorthWest,
    pfdSouthEast,
    pfdSouthWest,

    pfdTotal
};

static const int sc_naGO[pfdTotal][2] = {
    {           0, -CELL_LENGTH}, // NORTH
    {           0,  CELL_LENGTH}, 
    { CELL_LENGTH,            0}, 
    {-CELL_LENGTH,            0}, 
    { CELL_LENGTH, -CELL_LENGTH}, 
    {-CELL_LENGTH, -CELL_LENGTH}, 
    { CELL_LENGTH,  CELL_LENGTH}, 
    {-CELL_LENGTH,  CELL_LENGTH}, // SOUTH_WEST
};

bool operator<(const KCellExt& _Left, const KCellExt& _Right)
{
    return _Left.nTotalMark > _Right.nTotalMark;
};

BOOL KPathFinder::FindQuickPath(
    KScene* pScene, 
    int nSrcX, int nSrcY, int nSrcZ, 
    int nDstX, int nDstY, int nDstZ, 
    int nMinDistance, int nMaxDistance, int nHeight
)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KCellExt    SrcCellExt;

    m_OpenSet.clear();
    m_CloseSet.clear();
    m_Path.clear();
    m_PathNextRIt = m_Path.rend();

    KG_PROCESS_ERROR(pScene);

    m_pScene = pScene;
    m_nHeight = nHeight;

    bRetCode = PointToCell(&m_pSrcCell, &m_nSrcX, &m_nSrcY, &m_nSrcZ, pScene, nSrcX, nSrcY, nSrcZ);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = PointToCell(&m_pDstCell, &m_nDstX, &m_nDstY, &m_nDstZ, pScene, nDstX, nDstY, nDstZ);
    KG_PROCESS_ERROR(bRetCode);

    SrcCellExt.pCell            = m_pSrcCell;
    SrcCellExt.nX               = m_nSrcX;
    SrcCellExt.nY               = m_nSrcY;
    SrcCellExt.nZ               = m_nSrcZ;
    SrcCellExt.itFather         = m_CloseSet.end();
    SrcCellExt.nGoneMark        = 0;
    SrcCellExt.nTotalMark       = 0;
    SrcCellExt.nEstimateMark    = 1;
    SrcCellExt.nDirection       = pfdTotal;

    m_OpenSet.push_back(SrcCellExt);
    std::make_heap(m_OpenSet.begin(), m_OpenSet.end());

    m_nMinDistance  = nMinDistance;
    m_nMaxDistance  = nMaxDistance;
    m_lState        = pfsWaiting;

    #ifdef MAKE_MARK
    MakeMark(52, m_nDstX, m_nDstY, m_nDstZ);
    #endif

    bRetCode = Active(100);
    KG_PROCESS_ERROR(bRetCode);

    m_lState = pfsSuccess;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPathFinder::PointToCell(
    KCell** ppRetCell, int* pnRetX, int* pnRetY, int* pnRetZ, 
    KScene* pScene, int nX, int nY, int nZ
)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    int         nRelativeZ  = 0;
    KRegion*    pRegion     = NULL;

    KG_PROCESS_ERROR(ppRetCell);
    KG_PROCESS_ERROR(pScene);
    KG_PROCESS_ERROR(nX >= 0 && nY >= 0);

    *pnRetX = nX;
    *pnRetY = nY;

    pRegion  = pScene->GetRegionByPoint(nX, nY);
    KG_PROCESS_ERROR(pRegion);
    nX = (nX / CELL_LENGTH) % REGION_GRID_WIDTH;
    nY = (nY / CELL_LENGTH) % REGION_GRID_HEIGHT;

    *ppRetCell = pRegion->GetProximalCellForObj(nX, nY, nZ, m_nHeight, false, &nRelativeZ);
    KG_PROCESS_ERROR(*ppRetCell);
    *pnRetX -= *pnRetX % CELL_LENGTH;
    *pnRetY -= *pnRetY % CELL_LENGTH;

    *pnRetZ  = (*ppRetCell)->m_wHighLayer * POINT_PER_ALTITUDE + nRelativeZ;

    bResult = true;
Exit0:
    return bResult;
}

KPathFinder::KPathFinder()
{
    m_lState        = pfsInvalid;
    m_pSrcCell      = NULL; 
    m_pDstCell      = NULL;
    m_pLastCell     = NULL;
    m_nMinDistance  = 0;
    m_nMaxDistance  = 0;
    m_nHeight       = 0;
}

BOOL KPathFinder::Init()
{
    m_lState = pfsRuning;

    m_pSrcCell      = NULL;
    m_pDstCell      = NULL;
    m_pLastCell     = NULL;
    m_nMinDistance  = 0;
    m_nMaxDistance  = 0;
    m_nHeight       = 0;

    m_OpenSet.clear();
    m_CloseSet.clear();
    m_Path.clear();

    m_lState = pfsInvalid;
    return true;
}

BOOL KPathFinder::UnInit()
{
    m_lState = pfsRuning;

    m_pSrcCell      = NULL;
    m_pDstCell      = NULL;
    m_pLastCell     = NULL;
    m_nMinDistance  = 0;
    m_nMaxDistance  = 0;
    m_nHeight       = 0;

    m_OpenSet.clear();
    m_CloseSet.clear();
    m_Path.clear();

    m_lState = pfsInvalid;
    return true;
}

long KPathFinder::GetState()
{
    return m_lState;
}

BOOL KPathFinder::Active(int nStep)
{
    int             nResult         = false;
    int             nRetCode        = pfsInvalid;
    BOOL            bRetCode        = false;
    BOOL            bFindTargetFlag = false;
    unsigned long   ulStartTime     = 0;
    unsigned long   ulEndTime       = 0;
    KCellExt        CellExt;
    std::pair<KCellExtMapIt, bool> RetPair;
    KCellExtMapIt   it;

    nRetCode = InterlockedCompareExchange(&m_lState, pfsRuning, pfsWaiting);
    KG_PROCESS_ERROR(nRetCode == pfsWaiting);

    while (nStep-- > 0)
    {
        // If we have tried all cells in open set, then we failed to find a path.
        if (m_OpenSet.empty())
        {
            m_lState = pfsFailed;
            break;
        }

        // Get a cell from open set witch has the minimal F-value.
        std::pop_heap(m_OpenSet.begin(), m_OpenSet.end());
        CellExt = m_OpenSet.back();
        m_OpenSet.pop_back();

        // Put it into close set.
        RetPair = m_CloseSet.insert(std::make_pair(CellExt.pCell, CellExt));
        KG_PROCESS_ERROR(RetPair.second);

        m_pLastCell = CellExt.pCell;

        // Yes, we find the destination!
        if (m_pLastCell == m_pDstCell || CellExt.nEstimateMark == 0)
        {
            m_lState = pfsSuccess;
            break;
        }

        it = RetPair.first;
        bRetCode = AddAdjacentCellsToOpenHeap(&(it->second), it);
        KG_PROCESS_ERROR(bRetCode);
    }

    GeneratePath();
    nResult = true;
Exit0:

    if (m_lState == pfsRuning)
    {
        m_lState = pfsWaiting;
    }
    return nResult;
}

int KPathFinder::Estimator(int nX, int nY, int nZ)
{
    int nResult     = -1;
    int nStraight   = 0;
    int nDiagonal   = 0;

    KG_PROCESS_ERROR(nX >= 0 && nY >= 0 && nZ >= 0);

    nDiagonal = min(abs(nX - m_nDstX), abs(nY - m_nDstY));
    nStraight = abs(nX - m_nDstX) + abs(nY - m_nDstY);
    nResult   = CELL_LENGTHD * nDiagonal + CELL_LENGTH * (nStraight - 2 * nDiagonal);

    nResult += ZPOINT_TO_XYPOINT(abs(m_nDstZ - nZ)) * 2;
    nResult += OVER_RATE;

    KG_PROCESS_ERROR(nResult >= m_nMinDistance);

    nResult -= m_nMaxDistance;

    if (nResult < 0)
    {
        nResult = 0;
    }

Exit0:
    return nResult;
}

struct IsSameCell
{
    IsSameCell(const KCellExt& crCellExt) : m_pCell(crCellExt.pCell) {};

    bool operator()(const KCellExt& crCellExt)
    {
        return crCellExt.pCell == m_pCell;
    }

    KCell* m_pCell;
};

BOOL KPathFinder::AddAdjacentCellsToOpenHeap(KCellExt* pCellExt, std::map<KCell*, KCellExt>::iterator itFather)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(pCellExt);

    for (int i = 0; i < pfdTotal; ++i)
    {
        int                 nGoX                = pCellExt->nX + sc_naGO[i][0];
        int                 nGoY                = pCellExt->nY + sc_naGO[i][1];
        int                 nGoZ                = pCellExt->nZ;
        int                 nRelativeZ          = 0;
        int                 nObstacleDirection  = 0;
        int                 nEstimateMark       = 0;
        KRegion*            pGoRegion           = NULL;
        KCell*              pGoCell             = NULL;
        KCellExt            CellExt; 
        KCellExtVectorIt    it;
        KCellExtMapIt       itMap;

        if (nGoX < 0 || nGoY < 0)
        {
            continue;
        }

        pGoRegion = m_pScene->GetRegionByPoint(nGoX, nGoY);
        if (!pGoRegion)
        {
            continue;
        }

        CellExt.nX = nGoX;
        CellExt.nY = nGoY;

        nGoX = (nGoX / CELL_LENGTH) % REGION_GRID_WIDTH;
        nGoY = (nGoY / CELL_LENGTH) % REGION_GRID_HEIGHT;
        pGoCell = pGoRegion->GetAdaptedCellForObj(
            nGoX, nGoY, nGoZ, CLIMB_ABILITY * 4, m_nHeight, true,
            &nRelativeZ, &nObstacleDirection
        );
        if (!pGoCell)
        {
            continue;
        }

        // ·ÀÖ¹ÏÂË®
        if (pGoCell->m_BaseInfo.dwCellType == ctWater && nRelativeZ <= -m_nHeight / 2)
        {
            continue;
        }

        // ·ÀÖ¹ÏÂÐüÑÂ
        if (
            POINT_PER_ALTITUDE * ((int)pCellExt->pCell->m_wHighLayer - (int)pGoCell->m_wHighLayer)
            >= XYPOINT_TO_ZPOINT(CELL_LENGTH * 16)
        )
        {
            continue;
        }

        // No jump. 
        if (nRelativeZ >= XYPOINT_TO_ZPOINT(CELL_LENGTH * 3))
        {
            continue;
        }

        if (nRelativeZ > 0)
        {
            nRelativeZ = 0;
        }

        CellExt.nZ = pGoCell->m_wHighLayer * POINT_PER_ALTITUDE + nRelativeZ;
        CellExt.pCell = pGoCell;

        if (abs(nGoZ - CellExt.nZ) >= CLIMB_ABILITY * 4)
        {
            continue;
        }

        itMap = m_CloseSet.find(CellExt.pCell);
        if (itMap != m_CloseSet.end())    // Skip if in close set.
        {
            continue;
        }

        if (i < 4)
        {
            CellExt.nGoneMark = pCellExt->nGoneMark + CELL_LENGTH;
        }
        else
        {
            CellExt.nGoneMark = pCellExt->nGoneMark + CELL_LENGTHD * 3 / 2;
        }
        CellExt.nGoneMark += ZPOINT_TO_XYPOINT(abs(CellExt.nZ - pCellExt->nZ)) * 2;

        nEstimateMark = Estimator(CellExt.nX, CellExt.nY, CellExt.nZ);
        if (nEstimateMark < 0)  // Negative means no way. 
        {
            continue;
        }

        CellExt.nEstimateMark   = nEstimateMark;
        CellExt.nTotalMark      = CellExt.nGoneMark + CellExt.nEstimateMark;
        CellExt.itFather        = itFather;
        CellExt.nDirection      = i;

        it = std::find_if(m_OpenSet.begin(), m_OpenSet.end(), IsSameCell(CellExt));
        if (it != m_OpenSet.end())
        {
            if (CellExt.nGoneMark < it->nGoneMark)    // If a better way.
            {
                it->nGoneMark   = CellExt.nGoneMark;
                it->itFather    = CellExt.itFather;
                it->nDirection  = CellExt.nDirection;
            }

            continue;
        }

        // Unknown cell, push into open set.
        m_OpenSet.push_back(CellExt);
        std::push_heap(m_OpenSet.begin(), m_OpenSet.end());
    }

    bResult = true;
Exit0:
    return bResult;
}

#ifdef MAKE_MARK
void KPathFinder::MakeMark(DWORD dwDoodadID, int nX, int nY, int nZ)
{
#ifdef _SERVER
    int         nRetCode            = false;
    KDoodad*    pDoodad             = NULL;
    DWORD       dwDoodadTemplateID  = 52;
    int         nDir                = 0;

    if (dwDoodadID > 0)
    {
        dwDoodadTemplateID = dwDoodadID;
    }

    pDoodad = g_MemoryPool.New<KDoodad>();
    KGLOG_PROCESS_ERROR(pDoodad);

    nRetCode = g_pSO3World->m_DoodadSet.Register(pDoodad, ERROR_ID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pDoodad->Init();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = g_pSO3World->m_Settings.m_DoodadTemplateList.LoadFromTemplate(dwDoodadTemplateID, pDoodad);
    KGLOG_PROCESS_ERROR(nRetCode);

    pDoodad->m_nFaceDirection = nDir;
    pDoodad->SetDisappearFrames(GAME_FPS * 15, false);

    //ÁÙÊ±´úÂë
    pDoodad->m_nLastLootCount = 1;

    nRetCode = g_pSO3World->AddDoodad(pDoodad, m_pScene, nX + CELL_LENGTH / 2, nY + CELL_LENGTH / 2, nZ);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
#endif
    return;
}
#endif // MAKE_MARK

BOOL KPathFinder::GetPathFirstPoint(int* pnX, int* pnY, int* pnZ)
{
    BOOL        bResult = false;

    KG_PROCESS_ERROR(pnX);
    KG_PROCESS_ERROR(pnY);
    KG_PROCESS_ERROR(pnZ);

    KG_PROCESS_ERROR(!m_Path.empty());

    m_PathNextRIt = m_Path.rbegin();

    *pnX = m_PathNextRIt->nX;
    *pnY = m_PathNextRIt->nY;
    *pnZ = m_PathNextRIt->nZ;

    #ifdef MAKE_MARK
    MakeMark(11, *pnX, *pnY, *pnZ);
    #endif

    m_PathNextRIt++;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPathFinder::GetPathNextPoint(int* pnX, int* pnY, int* pnZ)
{
    BOOL        bResult = false;

    KG_PROCESS_ERROR(pnX);
    KG_PROCESS_ERROR(pnY);
    KG_PROCESS_ERROR(pnZ);

    KG_PROCESS_ERROR(!m_Path.empty());
    KG_PROCESS_ERROR(m_PathNextRIt != m_Path.rend());

    *pnX = m_PathNextRIt->nX;
    *pnY = m_PathNextRIt->nY;
    *pnZ = m_PathNextRIt->nZ;

    #ifdef MAKE_MARK
    MakeMark(11, *pnX, *pnY, *pnZ);
    #endif

    m_PathNextRIt++;

    bResult = true;
Exit0:
    return bResult;
}

void KPathFinder::GeneratePath()
{
    KCellExtMapIt   itFather        = m_CloseSet.end();
    int             nLastDirection  = pfdTotal;

    KG_PROCESS_ERROR(m_pLastCell);

    m_Path.clear();

    itFather = m_CloseSet.find(m_pLastCell);
    while (itFather != m_CloseSet.end())
    {
        if (nLastDirection != itFather->second.nDirection)
        {
            m_Path.push_back(itFather->second);
            nLastDirection = itFather->second.nDirection;
        }

        itFather = (itFather->second).itFather;
    }

Exit0:
    return;
}
