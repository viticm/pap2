
#include "IEKG3DSceneSFXEditor.h"
#include "KEditorDocumentBase.h"

class KPvsEditorDoc : public CDocument, public KEditorDocumentBase
{
	DECLARE_DYNCREATE(KPvsEditorDoc)

public :
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
    virtual BOOL OnNewDocument();
    virtual void OnCloseDocument();
    virtual void SetModifyPvs(IEKG3DRepresentPVS* pvs) { m_pModifyPvs = pvs; }

    IEKG3DScenePvsEditor* GetPvsScene() { return m_pPvsScene; }

public :
	KPvsEditorDoc();
	virtual ~KPvsEditorDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	DECLARE_MESSAGE_MAP()


protected :
    IEKG3DScenePvsEditor* m_pPvsScene;
    IEKG3DRepresentPVS*   m_pModifyPvs;
};
