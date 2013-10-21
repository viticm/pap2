#pragma once
#include "KEditorDocumentBase.h"
// KSceneDataFlowEditorDoc document


class KSceneDataFlowEditorDoc : public CDocument , public KEditorDocumentBase
{
	DECLARE_DYNCREATE(KSceneDataFlowEditorDoc)

public:
	KSceneDataFlowEditorDoc();
	virtual ~KSceneDataFlowEditorDoc();

#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();

	DECLARE_MESSAGE_MAP()
};


