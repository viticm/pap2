////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFileViewDialog.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-4-9 20:10:03
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DFILEVIEWDIALOG_H_
#define _KG3DFILEVIEWDIALOG_H_

//#include "KG3DSceneObjectEditor.h"
#include "KG3DObjectEditor.h"
#include "IEEditor.h"//(by dengzhihui 2006Äê11ÔÂ15ÈÕ 15:15:27

class KG3DFileViewDialog : public CFileDialog
{
	DECLARE_DYNAMIC(KG3DFileViewDialog)

public :
	KG3DFileViewDialog(
		BOOL bOpenFile = TRUE, LPCTSTR szModelFileName = NULL, LPCTSTR szAnimaFileName = NULL,
		LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL , 
		DWORD dwFlags  = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL, DWORD dwSize = 0 
	);
	virtual ~KG3DFileViewDialog(void);
	virtual void OnFileNameChange();
	BOOL OnInitDialog();

	LPCTSTR GetModelFilePath();
	LPCTSTR GetAnimaFilePath();

protected :
	IEKG3DSceneObjectEditor* m_lpScene;
	LPCTSTR                m_szModelFileName;
	LPCTSTR                m_szAnimaFileName;
	KG3DViewWindow         m_wndModelView;
	CStatic				   m_staticModel;
	CStatic				   m_staticAnima;

public :

	DECLARE_MESSAGE_MAP()
};



#endif //_KG3DFILEVIEWDIALOG_H_
