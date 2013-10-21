////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingPageClouds.h
//  Version     : 1.0
//  Creator     : ?
//	Modifier	: Chen Tianhong
//  Create Date : 2006-11-23 11:04:28
//  Comment     : 为了接口化和动态天气，改动了很多。
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESETTINGPAGECLOUDS_H_
#define _INCLUDE_KSCENESETTINGPAGECLOUDS_H_

////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "IEKG3DSkySystem.h"
#include "IEKG3DCloud.h"
#include "KSceneSettingPageBase.h"
#include "KColorDialog.h"
// KSceneSettingPageClouds dialog

class KSceneSettingPageClouds : public KSceneSettingPageBase
{
	DECLARE_DYNAMIC(KSceneSettingPageClouds)

public:
	KSceneSettingPageClouds();
	virtual ~KSceneSettingPageClouds();
	enum { IDD = IDD_SCENESETTING_CLOUD };
   
    virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
    BOOL UpdateTree();

public :
    CTreeCtrl   m_tree;
    CString     m_strText;
    CString     m_strMask;
    FLOAT       m_fHeight;
    FLOAT       m_fSpeed;
    INT         m_nUVRepeat;
    INT         m_nAngel;
    FLOAT       m_fSize;
	FLOAT		m_fVisibility;
    FLOAT       m_fAlphaRef;
    FLOAT       m_fTexFactor;
	KColorPickerButton	m_colorButtonDiffuse;
	KColorPickerButton	m_colorButtonAmbient;
private:
	KG3DCLOUD_DESCSTRUCT	m_DlgStruct;
	TCHAR	m_tszColorTexBuffer[MAX_PATH];
	TCHAR	m_tszMaskTexBuffer[MAX_PATH];
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
private:
	HRESULT SynFromInterfaceToInner(DWORD_PTR dwMDO = 0);
	HRESULT	SynFromInnerToInterface(DWORD_PTR dwMDO = 0);
	IEKG3DCloud* GetValidCurCloudPointer();
	HRESULT	UpdateByStruct(DWORD dwOption = NULL);///如果采取默认参数就不强制制定MDO，由Collect决定
public:
    afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedDel();
    afx_msg void OnBnClickedAdd();
    afx_msg void OnBnClickedCloud();
    afx_msg void OnBnClickedMask();
	afx_msg void OnBnClickedEnableColorControl();
/*
public:
	afx_msg void OnBnClickedSsCloudBnApplymod();*/

};


#endif //_INCLUDE_KSCENESETTINGPAGECLOUDS_H_
