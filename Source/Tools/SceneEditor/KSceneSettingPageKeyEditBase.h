////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingPageKeyEditBase.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-3-20 17:23:37
//  Comment     : 关于修改、编辑EnvEffect和EnvEffect的关键帧的通用界面基类，用于统一处理
//				四个消息，public的类只需把所有CPropertyPage变成KSceneSettingPageKeyEditBase
//				然后写好CollectInterfaceToData和CollectDataToInterface的虚方法就好了。
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESETTINGPAGEKEYEDITBASE_H_
#define _INCLUDE_KSCENESETTINGPAGEKEYEDITBASE_H_
////////////////////////////////////////////////////////////////////////////////
#if 0


// KSceneSettingPageKeyEditBase dialog
struct KG3DENVEFF_HANDLE;
class KSceneSettingPageKeyEditBase : public CPropertyPage
{
	DECLARE_DYNAMIC(KSceneSettingPageKeyEditBase)
private:
	DWORD m_dwType;
public:
	KSceneSettingPageKeyEditBase(int nID, DWORD dwType);
	virtual ~KSceneSettingPageKeyEditBase();
	
	afx_msg void OnBnClickedSsBnModKey();
	afx_msg void OnBnClickedSsBnNoModKey();
	afx_msg void OnBnClickedSsBnUpdate();
	afx_msg void OnBnClickedSsBnCancel();
	afx_msg void OnBnClickedSsBnHelp();
	afx_msg void OnBnClickedSsCkEnableControlByDW();
	afx_msg void OnBnClickedSsCkEnableEnv();
// Dialog Data
	enum { IDD = IDD_SCENESETTING_KEY_EDIT_BASE };
protected:

	virtual BOOL OnBeginKeyEditState();
	virtual BOOL OnEndKeyEditState();
	virtual HRESULT UpdateObject();
	virtual HRESULT	SynFromInterfaceToInner(DWORD_PTR dwOption = 0);
	virtual HRESULT	SynFromInnerToInterface(DWORD_PTR dwOption = 0);
	virtual INT  GetSelectedEnvEffect() = 0;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
private:
	enum
	{
		DISABLE_CWND_COUNT = 12,
		ENABLE_CWND_COUNT = 2,
	};
	CWnd* m_CWndDisableInKeyEdit[DISABLE_CWND_COUNT];
	CWnd* m_CWndEnableInKeyEdit[ENABLE_CWND_COUNT];
	CWnd* m_pCWndTargetTree;
	char	m_cBufferForDescStruct[128];
	//KG3DENVEFF_HANDLE m_EnvEffHandle;
	INT m_nDWGCKeyIndex;
	KG3DENVEFF_HANDLE* GetEnvEffHandleFromParent();
	INT GetEnvEffKeyIndexFromParent();
	BOOL GetIsKeyEditStateFromParent();
	bool m_bHasModInnerState;

	static int m_pEnableInKeyEditStateIDs[];
	static int m_pDisableInKeyEditStateIDs[];
	void CollectChildWindows();
public:
	virtual BOOL OnPageSetActive();
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();	
};

#endif
#endif //_INCLUDE_KSCENESETTINGPAGEKEYEDITBASE_H_
