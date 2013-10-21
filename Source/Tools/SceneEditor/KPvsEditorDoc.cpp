// KPvsEditorDoc.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KPvsEditorDoc.h"
#include "IEKG3DSceneSFXEditor.h"
#include "IEEditor.h"


// KPvsEditorDoc

IMPLEMENT_DYNCREATE(KPvsEditorDoc, CDocument)

KPvsEditorDoc::KPvsEditorDoc()
    : m_pPvsScene(NULL),
      m_pModifyPvs(NULL)
{
}

BOOL KPvsEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

    ASSERT(m_pPvsScene == NULL);

    IEKG3DScene* pScene = NULL;

    g_pEngineManager->NewOneScene(SCENETYPE_PVSEDITOR, (IKG3DScene**)&pScene);
    g_pEngineManager->SetFocusScene(pScene);

    m_pPvsScene = pScene->GetScenePvsEditor();

    if (!m_pPvsScene)
        return FALSE;

    m_pPvsScene->NewPvs();

    SetScene(m_pPvsScene->GetScene());

	return TRUE;
}

KPvsEditorDoc::~KPvsEditorDoc()
{

}


BEGIN_MESSAGE_MAP(KPvsEditorDoc, CDocument)
END_MESSAGE_MAP()


// KPvsEditorDoc diagnostics

#ifdef _DEBUG
void KPvsEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void KPvsEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// KPvsEditorDoc serialization

void KPvsEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
#endif


BOOL KPvsEditorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    ASSERT(m_pPvsScene == NULL);

    IEKG3DScene* pScene = NULL;

    g_pEngineManager->NewOneScene(SCENETYPE_PVSEDITOR, (IKG3DScene**)&pScene);
    g_pEngineManager->SetFocusScene(pScene);

    m_pPvsScene = pScene->GetScenePvsEditor();

    if (!m_pPvsScene)
        return FALSE;

    m_pPvsScene->OpenPvs(lpszPathName);

    SetScene(m_pPvsScene->GetScene());

    
    return TRUE;
}

BOOL KPvsEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
    if (!m_pPvsScene)
        return FALSE;

    HRESULT hr = E_FAIL;
    TCHAR szRetPaht[MAX_PATH];

    if (g_GetFilePathFromFullPath(szRetPaht, lpszPathName))
        hr = m_pPvsScene->SavePvs(szRetPaht);
    else
        hr = m_pPvsScene->SavePvs(lpszPathName);

    if (SUCCEEDED(hr) && m_pModifyPvs)
    {
        m_pModifyPvs->Clear(FALSE);
        m_pModifyPvs->LoadPvsFile(m_pModifyPvs->GetName(), 0);
    }

    return SUCCEEDED(hr);
}

void KPvsEditorDoc::OnCloseDocument()
{
    if (m_pPvsScene)
    {
        while (true)
        {
            int nOp = AfxMessageBox(TEXT("是否保存当前编辑的pvs文件"), MB_YESNOCANCEL);

            if (nOp == IDYES)
            {
                if (m_strPathName.IsEmpty())
                {
                    TCHAR szFilter[] = TEXT("Pvs Files(*.pvs)|*.pvs||");
                    CFileDialog dlg(FALSE, TEXT("pvs"), TEXT("*.pvs"), OFN_DONTADDTORECENT | OFN_EXPLORER | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT, szFilter);
                    
                    if (dlg.DoModal() == IDOK)
                    {
                        m_strPathName = dlg.GetPathName();
                        OnSaveDocument(m_strPathName);
                        break;
                    }
                }
                else
                {
                    OnSaveDocument(m_strPathName);
                    break;
                }
            }
            else if (nOp == IDCANCEL)
            {
                return;
            }
            else
            {
                break;
            }
        }

        g_pEngineManager->RemoveScene(m_pPvsScene->GetScene());
        m_pPvsScene = NULL;
    }

    CDocument::OnCloseDocument();
}


// KPvsEditorDoc commands
