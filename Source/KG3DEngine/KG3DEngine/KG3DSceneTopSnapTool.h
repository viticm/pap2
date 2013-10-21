////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneTopSnapTool.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-13 16:47:33
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSCENETOPSNAPTOOL_H_
#define _INCLUDE_KG3DSCENETOPSNAPTOOL_H_

#include "KG3DCommonObject.h"
#include "KG3DScene.h"
////////////////////////////////////////////////////////////////////////////////
//1.添加到Scene
//2.写算法
class KG3DSceneTopSnapTool : public KG3DCommonObjectSimple
{
public:
	//<KG3DCommonObject>
	STDMETHOD_(ULONG, GetType)();	//必须实现
	static KG3DCommonObject* Build(LPCTSTR lpFile, DWORD_PTR wParam, DWORD_PTR);
	
#if defined(DEBUG) | defined(_DEBUG)
protected:
#else
private:
#endif
	STDMETHOD(Render)();
	STDMETHOD(FrameMove)();
	//</KG3DCommonObject>
	KG3DSceneTopSnapTool(KG3DScene& Scene);
private:
	KG3DScene& m_Scene;
};

#endif //_INCLUDE_KG3DSCENETOPSNAPTOOL_H_
