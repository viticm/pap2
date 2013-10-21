#pragma once

#include "KG3DSceneOutputWnd.h"

class KG3DSceneWndManager
{
public:
	typedef list<KG3DSceneOutputWnd*> TypeOutWndContainer;
	TypeOutWndContainer m_listOutputWindows;

	KG3DSceneOutputWnd*  m_lpPrimaryWindow;
public:
	KG3DScene* GetParentScene(){return m_lpParentScene;}

    HRESULT FrameMove();

	IEKG3DSceneOutputWnd* GetOutputWindow(int nID);
    IEKG3DSceneOutputWnd* GetForceWindow();

	int FindOutputWindow(HWND hWnd);
	int FindOutputWindow(TCHAR szName[]);
	int AddOutputWindow(TCHAR szName[],HWND hWnd,DWORD dwType,DWORD_PTR dwOption);
	HRESULT FindFar_Near_Point(D3DXVECTOR3  &Near,D3DXVECTOR3 &Far);
	
	HRESULT RemoveOutputWindow(int nID);

	HRESULT OnResizeWindow(PRECT const pRect);

	//HRESULT GetPickRay(D3DXVECTOR3 * RayOrig,D3DXVECTOR3* RayDir );

	HRESULT SetPrimaryWindow(int nID);
 	HRESULT SetCurrentOutputWindow(int nID);
    HRESULT GetPrimaryWindow(IEKG3DSceneOutputWnd **ppiRetPrimaryWindow);
    HRESULT GetCurOutputWnd(IEKG3DSceneOutputWnd **ppiRetCurOutputWnd);

	KG3DSceneOutputWnd* GetCurOutputWnd(){return m_lpCurOutputWnd;}
	

	KG3DSceneOutputWnd* GetSceneEditorMainOutputWnd(){return m_lpPrimaryWindow;}
public:
	KG3DSceneWndManager(KG3DScene* pScene);
	virtual ~KG3DSceneWndManager(void);
private:
	int m_nIDCount;
	KG3DScene* m_lpParentScene;
	//KG3DSceneOutputWnd* m_lpSceneEditorMainWnd;
	KG3DSceneOutputWnd*  m_lpCurOutputWnd;
};
