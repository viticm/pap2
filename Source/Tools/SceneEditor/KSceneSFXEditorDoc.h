////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorDoc.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-18 17:12:07
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESFXEDITORDOC_H_
#define _INCLUDE_KSCENESFXEDITORDOC_H_

////////////////////////////////////////////////////////////////////////////////
#include "afxwin.h"

interface IEKG3DSceneSFXEditor;
class KSceneSFXEditorDoc :
	public CDocument
{
	DECLARE_DYNCREATE(KSceneSFXEditorDoc)
public:
	KSceneSFXEditorDoc(void);
	virtual ~KSceneSFXEditorDoc(void);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
    HRESULT GetSceneSFXEditor(IEKG3DSceneSFXEditor **ppiRetSceneSFXEditor);

protected:
    IEKG3DSceneSFXEditor* m_lpSceneSFXEditor;
    virtual BOOL OnNewDocument();

};


#endif //_INCLUDE_KSCENESFXEDITORDOC_H_
