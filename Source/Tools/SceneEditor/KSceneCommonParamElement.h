////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneCommonParamElement.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-2 20:22:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _INCLUDE_KSCENECOMMONPARAMELEMENT_H_
#define _INCLUDE_KSCENECOMMONPARAMELEMENT_H_

////////////////////////////////////////////////////////////////////////////////




// KSceneCommonParamElement dialog
class KSceneCommonParamElement : public CDialog
{
	DECLARE_DYNAMIC(KSceneCommonParamElement)

public:
	KSceneCommonParamElement(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneCommonParamElement();

// Dialog Data
	enum { IDD = IDD_DLG_COMMON_PARAM_ELEMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	DWORD m_dwType;
	BOOL m_bHasChanged;
	BOOL m_bHasInit;
	BOOL m_bHasInitialDialog;

	CString m_csData;
	CString m_csTitle;
	//////////////////////////////////////////////////////////////////////////
public:
	DWORD GetType(){return m_dwType;}
	CString& GetData()
	{
		if (this->GetSafeHwnd())
		{
			UpdateData();
		}
		return m_csData;
	}
	CString& GetTitle()
	{
		return m_csTitle;
	}
	BOOL Init(DWORD dwType, CString& csTitle, VOID* pValueBuffer, INT nValueBufferSize);
	BOOL GetData(VOID* pValueBuffer, INT nValueBufferSize, BOOL* bHasChanged);
	BOOL IsSucceed(){ return (m_bHasInitialDialog && m_bHasInit) ? TRUE : FALSE; }
public:
	afx_msg void OnEnChangeEdCommonParam();
	virtual BOOL OnInitDialog();
	afx_msg void OnEnKillfocusEdCommonParam();

	virtual void OnOK( );
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual void EnableEditing();
	BOOL m_bEnableEditing;
};



#endif //_INCLUDE_KSCENECOMMONPARAMELEMENT_H_
