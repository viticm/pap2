////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KAnimateManager.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-22 14:14:13
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KANIMATEMANAGER_H_
#define _INCLUDE_KANIMATEMANAGER_H_

////////////////////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////////////////////

class KAnimateBlock;

class KAnimateManager
{
    typedef std::vector<int> KIntArray;
    typedef std::vector<KAnimateBlock *> KAnimateArray;
protected:
    KAnimateManager();
    ~KAnimateManager();

public:
    static int Init();
    static int Exit();
    static KAnimateManager &GetSelf();

    int Append(KAnimateBlock *pAnimate);
    int Remove(KAnimateBlock *pAnimate);
    int Remove(int nIndex);

    int Clear();
    int Sort(int nSortType);

    int IsFrameAlreadyHasBrother(LPCTSTR pcszFramName);
    KAnimateBlock *GetParent(LPCTSTR pcszFramName);
	KAnimateBlock *GetParentJustFromName(LPCTSTR pcszFramName);

    int FormateSaveData();
    
    int ClearSaveData();
    int AppendSaveData(int nData);

    int GetSaveAnimateCount();
    int GetSaveAnimateBufferSize();
    int Save(FILE *fpFile);
    int ExportText(FILE *fpFile);

    int UpdataListView();
    KAnimateBlock *Get(int nIndex);
    KAnimateBlock *Get(LPCTSTR pcszAnimateName);
    int IsFrameAleadyInAnyAnimateGroup(LPCTSTR pcszName);

protected:
    static bool SortToSaveFunction(KAnimateBlock *pAninmate1, KAnimateBlock *pAnimate2);
private:
    static KAnimateManager *ms_pSelf;
    KAnimateArray m_aAnimate;

    int m_nSaveAnimateCount;
    KIntArray m_aSaveAnimateData;
};





#endif //_INCLUDE_KANIMATEMANAGER_H_
