////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KTgaManager.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-19 14:00:19
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KTGAMANAGER_H_
#define _INCLUDE_KTGAMANAGER_H_

////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include "KTga.h"
#include "UiTex.h"

////////////////////////////////////////////////////////////////////////////////

#define MAX_ADSORB_DISTANCE     5
#define MAX_NEIGHBOR_DISTANCE   5

class KTga32;

enum SORT_TYPE
{
    SORT_TYPE_BEGINE = 0,
    SORT_BY_NAME = SORT_TYPE_BEGINE,
    SORT_BY_WIDTH,
    SORT_BY_HEIGHT,
    SORT_BY_SIZE,
    SORT_TO_SAVE,
    SORT_TO_AUTO_MAKE_WIDTH,
    SORT_TO_AUTO_MAKE_HEIGHT,
    SORT_TO_AUTO_MAKE_SIZE,
    SORT_TYPE_END,
};

LPCTSTR g_GetSortTypeName(int nSortType);

struct KTGABLOCKSAVEDATA
{
    TCHAR szName[MAX_PATH];
    int nLeft;
    int nTop;
    int nWidth;
    int nHeight;
};

class KTgaManager
{
    typedef std::vector<KTGABLOCKSAVEDATA> KTgaSaveDataArray;
    typedef std::vector<KTgaBlock *> KTgaBlockArray;
protected:
    KTgaManager();
    ~KTgaManager();
public:
    static int Init();
    static int Exit();
    static KTgaManager &GetSelf();

    int Append(KTgaBlock *pTgaBlock);
    int Remove(KTgaBlock *pTgaBlock);
    int Sort(int nSortType);
    int Clear();
    int IsTgaExist(LPCTSTR pcszName);
    int GetIndex(LPCTSTR pcszName);
    KTgaBlock *Get(int nIndex);
    KTgaBlock *Get(LPCTSTR pcszName);

    int GetIndexByUnPosedIndex(int nUnPosedIndex);
    int GetIndexByPosedIndex(int nPosedIndex);
    
    int GetPosedBlockIndex(int x, int y);

    int UpDataListView();

    int UpDataShow();

    int UnMakeAll();

    int CalculateBlockPos(int nIndex, POINT &ptWanted);
    int TryDropBlock(int nIndex);

    int IsTgaPosSeted(LPCTSTR pcszName);
    int IsFrameShoudBeSaved(LPCTSTR pcszName);

    int GetSaveBlockCount();
    int GetSaveArea(RECT &rcSave);

    int ClearSaveData();
    int FormateSaveData();
    int GetSaveIndex(LPCTSTR pcszName);
    int GetSaveBufferWidth();
    int GetSaveBufferHeight();
    int Save(FILE *fpFile);
    int ExportText(FILE *fpFile);
    int AppendSaveTgaData(KTgaBlock *pBlock, int nLeft, int nTop);
    KTga32 &GetSaveTga();

    int ExportNewText(FILE *fpFile);
    int AutoMakeup();
    int ExportAllTga(LPCTSTR pcszPath);
    int ExportTga(LPCTSTR pcszPath, int nIndex);
	int ExportCur(LPCTSTR pcszPath, int nIndex);
    int RemoveThePosedBlock();
    int Remove(int nIndex);
    int Replace(int nReplacedIndex, int nReplaceIndex);
private:
    static bool SortByNameFunction(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2);
    static bool SortByWidthFunction(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2);
    static bool SortByHeightFunction(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2);
    static bool SortBySizeFunction(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2);
    static bool SortToSaveFunction(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2);
    static bool SortToAutoMakeupWidth(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2);
    static bool SortToAutoMakeupHeight(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2);
    static bool SortToAutoMakeupSize(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2);

    int AutoMakeupWidthDanger();
    int AutoMakeupHeightDanger();
    int AutoMakeupNothingDanger();
protected:
private:
    static KTgaManager *ms_pSelf;
    KTgaBlockArray m_aTgaBlock;

    KTgaSaveDataArray m_aSaveData;
    int m_nSaveWidth;
    int m_nSaveHeight;
    KTga32 m_SaveTga;
};




#endif //_INCLUDE_KTGAMANAGER_H_
