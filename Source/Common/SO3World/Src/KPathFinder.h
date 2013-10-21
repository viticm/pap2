#ifndef _KPATHFINDER_H_
#define _KPATHFINDER_H_

#include <vector>
#include <map>
#include <algorithm>

#define MAKE_MARK    1

#define CELL_LENGTHD (int)(CELL_LENGTH * 1.4)

class KCell;
class KScene;

struct KCellExt 
{
    KCell*      pCell;
    std::map<KCell*, KCellExt>::iterator    itFather;

    int         nX;
    int         nY;
    int         nZ;

    int         nGoneMark;
    int         nEstimateMark;
    int         nTotalMark;

    int         nDirection;
};

enum PATH_FINDER_STATE
{
    pfsInvalid = 0,

    pfsRuning,
    pfsWaiting,

    pfsSuccess,
    pfsFailed,

    pfsTotal
};

class KPathFinder
{
public:
    KPathFinder();
    ~KPathFinder() {};

    BOOL Init();
    BOOL UnInit();

    BOOL FindQuickPath(
        KScene* pScene, 
        int nSrcX, int nSrcY, int nSrcZ, 
        int nDstX, int nDstY, int nDstZ, 
        int nMinDistance, int nMaxDistance, int nHeight
    );

    long GetState();

    BOOL GetPathFirstPoint(int* pnX, int* pnY, int* pnZ);
    BOOL GetPathNextPoint(int* pnX, int* pnY, int* pnZ);

    template <class TFunc>
    void TraverseOpenSet(TFunc& Func);

    template <class TFunc>
    void TraverseCloseSet(TFunc& Func);

    template <class TFunc>
    void TraversePathSet(TFunc& Func);

private:
    BOOL Active(int nStep);

    BOOL PointToCell(
        KCell** ppRetCell, int* pnRetX, int* pnRetY, int* pnRetZ, 
        KScene* pScene, int nX, int nY, int nZ
    );

    int  Estimator(int nX, int nY, int nZ);
    BOOL AddAdjacentCellsToOpenHeap(KCellExt* pCellExt, std::map<KCell*, KCellExt>::iterator itFather);

    void GeneratePath();

#ifdef MAKE_MARK
    void MakeMark(DWORD dwDoodadID, int nX, int nY, int nZ);
#endif

    volatile long           m_lState;

    int                     m_nSrcX;
    int                     m_nSrcY;
    int                     m_nSrcZ;
    int                     m_nDstX;
    int                     m_nDstY;
    int                     m_nDstZ;
    int                     m_nMinDistance;
    int                     m_nMaxDistance;
    int                     m_nHeight;

    KScene*                 m_pScene;
    KCell*                  m_pSrcCell;
    KCell*                  m_pDstCell;
    KCell*                  m_pLastCell;

    typedef std::vector<KCellExt>               KCellExtVector;
    typedef KCellExtVector::iterator            KCellExtVectorIt;
    typedef KCellExtVector::reverse_iterator    KCellExtVectorRIt;

    typedef std::map<KCell*, KCellExt>          KCellExtMap;
    typedef KCellExtMap::iterator               KCellExtMapIt;

    KCellExtVector          m_OpenSet;
    KCellExtMap             m_CloseSet;

    KCellExtVector          m_Path;
    KCellExtVectorRIt       m_PathNextRIt;
};

template <class TFunc>
void KPathFinder::TraverseOpenSet(TFunc& Func)
{
    KCellExtVectorIt it;
    KCellExtVectorIt itEnd;

    itEnd = m_OpenSet.end();
    for (it = m_OpenSet.begin(); it != itEnd; ++it)
    {
        Func(&(*it));
    }
}

template <class TFunc>
void KPathFinder::TraverseCloseSet(TFunc& Func)
{
    KCellExtMapIt it;
    KCellExtMapIt itEnd;

    itEnd = m_CloseSet.end();
    for (it = m_CloseSet.begin(); it != itEnd; ++it)
    {
        Func(&(it->second));
    }
}

template <class TFunc>
void KPathFinder::TraversePathSet(TFunc& Func)
{
    KCellExtVectorRIt it;
    KCellExtVectorRIt itEnd;

    itEnd = m_Path.rend();
    for (it = m_Path.rbegin(); it != itEnd; ++it)
    {
        Func(&(*it));
    }
}

#endif
