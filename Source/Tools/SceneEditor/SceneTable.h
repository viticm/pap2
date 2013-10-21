// SceneTable.h: interface for the KSceneTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENETABLE_H__6534FA05_207A_48BD_A79E_FFAA15688B94__INCLUDED_)
#define AFX_SCENETABLE_H__6534FA05_207A_48BD_A79E_FFAA15688B94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "Scene.h"
//#include "ObjectTable.h"
#include ".\scenemesheditor.h"
#include ".\sceneswordtest.h"
#include "./KSceneObjectEditor.h"

class KSceneTable  
{
private:
	DWORD    m_dMaxNumScene;	
	LPSCENE* m_lpScenes;
public:
	DWORD    m_dNumScene;
	LPSCENE  m_lpCurScene;
	PVOID    m_pManager;
public:
	HRESULT  OnLostDevice(void);
	HRESULT  OnResetDevice(void);

	HRESULT  SetManager(PVOID pManager);
	HRESULT  RefreshCurScene();
	DWORD    m_dCurSceneID;

	HRESULT GetScene(LPSCENE* ppScene,DWORD ID);
	HRESULT Get1NewScene(DWORD Type,LPSCENE* ppScene);
	HRESULT Get1NewScenePointer(LPSCENE** ppScene);

	KSceneTable();
	virtual ~KSceneTable();

	virtual HRESULT CleanUp(void);
};

#endif // !defined(AFX_SCENETABLE_H__6534FA05_207A_48BD_A79E_FFAA15688B94__INCLUDED_)
