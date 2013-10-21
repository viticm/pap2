////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGPropertyListCtrl.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-2-7 15:49:38
//  Comment     : 2006-2-24 Update To V2.0
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGPROPERTYLISTCTRL_H_
#define _KGPROPERTYLISTCTRL_H_

#include "KGListCtrl.h"

#define  WM_USER_UPDATE_LIST_ITEM	 WM_USER + 2
#define  WM_LIST_UPDATE_SCENE        WM_USER + 100
#define  WM_NOTIFY_ITEM_VALUE_UPDATE WM_USER + 200

#define  ID_BN_SHOW   10
#define  ID_ED_SHOW   20
#define  ID_SP_SHOW   40
#define  ID_CB_SHOW	  50
class IEKG3DScene;
enum ITEM_VILUE_TYPE
{
	// Current 
	VALUE_STRING,			// String 
	VALUE_INT,				// Int
	VALUE_FLOAT,			// Float
	VALUE_BOOL,				// Bool
	VALUE_COLOR,			// Color
	VALUE_FILE,				// File Path

	// ex
	VALUE_IDS,				// IDs this is so cool :)

	// Quest Edit 
	VALUE_ITEM_TID_IID,
	VALUE_ITEM_TID_IID_NUM,
	VALUE_NPC_TEMPLATE,
	VALUE_GOSSIP,
	VALUE_TYPE,				// Quest Type
	VALUE_QUEST_MAPNPC,		// Map Id And NPC Id
	VALUE_MIN_MAX,			// Min Max Value
	VALUE_DITEM,            // Drop Item
	VALUE_ITEM, 			// Item Type Id And Number
	VALUE_FORCE_ID,			// Force Id
	VALUE_FORCE_IV,			// Force Id And Value
	VALUE_COOLDOWN_ID,		// Cool Down Id
	VALUE_STATE_ID,			// State Id
	VALUE_STATE_IV,			// State Id And Value
	VALUE_LIVE_IV,			// Live Skill Id And Value
	VALUE_SKILL_IV,			// Skill Id And Value
	VALUE_NPC_ID,			// NPC Id
	VALUE_NPC_ID_NUM,		// NPC Id And Value
	VALUE_PREQUEDT_ID,		
	VALUE_OBJ_TEMPLATE_ID,
	VALUE_SPRING_DATE,

	//Npc
	VALUE_4PAIR,
	VALUE_4FILE,
	VALUE_NPC_ANI_SOUND,
	VALUE_NPC_SOCKET,
	VALUE_NPC_NUM_DATA,
	VALUE_NPC_SKILL,
	VALUE_NPC_DROP,
	VALUE_UNKNOW			// 


};

struct USER_ITEM_DATA
{
	char szPropertyName[128];         // Property Name
	char szPropertyValue[1024];       // Property Value

	char szParamName_1[128];
	char szParamName_2[128];
	char szParamName_3[128];
	char szParamName_4[128];
	char szParamName_5[128];
	char szParamName_6[128];
	char szParamName_7[128];
	char szParamName_8[128];
	char szParamName_9[128];

	char szFileFiter[128];
	
	DWORD   dwValueType;             

	int     nLength;				  
	int     nMin;					 
	int     nMax;					  	
	int     nCount;					 

	int     nNeedFlag;				
	int     nEditFlag;					

	int     nRowsIndexInFile;

	USER_ITEM_DATA()
	{
		*szPropertyValue = '\0';
		*szPropertyName  = '\0';

		*szParamName_1   = '\0';
		*szParamName_2	 = '\0';
		*szParamName_3	 = '\0';
		*szParamName_4	 = '\0';
		*szParamName_5	 = '\0';
		*szParamName_6	 = '\0';
		*szParamName_7	 = '\0';
		*szParamName_8   = '\0';
		*szParamName_9   = '\0';

		*szFileFiter  	 = '\0';

		dwValueType      = VALUE_UNKNOW;
		nLength			 = 256;
		nMin		     = INT_MIN;
		nMax			 = INT_MAX;
		nCount			 = 0;

		nNeedFlag		 = false;
		nEditFlag		 = true;

		nRowsIndexInFile = 0;
	}
};

enum LIST_EDIT_TYPE
{
	LIST_EDIT_TYPE_POPUP,
	LIST_EDIT_TYPE_BAND
};

class KGPropertyListCtrl : public KGListCtrl
{
public:
	CArray<CString, CString&> m_arrayFindString;
	LPKGLISTITEM	m_lpCopyItem;					
	ITabFile**		m_ppTabFile;			
	IIniFile**		m_ppIniFile;
	int				m_nCurRowInTabFile;
	int				m_nEditFlag;
	DWORD           m_eEditType;

	CButton			m_bnEditShow;
	CEdit			m_editEdit;
	CSpinButtonCtrl m_spinCtrl;

	CComboBox		m_comboxCtrl;
	CString			m_pszState[3];

	ITEM_VILUE_TYPE m_eSaveVauleType;	
	int				m_nBandDataFlag;
	LPKGLISTITEM    m_lpSaveItem;

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	int BandData(ITabFile** ppTabFile, IIniFile** ppIniFile, DWORD eEditType = LIST_EDIT_TYPE_POPUP);
	int UpdateProperty(
		const char szPrimaryKeyName[], int nPrimaryKeyValue, int* pnLoadNumber = NULL
	);
	int UpdateProperty(int nCurrentRow, int* pnLoadNumber = NULL);
	int EnableEdit(int nCanEditFlag = true);
	static int IsClassifySection(IIniFile* pIniFile,const char pcszSectionName[]); 
	static ITEM_VILUE_TYPE GetKeyType(CString szKeyType);
	LPKGLISTITEM FindItemByPropertyName(LPKGLISTITEM pStartItem, CString szPropertyName);
	void SetScene(LPVOID pScene);
protected:
	int FindIniSection(CString szSectionName);
	int InsertProperty(
		LPKGLISTITEM pDependedItem, char pcszSectionName[], int* pnInsertNumber, int nCurrentRow
	);
	int RetrievesItemData(LPKGLISTITEM pItem);

public :
	int ModifyNpcSkill(ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pItem, CWnd* pParent);
	int ModifyNpcDrops(ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pItem, CWnd* pParent);

public:
	KGPropertyListCtrl(void);
	~KGPropertyListCtrl(void);
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBnClick();
	afx_msg LRESULT OnUpdateListItem(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	IEKG3DScene* m_pScene;
};

#endif // _KGPROPERTYLISTCTRL_H_