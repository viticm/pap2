////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingPageFarMount.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-10-28 11:34:17
//  Comment     : 远山卷轴的管理界面，对应IDD_SCENESETTING_FARMOUNT
//                是KSceneSettingDialog的一个子页面
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _INCLUDE_KSCENESETTINGPAGEFARMOUNT_H_
#define _INCLUDE_KSCENESETTINGPAGEFARMOUNT_H_

////////////////////////////////////////////////////////////////////////////////
#include "IEKG3DScene.h"
#include "IEKG3DMeshFarMountain.h"
//#include "afxwin.h"
#include "KColorDialog.h"
#include "KSceneSettingPageBase.h"
//#include "afxcmn.h"
//#include "KSceneSettingPageKeyEditBase.h"
// KSceneSettingPageFarMount dialog

class KSceneSettingPageFarMount : public KSceneSettingPageBase//: public KSceneSettingPageKeyEditBase
{
	DECLARE_DYNAMIC(KSceneSettingPageFarMount)

public:
	KSceneSettingPageFarMount();
	virtual	~KSceneSettingPageFarMount();
	// Dialog Data
	enum { IDD = IDD_SCENESETTING_FARMOUNT };
public:
	int m_nFlag;///决定是否被渲染
	
	int		Update();
	virtual	BOOL OnSetActive();
private:
	///下面方法用于基类与界面交互
	virtual HRESULT	SynFromInterfaceToInner(DWORD_PTR dwOption = 0 );///这个函数比较界面并自己处理ModificationOption
	virtual HRESULT	SynFromInnerToInterface(DWORD_PTR dwOption = 0);
private:
	HRESULT	RestoreDefaultInterface();
	HRESULT	UpdateTree();
	KG3DMESHFARMOUNTAIN_DESCSTRUCT	m_DlgStruct;///用于保存变量
	//TCHAR	m_TexPathBuffer[MAX_PATH];///贴图完全路径，应该通过m_DlgStruct访问
	int		GetSeletedFarMountIndex( IEKG3DMeshFarMountain* pFarMount = NULL );///帮助函数，用于得到当前在Tree中选择的FarMount的是否有效,如果传入指针的话，就可以检查指针是否有效
	IEKG3DMeshFarMountain*	GetValidFarMountPointer( IEKG3DMeshFarMountain* pFarMount = NULL );///会检查指针是否有效，默认从Tree中拿
	//VOID	CheckFogColorSyn(IEKG3DMeshFarMountain* pFarMount);
public:
	HRESULT	UpdateTexture();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// 贴图不完全路径
	CString m_csTexPath;
	// 卷轴半径
	FLOAT m_fRadius;
	// 卷轴高度
	FLOAT m_fHeight;
	// 卷轴的U重复次数
	FLOAT m_fTexU;
	// 卷轴底部高度
	FLOAT m_fBotmZPos;
	// 卷轴Z轴转动
	FLOAT m_fZRotation;
	// 底部渐变高度
	FLOAT m_fBotmBlendHeight;
	// 表面颜色
	//KColorPickerButton m_bnColor;
	// 雾融合因子,0到1
	FLOAT m_fFogBlendFactor;
	// 雾融合颜色
	KColorPickerButton m_bnFogColor;

    FLOAT m_fTexFactor;

	CTreeCtrl m_Tree;
	CButton m_ckSynWithFog;
	CEdit m_edTexPath;
public:
	afx_msg void OnBnClickedSsFarmountBnAdd();
	afx_msg void OnBnClickedSsFarmountBnDel();
	afx_msg void OnBnClickedSsFarmountBnUp();
	afx_msg void OnBnClickedSsFarmountBnDown();
	afx_msg void OnTvnSelchangedSsFarmountTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSsFarmountBnTexpath();
	afx_msg void OnBnClickedSsFarmountBnUpdatemesh();
	afx_msg void OnBnClickedSsFarmountBnUpdateblend();
	afx_msg void OnBnClickedCkEnableColor();
	afx_msg void OnBnClickedSsBnUpdateother();
	//afx_msg void OnBnClickedSurfaceColor();
};


#endif //_INCLUDE_KSCENESETTINGPAGEFARMOUNT_H_
