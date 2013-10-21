
#pragma once
#include "IEKG3DScene.h"
#include "resource.h"
#include "afxwin.h"
#include "KColorDialog.h"
#include "IEKG3DMeshSkyBox.h"
#include "KSceneSettingPageBase.h"
//#include "KSceneSettingPageKeyEditBase.h"
//class KG3DSceneSceneEditor;

class KSceneSettingPageSkyBox : public KSceneSettingPageBase//: public KSceneSettingPageKeyEditBase
{
	DECLARE_DYNAMIC(KSceneSettingPageSkyBox)

public:
	KSceneSettingPageSkyBox();
	virtual ~KSceneSettingPageSkyBox();
	enum { IDD = IDD_SCENESETTING_SKY };

public:
    CString m_szPath;
    int     m_nFlag;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
    virtual BOOL OnSetActive();
public:
	KColorPickerButton m_bnTopColor;
	//KColorPickerButton m_bnBotmColor;
	KColorPickerButton m_bnMidColor;
public:
	CString m_czTopPercent;
	CString m_czTHBlendPercent;
	CString m_czMidPercent;
	CString m_czHBBlendPercent;
	CString m_czBotmPercent;
	//<CTH Added>
private:
    KG3DMESHSKYBOX_DESCSTRUCT	m_DlgSkyColorStruct;
	///下面方法用于内部描述天空球贴图的类与界面交互
	virtual HRESULT	SynFromInterfaceToInner(DWORD_PTR dwOption = 0);
	virtual HRESULT	SynFromInnerToInterface(DWORD_PTR dwOption = 0);
private:	 
	//CButton m_ckEnableSynWithFogColor;
	//afx_msg void OnBnClickedCheckSkysphereSynWithFogColor();
	//VOID CheckFogColorSync();
public:
/*
		afx_msg void OnBnClickedSsBnModKey();
		afx_msg void OnBnClickedSsBnNoModKey();*/
	
	afx_msg void OnBnClickedSsBnCancel();
	afx_msg void OnBnClickedSsListBnAdd();
	afx_msg void OnBnClickedSsListBnDel();
	//afx_msg void OnBnClickedBottomColor();
};
