////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFileViewDialog.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-4-9 20:11:15
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DFileViewDialog.h"
#include "KGLog.h"
//#include "KG3DEngineManager.h"
//#include "SceneEditorCommon.h" //by dengzhihui Refactor 2006年11月6日

//by dengzhihui Refactor 2006年11月15日

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IKG3DScene* GetBaseScene(IEKG3DSceneObjectEditor* p)
{
	_ASSERTE(p);

	IKG3DScene* pRet = NULL;
	p->GetBaseScene(&pRet);
	_ASSERTE(pRet);

	return pRet;
}

KG3DFileViewDialog::KG3DFileViewDialog(
	BOOL bOpenFile,  LPCTSTR szModelFileName, LPCTSTR szAnimaFileName,
	LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
	DWORD dwFlags,LPCTSTR lpszFilter, CWnd* pParentWnd, DWORD dwSize
)
: CFileDialog(bOpenFile, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, dwSize)
{
	m_szModelFileName = szModelFileName;
	m_szAnimaFileName = szAnimaFileName;
	m_lpScene         = NULL;
}

KG3DFileViewDialog::~KG3DFileViewDialog(void)
{
	//g_pEngineManager->RemoveScene(m_lpScene);
	IKG3DScene* pScene = NULL;
	HRESULT hr = m_lpScene->GetBaseScene(&pScene);
	KGLOG_PROCESS_ERROR(pScene);
	g_pEngineManager->RemoveScene(pScene);//by dengzhihui Refactor 2006年11月6日
Exit0:
	return;
}

IMPLEMENT_DYNAMIC(KG3DFileViewDialog, CFileDialog)
BEGIN_MESSAGE_MAP(KG3DFileViewDialog, CFileDialog)
END_MESSAGE_MAP()

BOOL KG3DFileViewDialog::OnInitDialog()
{
	int nResult  = false;
	int nRetCode = false;

	int nOutputWindowID = 1;

	CWnd* pParentWnd = NULL;
	CRect rectMain;
	CRect rectClin;

	nRetCode = CFileDialog::OnInitDialog();
	KGLOG_PROCESS_ERROR(nRetCode);

	pParentWnd = GetParent();
	KGLOG_PROCESS_ERROR(pParentWnd);

	pParentWnd->GetWindowRect(rectMain);
	pParentWnd->GetClientRect(rectClin);

	rectMain.right += 260;
	pParentWnd->MoveWindow(&rectMain);

	rectClin.left    = rectClin.right;
	rectClin.right   = rectClin.right + 255;
	rectClin.top     = 36;
	rectClin.bottom  = 290;

	m_wndModelView.Create(
		NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, rectClin, pParentWnd, 100
	);
	SetClassLongPtr(m_wndModelView.m_hWnd, GCL_HBRBACKGROUND, (LONG)(LONG_PTR)CreateSolidBrush(0x0));

	rectClin.top    = rectClin.bottom + 4;
	rectClin.bottom = rectClin.bottom + 20;
	m_staticModel.Create(
		"当前模型 : ", WS_CHILD | WS_VISIBLE, rectClin, pParentWnd, 200
	);
	m_staticModel.SetFont(GetFont());

	rectClin.top    = rectClin.bottom + 4;
	rectClin.bottom = rectClin.bottom + 20;
	m_staticAnima.Create(
		"当前动画 : ", WS_CHILD | WS_VISIBLE, rectClin, pParentWnd, 300
	);
	m_staticAnima.SetFont(GetFont());

	g_pEngineManager->NewOneScene(SCENETYPE_OBJECTEDITOR,(IKG3DScene**)&m_lpScene);//by dengzhihui Refactor 2006年11月6日
	KGLOG_PROCESS_ERROR(m_lpScene);

	IKG3DScene* pScene = GetBaseScene(m_lpScene);

	g_pEngineManager->SetFocusScene(pScene);//by dengzhihui Refactor 2006年11月6日
	pScene->AddOutputWindow(
		_T("MODEL VIEW"), m_wndModelView.m_hWnd, OUTPUTWND_OBJECTEDITOR_MAIN, 0,
		&nOutputWindowID
	);
	g_pEngineManager->SetFocusScene(pScene);//by dengzhihui Refactor 2006年11月6日
	pScene->OnResizeWindow(NULL);

	m_lpScene->OpenObject((LPSTR)m_szModelFileName);
	m_lpScene->PlayAnimation((LPSTR)m_szAnimaFileName);

	SetWindowText(_T("很牛X的打开文件对话框 >_<"));

	nResult = true;
Exit0:
	return nResult;
}

void KG3DFileViewDialog::OnFileNameChange()
{
	CString szFileName = GetPathName();
	if (szFileName.Right(5) == _T(".Mesh") || szFileName.Right(5) == _T("mesh"))
	{
		if (m_lpScene)
		{
			m_lpScene->OpenObject(szFileName.GetBuffer());
			m_szModelFileName = szFileName;
			m_staticModel.SetWindowText(CString("当前模型 : ") + GetFileName());
		}
	}
	if (szFileName.Right(4) == _T(".Ani") || szFileName.Right(4) == _T("ani"))
	{
		if (m_lpScene)
		{
			m_lpScene->PlayAnimation(szFileName);
			m_szAnimaFileName = szFileName;
			m_staticAnima.SetWindowText(CString("当前动画 : ") + GetFileName());
		}
	}
}

LPCTSTR KG3DFileViewDialog::GetModelFilePath()
{
	return m_szModelFileName;
}

LPCTSTR KG3DFileViewDialog::GetAnimaFilePath()
{
	return m_szAnimaFileName;
}
