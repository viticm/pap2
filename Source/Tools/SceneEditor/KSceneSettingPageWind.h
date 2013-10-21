#pragma once
#ifndef _INCLUDE_KSCENESETTINGPAGEWIND_H_
#define _INCLUDE_KSCENESETTINGPAGEWIND_H_

// KSceneSettingPageWind dialog
#include "resource.h"
//#include "KSceneSettingPageKeyEditBase.h"
#include "afxwin.h"
#include "KSceneSettingPageBase.h"
#include "afxcmn.h"

class KSceneSettingPageWind : public KSceneSettingPageBase
{
	DECLARE_DYNAMIC(KSceneSettingPageWind)

public:
	KSceneSettingPageWind(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSettingPageWind();

	// Dialog Data
	enum { IDD = IDD_SCENESETTING_WIND };

public:
	HRESULT SynFromInnerToInterface(DWORD_PTR dwOption = 0);	
	HRESULT SynFromInterfaceToInner(DWORD_PTR dwOption = 0);

	BOOL UpdateTree();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedUniformWinddir();
	afx_msg void OnBnClickedFreeWinddir();
	afx_msg void OnBnClickedWindWave2EnableBtn();
	afx_msg void OnBnClickedWindWave1EnableBtn();
	afx_msg void OnTimer(UINT_PTR uIDEvent);
	afx_msg void OnBnClickedWindSwayU();
	afx_msg void OnBnClickedWindSwayV();
	afx_msg void OnBnClickedWindSwayW();
	afx_msg void OnTvnSelchangedWindTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAddWind();

private:
	float mWindDirX;
	float mWindDirY;
	float mWindDirZ;
	DWORD m_Sway;
	float mWindAmplitude1;
	float mWindAmplitude2;
	float mWindWaveLen1;
	float mWindWaveLen2;
	float mWindSpeed1;
	float mWindSpeed2;
	float mWindWaveScaleUp1;
	float mWindWaveScaleUp2;
	float mWindWaveScaleDown1;
	float mWindWaveScaleDown2;
	float mWindWaveOffset1;
	float mWindWaveOffset2;
	BOOL  mbEnableWindWave1;
	BOOL  mbEnableWindWave2;
	BOOL  mbEnableDirWind;
	int mWindDegree;

	int mCurrentWindid;
	CTreeCtrl m_Tree;
public:
	afx_msg void OnBnClickedDelWind();
};

#endif