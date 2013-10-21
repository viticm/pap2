// KModelWater.h: interface for the KModelWater class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KMODELWATER_H__F7683E27_0EAF_44A3_BA10_7848E56260C6__INCLUDED_)
#define AFX_KMODELWATER_H__F7683E27_0EAF_44A3_BA10_7848E56260C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "XMesh.h"
#include "Sea.h"
#include "Water.h"

namespace Water
{
class KModelWater : public KModel  
{
public:
	LPD3DXMESH  m_pWaterUp;//水面上层
	LPD3DXMESH  m_pWaterDn;//水面下层（颜色比较深）

	DWORD m_dwNumPoly_X;
	DWORD m_dwNumPoly_Z;
	Sea WSea;         //海浪
	CWater Water;     //水纹
	CRipple Ripple;   //涟漪
	IDirect3DIndexBuffer9  *m_pibIndices;
//	LPD3DXEFFECT           m_pEffect;

	KAnimateTexture*     m_lpBumpSpr;

	LPDIRECT3DTEXTURE9   m_pBumpMapTexture;  //水面bump的纹理
	LPDIRECT3DTEXTURE9     m_pWaterTex;      //水面纹理
	LPDIRECT3DCUBETEXTURE9 m_pSkyCubeTex;    //天空盒纹理
public:
	HRESULT LoadMesh(LPSTR pFileName);
	HRESULT FrameMove();
	HRESULT Render();
	HRESULT CleanUp();
	HRESULT InitBumpMap();


	KModelWater();
	virtual ~KModelWater();

};
};

#endif // !defined(AFX_KMODELWATER_H__F7683E27_0EAF_44A3_BA10_7848E56260C6__INCLUDED_)
