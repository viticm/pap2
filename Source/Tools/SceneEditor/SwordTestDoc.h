#pragma once

//#include ".\sceneswordtest.h"
#include "KEditorDocumentBase.h"

// KSwordTestDoc 文档

class KSwordTestDoc : public CDocument , public KEditorDocumentBase
{
	DECLARE_DYNCREATE(KSwordTestDoc)

public:
	//HRESULT SetScene(KSceneSwordTest *pScene);

	//KSceneSwordTest* m_lpSwordTest;

	KSwordTestDoc();
	virtual ~KSwordTestDoc();
	virtual void Serialize(CArchive& ar);   // 为文档 I/O 重写
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
