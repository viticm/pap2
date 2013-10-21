#pragma once
#include "KG3DSceneOutputWnd.h"

class KG3DOutputWindowTable
{
public:
	HRESULT Init();
	HRESULT UnInit();
	HRESULT Assign(KG3DSceneOutputWnd* pOutputWnd,HWND hWnd);
	HRESULT Recycle(KG3DSceneOutputWnd* pOutputWnd);
private:
	std::map<HWND,KG3DSceneOutputWnd*> m_WindowTable;
};