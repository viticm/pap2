////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGPropertyTreeCtrl.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-2-13 17:01:55
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGPROPERTYTREECTRL_H_
#define _KGPROPERTYTREECTRL_H_

#include "KGTreeCtrl.h"
#include "KGPropertyEditUtility.h"

#define SUBSET_TREE_DATA    65535 
#define RELATING_DATA_COUNT 4

typedef int (*Funtion)(ITabFile* pTabFile, int nRow);

class KGPropertyTreeCtrl : public KGTreeCtrl
{
public :
	int BandData( 		
		ITabFile** ppTabFile, IIniFile** ppQuestIniFile, IIniFile** ppIniFile,
		KGPrimaryKeyManager* nKeyManager, int pnTypeRow[], int nTypeRowSize,
		int nTitleRow, int nDataRow, CImageList* pImageList, Funtion GetImageIndex
	);

	int BandData( 		
		ITabFile** ppTabFile, IIniFile** ppQuestIniFile, IIniFile** ppIniFile,
		KGPrimaryKeyManager* nKeyManager, char** ppszClassifyFields, 
		int nClassifyFieldsSize, char* szCaptionField, char* szPrimaryKeyField, 
		CImageList* pImageList, Funtion GetImageIndex
	);

	int Update(int* pnLoadNumber = NULL);
	
	virtual int BeginStick(HTREEITEM hSour, HTREEITEM hDest, MOVE_TYPE eCopyType);
	virtual int AfterStick(HTREEITEM hNewItem, MOVE_TYPE eCopyType);
	virtual int BeginDelItem(HTREEITEM hItem);
	virtual int AfterDelItem(HTREEITEM hNextItem);
	virtual int AfterCopyItem(
		HTREEITEM hCopyItem,   HTREEITEM hNewItem,
		HTREEITEM hParentItem, HTREEITEM hAfterItem
	);
 
	HTREEITEM GetInnermostChildItem(HTREEITEM hTreeItem, int nFirstFlag = true);

	// nCallFlag == flase  : Call By GetNextSibingItem(...)
	// nCallFlag == true   : Call By GetChildItem(...)
	int GetInnermostChildItem_Assistant(
		HTREEITEM* phTreeItem, int* pnCounter, HTREEITEM hTreeItem,
		int nLayer, int nCallFlag, int nFirstFlag = true
	); 
	HTREEITEM GetItemByPrimaryKey(HTREEITEM hStartItem, DWORD_PTR nPrimaryKey);
	HTREEITEM GetPrevDataItem(HTREEITEM hTreeItem);
	HTREEITEM GetNextDataItem(HTREEITEM hTreeItem);

	int ItemIsInSubset(HTREEITEM hTreeItem);
	int IsSubsetItem(HTREEITEM hTreeItem);
	int IsDataItem(HTREEITEM hTreeItem);
	int IsClassItem(HTREEITEM hTreeItem);

	int GetItemPos(HTREEITEM hTreeItem);
	int GetCollectivityLastPos(HTREEITEM hTreeItem);
	int GetCollectivityFirstPos(HTREEITEM hTreeItem);

	int CopyOneRow(int nSourRow, int nDestRow);
	int DelRowInTabFile(HTREEITEM hTreeItem);
	int RefrushTypeInfo(HTREEITEM hTreeItem);
	int UpdateInfoRelating(HTREEITEM hTreeItem);
	
	int	FindInsertPosInFile(const CString pcszClassifyField[], int nFieldCount);
	//int FindInSubset(CString szPrimaryKey, CString& szReturnSectionName);

	int EraseItemInSubset(HTREEITEM hTreeItem);

	KGPropertyTreeCtrl();
	~KGPropertyTreeCtrl();

	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

	DECLARE_MESSAGE_MAP()

public :
	KGPrimaryKeyManager* m_pPrimaryManager;

	IIniFile**  m_ppQuestIniFile;
	ITabFile**	m_ppTabFile;
	IIniFile**  m_ppIniFile;

	Funtion     m_fnGetImageIndex;
	HTREEITEM   m_hNextItem;

	CString		m_stringName;

	//int*    m_pnTypeRow;
	//int     m_nTypeRowSize;
	//int     m_nDataRow;
	//int	    m_nTitleRow;

	char**  m_ppszClassifyFields;
	int		m_nClassifyFieldsSize;
	char*	m_pszCaptionField;
	char*   m_pszPrimaryKesField;

public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif // _KGPROPERTYTREECTRL_H_
