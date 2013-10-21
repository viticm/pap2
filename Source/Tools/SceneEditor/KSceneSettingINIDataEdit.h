////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingINIDataEdit.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-2-27 16:20:11
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _INCLUDE_KSCENESETTINGINIDATAEDIT_H_
#define _INCLUDE_KSCENESETTINGINIDATAEDIT_H_
#include "afxwin.h"

////////////////////////////////////////////////////////////////////////////////

// KSceneSettingINIDataEdit dialog
#define MSG_CHILD_WINDOW_CLOSE (WM_USER+1001)

class KSceneSettingINIDataEdit : public CDialog
{
	DECLARE_DYNAMIC(KSceneSettingINIDataEdit)

public:
	KSceneSettingINIDataEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSettingINIDataEdit();

// Dialog Data
	enum { IDD = IDD_SCENESETTING_INIDATA_DIALOG };
	
	BOOL m_bHasMySelfChanged;
public:
	VOID Init(LPCSTR tczTitle, LPCSTR tczData);
	VOID SetTitle(LPCTSTR tczTitle);
	VOID StringData(LPCTSTR tczData);
	const CString& StringData();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	//CEdit m_DataEdit;
	CString m_csTitle;
	CString m_csEditData;
protected:
	virtual void OnOK();
public:
	afx_msg void OnEnChangeSsEdData();
};


#endif
