// RecordeOperatorDoc.cpp : implementation of the CRecordeOperatorDoc class
//

#include "stdafx.h"
#include "RecordeOperator.h"

#include "RecordeOperatorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRecordeOperatorDoc

IMPLEMENT_DYNCREATE(CRecordeOperatorDoc, CDocument)

BEGIN_MESSAGE_MAP(CRecordeOperatorDoc, CDocument)
END_MESSAGE_MAP()


// CRecordeOperatorDoc construction/destruction

CRecordeOperatorDoc::CRecordeOperatorDoc()
{
	// TODO: add one-time construction code here
	m_lpPlayer = NULL;

}

CRecordeOperatorDoc::~CRecordeOperatorDoc()
{
}

BOOL CRecordeOperatorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

BOOL  CRecordeOperatorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{	
	IERecPlayer* piRecord = static_cast<IERecPlayer*>(g_p3DEngine->GetNewRecPlayer());
	//g_p3DEngine->NewOneRecord(&piRecord);
	SetRecord(piRecord);
	//g_SetRootPath();
	//g_SetFilePath("");
	if (piRecord)
	{
		HRESULT hr = piRecord->LoadRecordFile(lpszPathName);
		if (FAILED(hr))
		{
			MessageBox(NULL, "录像文件读取失败，即将退出","警告",MB_OK | MB_ICONWARNING);
			goto Exit0;
		}
	}
	SetCurrentDirectory(g_szDefWorkDirectory);
	return TRUE;
Exit0:
	return FALSE;
}

// CRecordeOperatorDoc serialization

void CRecordeOperatorDoc::Serialize(CArchive& ar)
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

void CRecordeOperatorDoc::OnCloseDocument()
{
	//m_lpPlayer->UnInit();
	SAFE_DELETE(m_lpPlayer);
	CDocument::OnCloseDocument();
}
// CRecordeOperatorDoc diagnostics

#ifdef _DEBUG
void CRecordeOperatorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRecordeOperatorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CRecordeOperatorDoc commands
