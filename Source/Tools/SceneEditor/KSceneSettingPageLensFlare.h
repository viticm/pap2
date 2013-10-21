////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingPageLensFlare.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-7-24 15:44:19
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESETTINGPAGELENSFLARE_H_
#define _INCLUDE_KSCENESETTINGPAGELENSFLARE_H_
#include "KGCommonBuffer.h"
#include "KSceneSettingPageBase.h"
////////////////////////////////////////////////////////////////////////////////
#pragma once

class IEKG3DMeshLensFlare;
typedef	struct _IEKG3DMeshLensFlareDescStruct	IEKG3DMeshLensFlareDescStruct;
struct LensFlareInfo;
class KSceneSettingPageLensFlare : public KSceneSettingPageBase
{
	DECLARE_DYNAMIC(KSceneSettingPageLensFlare)
private:
	enum{even_id = 1,};

	//只能用于界面交换
	CTreeCtrl   m_tree;
	CString     m_csText;
	int         m_nTexCellNumX;
	int         m_nTexCellNumY;

	FLOAT       m_fSize;
	FLOAT       m_fHeight;
	float		m_fVisibility;
	//BOOL		m_bDrawDummy;
	float		m_fBoundAspect;
	float		m_fDummySize;

	//内部参数
	D3DVIEWPORT9            m_viewPort;

	KG_CUSTOM_HELPERS::TCommonParamBuffer<IEKG3DMeshLensFlareDescStruct, 288>	m_pDescStruct;///这个类是一个仅用前置声明就可以当指针使用的类，内部会自动构建实体和管理Buffer，但需要在cpp中有头文件
	KG_CUSTOM_HELPERS::TCommonParamBuffer<LensFlareInfo, 20>	m_pLensInfo;

	INT m_nWhichLensFlare;///标志当前的LensFlare是哪个
	UINT_PTR m_uTimmer;
	CRect	m_CurSelectRect;

	size_t	m_uWhichItemShouldBeSelected;	//SetItem之后，要更新选择，因为改Height之后，Lens的位置可能会变
	BOOL	m_bItemSelectionPosChanged;
private:
	IEKG3DMeshLensFlare* GetLensFlareInterface();
	void DrawTex();	//画右边的选择框
public:
	KSceneSettingPageLensFlare();
	virtual ~KSceneSettingPageLensFlare();
	enum { IDD = IDD_SCENESETTING_LENSFLARE };

	HRESULT SynFromInnerToInterface(DWORD_PTR dwOption = 0);
	HRESULT SynFromInterfaceToInner(DWORD_PTR dwOption = 0);

    int UpdateSelectionFrame(INT x, INT y);//更新选择框，以渲染出选择框来
    BOOL UpdateTree();///触发OnSelChange会调用SynFromInnerToInterface
private:
	void SynFromInnerToInterface_LensFlareInfo();
protected :
	virtual void DoDataExchange(CDataExchange* pDX);	
	DECLARE_MESSAGE_MAP()
public:
	BOOL OnInitDialog();
    afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR uIDEvent);
    afx_msg void OnBnClickedTexture();
    afx_msg void OnBnClickedAdd();
    afx_msg void OnBnClickedDel();
    afx_msg void OnBnClickedDefult();
	//afx_msg void OnBnClickedSsBnUpdate();
	//afx_msg void OnBnClickedSsBnCancel();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedSsSwitch();

	afx_msg void OnBnClickedSsState1();
	afx_msg void OnBnClickedReloadShader();
};


#endif //_INCLUDE_KSCENESETTINGPAGELENSFLARE_H_
