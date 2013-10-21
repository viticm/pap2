////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingPageEnvironment.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-2-26 11:12:30
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "afxwin.h"
#ifndef _INCLUDE_KSCENESETTINGPAGEENVIRONMENT_H_
#define _INCLUDE_KSCENESETTINGPAGEENVIRONMENT_H_
#include "IEKG3DEnvironment.h"
#include "KSceneSettingPageBase.h"
////////////////////////////////////////////////////////////////////////////////



// KSceneSettingPageEnvironment dialog
class IEKG3DScene;
class IEKG3DEnvironment;
class IEKG3DEnvEffect;
class KSceneSettingPageEnvironment : public KSceneSettingPageBase
{
	DECLARE_DYNAMIC(KSceneSettingPageEnvironment)

public:
	KSceneSettingPageEnvironment();
	virtual ~KSceneSettingPageEnvironment();

// Dialog Data
	enum { IDD = IDD_SCENESETTING_ENVIRONMENT };
private:	
	vector<KG3DENVEFF_HANDLE> m_EnvEffDescArray;
public:
	VOID ReloadList();
	//VOID UpdataInterface(BOOL bToInterface = TRUE);
	virtual HRESULT SynFromInnerToInterface(DWORD_PTR dwOption = 0);	
	virtual HRESULT SynFromInterfaceToInner(DWORD_PTR dwOption = 0);
	IEKG3DEnvEffect* GetSelectedEffect();
	INT AddEnvEffectToList(IEKG3DEnvEffect* pEnvEffect, KG3DENVEFF_HANDLE hHandle, INT hItem);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_EffList;
	
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnLbnSelchangeListScenesettingEnv();
	CButton m_bnIsEnable;
	afx_msg void OnBnClickedCkEnableScenesettingEnv();
	afx_msg void OnBnClickedBnAddScenesettingEnv();
	afx_msg void OnBnClickedBnDelScenesettingEnv2();
	afx_msg void OnBnClickedCkSsEnableControlByDw();
	afx_msg void OnBnClickedSaveAs();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedSave();
	CButton m_cbControlByDW;
};


#endif //_INCLUDE_KSCENESETTINGPAGEENVIRONMENT_H_
