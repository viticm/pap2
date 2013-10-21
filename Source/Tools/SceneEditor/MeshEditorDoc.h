#pragma once

#include "KEditorDocumentBase.h"
//#include "kg3dscenemodeleditor.h"

interface IEKG3DScene;
class KSceneModelEditorDoc : public COleDocument, public KEditorDocumentBase
{
	DECLARE_DYNCREATE(KSceneModelEditorDoc)

public:
	HRESULT SetPanelContainer(IEKG3DModel *pModel);//Add by huangjinshou 2008 1-15
	BOOL OnOpenDocument(LPCTSTR lpszPathName);
	KSceneModelEditorDoc();
	virtual ~KSceneModelEditorDoc();
	virtual void Serialize(CArchive& ar);   // ÎªÎÄµµ I/O ÖØÐ´
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPhysiscsetting();
	virtual void OnCloseDocument();
	HRESULT SetScene(IEKG3DScene* pScene);
	HRESULT GetCurrentModel(IEKG3DModel **ppiModel);
	HRESULT GetAllModels(vector<IEKG3DModel*>*pvecModel);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	afx_msg void OnReloadmesh();
};
 