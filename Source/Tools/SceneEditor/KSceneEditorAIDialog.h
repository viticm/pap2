#pragma once
#include "afxcmn.h"
#include "KSceneEditorDocLogical.h"
#include "IETools.h"

// KSceneEditorAIDialog dialog

class KSceneEditorAIDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneEditorAIDialog)

	enum ImageType
	{
		image_all,
		image_aiset,
		image_npc,
		image_AIPAR,
		image_aivalue,
	};

	struct AISetWithNpc 
	{
		DWORD dwNpcAISet;
		set<KSceneEditorDocLogical::NPCInfo*>m_setNpc;
	};
	map<DWORD,AISetWithNpc>m_mapNpcAiSet;

	struct DataCollection
	{
		set<KSceneEditorDocLogical::NPCInfo*>m_setNpc;
	};

	struct DataSet
	{
		string m_szName;
		map<std::string,DataCollection>m_mapDataCollections;
	};

	vector<DataSet>m_vecDataSet;

	void AnalyseNpcAI();
	void FillMap();

	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;

public:
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;
	}
	KSceneEditorAIDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorAIDialog();

// Dialog Data
	enum { IDD = IDD_SCENEEDITORAISET };
	void SetDocLogical(KSceneEditorDocLogical* pLogical)
	{
		m_lpDocLogical = pLogical;
	}
protected:
	void FillTree();
	void FillList(set<KSceneEditorDocLogical::NPCInfo*>* psetNpc);

	KSceneEditorDocLogical* m_lpDocLogical;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_Tree;
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckPatrol();
	afx_msg void OnBnClickedCheckPatrol2();
	BOOL m_bDataAnalyse;
	afx_msg void OnBnClickedDataswitch();
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
};
