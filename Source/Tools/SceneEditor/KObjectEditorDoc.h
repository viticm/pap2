#if !defined(AFX_KSceneObjectEditorDoc_H__3B76A0C8_1698_4B18_A732_DFAA0930F0B2__INCLUDED_)
#define AFX_KSceneObjectEditorDoc_H__3B76A0C8_1698_4B18_A732_DFAA0930F0B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneObjectEditorDoc.h : header file
//
//#include "./KSceneObjectEditor.h"
#include "KEditorDocumentBase.h"
/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorDoc document

class KSceneObjectEditorDoc : public CDocument , public KEditorDocumentBase
{
protected:
	KSceneObjectEditorDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(KSceneObjectEditorDoc)

// Attributes
public:
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneObjectEditorDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation


public:
	virtual ~KSceneObjectEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(KSceneObjectEditorDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSceneObjectEditorDoc_H__3B76A0C8_1698_4B18_A732_DFAA0930F0B2__INCLUDED_)
