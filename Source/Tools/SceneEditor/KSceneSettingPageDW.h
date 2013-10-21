////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingPageDW.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-10 9:48:44
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _INCLUDE_KSCENESETTINGPAGEDW_H_
#define _INCLUDE_KSCENESETTINGPAGEDW_H_

////////////////////////////////////////////////////////////////////////////////

#include "afxwin.h"
//#include "KGHelperClasses.h"
#include "KGCommonBuffer.h"
//#include "KGCommonList.h"
#include "KSceneCustomListContrl.h"
#include "KSceneSettingDialogDWGCKeyTrack.h"
#include "KSceneSettingPageBase.h"

interface IEKG3DRegionInfoManager;
typedef struct _KG3DREGION_INFORMER_LOADFILE_PARAM KG3DREGION_INFORMER_LOADFILE_PARAM;
class KSceneSettingDialogDWGCKeyTrack;

class KSceneSettingPageDW : public KSceneSettingPageBase
{
	DECLARE_DYNAMIC(KSceneSettingPageDW)

public:
	KSceneSettingPageDW();
	virtual ~KSceneSettingPageDW();

// Dialog Data
	enum { IDD = IDD_SCENESETTING_DW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int m_nTexID;
	int m_nOutputWndID;
	BOOL m_bShowRegionMap;
	CHAR* m_pLoadParamStructBuffer;
	CString m_csRegionMapPath;
	//////////////////////////////////////////////////////////////////////////
	CButton m_ckShowRegionMap;
	CStatic m_stOutputWnd;
	KSceneSettingDialogDWGCKeyTrack	m_CWndKeyTrack;
	
	typedef KSH::COMMON_CTRL::GetContainerByInterfaceName<IEKG3DRegionInfoManager>	TypeContainerGetter;
	typedef KSH::COMMON_CTRL::GetElemOnceAll<IEKG3DRegionInfoManager, DWORD>	TypeElemGetter;
	typedef KSH::COMMON_CTRL::GetNameByValue<IEKG3DRegionInfoManager, DWORD> TypeNameGetter;

	typedef KSH::COMMON_CTRL::KSceneCustomListContrl<IEKG3DRegionInfoManager, DWORD
		, TypeContainerGetter, TypeElemGetter, TypeNameGetter> TypeList;

	TypeList	m_RegionList;

	KG_CUSTOM_HELPERS::TCommonParamBuffer<KG3DREGION_INFORMER_LOADFILE_PARAM, 69>	m_pLoadParam;
	CEdit m_edTexPath;

	
public:
	BOOL m_bNeedShowRegionMap;
	int m_nFlag; 
	int Update();
	
private:
	BOOL AddOutputTexWnd(INT nTexID);
	VOID RemoveOutputTexWnd();
	void ReloadRegionMap(const CString& csTempString, VOID* pParam, INT nParamSize);
	IEKG3DRegionInfoManager* GetCurRegionInformer(DWORD_PTR* pdwHandle = NULL);
	KG3DREGION_INFORMER_LOADFILE_PARAM* GetLoadParam(IEKG3DRegionInfoManager* pRIM);
	CString	GetLoadPath();
public:
	virtual HRESULT	SynFromInterfaceToInner(DWORD_PTR dwOption = 0);
	virtual HRESULT	SynFromInnerToInterface(DWORD_PTR dwOption = 0);
	HRESULT ShowRegionMap(BOOL bShow = TRUE);
	
public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual BOOL OnInitDialog();
	
	afx_msg void OnBnClickedSsBnReadRegionSplitMap();
	afx_msg void OnBnClickedCkShowRegionSplitMap();
	afx_msg void OnBnClickedSsBnReadRegionSplitMapAgain();	
	afx_msg void OnBnClickedButtonKeyTrack();
};


#endif //_INCLUDE_KSCENESETTINGPAGEDW_H_
