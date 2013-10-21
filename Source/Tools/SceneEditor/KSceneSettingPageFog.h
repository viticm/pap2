////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingPageFog.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-10-25 18:55:04
//  Comment     : Now Maintained by Chentianhong
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESETTINGPAGEFOG_H_
#define _INCLUDE_KSCENESETTINGPAGEFOG_H_

#pragma once
#include "resource.h"
//#include "KSceneSettingPageKeyEditBase.h"
#include "KColorDialog.h"
#include "afxwin.h"
#include "KSceneSettingPageBase.h"

//class KG3DSceneSceneEditor;
interface IEKG3DScene;
interface IEKG3DEnvironment;
interface IEKG3DEnvEffFog;

class KSceneSettingPageFog : public KSceneSettingPageBase//: public KSceneSettingPageKeyEditBase
{
	DECLARE_DYNAMIC(KSceneSettingPageFog)
public:
	KSceneSettingPageFog();
	virtual ~KSceneSettingPageFog();
	enum { IDD = IDD_SCENESETTING_FOG };
	DWORD m_dwModificationOption;

public:
    CComboBox m_ComboxFogFlag;
    CComboBox m_CanDoTableFog;
    CComboBox m_CanDoVertexFog;
    CComboBox m_CanDoWFog;
    CComboBox m_DeviceUsesWFog;
    CComboBox m_RangeBasedFog;
    CComboBox m_UsingTableFog;

    CComboBox m_FogMode;

    float     m_fFogStartValue; 
    float     m_fFogEndValue;    
    float     m_fFogDensity;     

protected:
	virtual void DoDataExchange(CDataExchange* pDX);  

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();

    afx_msg void OnBnClickedButton1();
    virtual BOOL OnInitDialog();
	afx_msg void OnCbnEditchangeFogOpen();
	afx_msg void OnCbnEditchangeFogUw();
	afx_msg void OnCbnEditchangeFogRange();
	afx_msg void OnCbnEditchangeFogUseTable();
	afx_msg void OnCbnEditchangeFogMode();
	afx_msg void OnEnChangeStart();
	afx_msg void OnEnChangeEnd();
	afx_msg void OnEnChangeSitv();
	afx_msg void OnBnClickCheckSynFogColor();
public:
	///下面方法用于基类与界面交互
	virtual HRESULT SynFromInterfaceToInner(DWORD_PTR dwOption = 0);
	virtual HRESULT SynFromInnerToInterface(DWORD_PTR dwOption = 0);
public:
	inline void SynContainedComboBoxWithBOOL(CComboBox& cbTarget, BOOL bData);
	inline void SynBOOLwithContainedComboBox(BOOL& bData, CComboBox& cbTarget);
	KColorPickerButton m_bnColor;
//private:
	//void SynColorElementsWithFogColor( D3DCOLOR FogColor );
};


#endif //_INCLUDE_KSCENESETTINGPAGEFOG_H_
