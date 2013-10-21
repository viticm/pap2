// MeshScreenSFX.cpp: implementation of the KModelSFXScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshScreenSFX.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace ModelSFX
{
KModelSFXScreen::KModelSFXScreen()
{

}

KModelSFXScreen::~KModelSFXScreen()
{

}

HRESULT KModelSFXScreen::LoadMesh(LPSTR pFileName)
{
	wsprintf(m_szMeshFileName,"Screen SFX");

	m_dNumMaterial = 1;
	m_lpMaterial = new MATERIAL[m_dNumMaterial];
	ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);
	
	DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
		MATERIAL_OPTION_FILL_SOLID|
		MATERIAL_OPTION_SHADE_GOURAUD|
		MATERIAL_OPTION_CULL_CW|MATERIAL_OPTION_LIGHTING|
		MATERIAL_OPTION_SPECULARENABLE;
	
	for(DWORD i=0;i<m_dNumMaterial;i++)
	{
		m_lpMaterial[i].m_sMaterial9.Diffuse.r = 0.7f ;
		m_lpMaterial[i].m_sMaterial9.Diffuse.g = 0.7f ;
		m_lpMaterial[i].m_sMaterial9.Diffuse.b = 0.7f ;
		m_lpMaterial[i].m_sMaterial9.Diffuse.a = 1.0f ;
		m_lpMaterial[i].m_sMaterial9.Ambient = m_lpMaterial[i].m_sMaterial9.Diffuse;
		m_lpMaterial[i].m_sMaterial9.Specular = m_lpMaterial[i].m_sMaterial9.Diffuse;
		m_lpMaterial[i].m_sMaterial9.Power = 15;
		m_lpMaterial[i].m_dOption = Def_Option;
	}

	TCHAR Name[256];
	wsprintf(Name,"%s\\ScreenSFX.Mtl",g_Def_ModelDirectory);
	LoadMaterial(Name);

	//m_lpMaterial[0].m_dTextureIDs[0] = g_cGraphicsTool.m_BufferTextureID;
	return S_OK;
}

HRESULT KModelSFXScreen::Render()
{
	KModel::SetMaterial(0);
	// Draw a big, gray square
	D3DXVECTOR2 A(0,0);
	float f = sinf(timeGetTime()*0.003f)*1.1f+1.0f;

	if (f>1)
		f = 1;
	
	D3DXVECTOR2 C(1024,768*f*0.5f);

	D3DXVECTOR2 A1(0,768);
	D3DXVECTOR2 C1(1024,768*(1-f*0.5f));

	D3DCOLOR Color = ColorValue2ref(m_lpMaterial[0].m_sMaterial9.Diffuse);
	DWORD Alpha = ((DWORD)(m_lpMaterial[0].m_sMaterial9.Diffuse.a*255))<<24;
	Color |= Alpha;

	g_cGraphicsTool.DrawScreenRectNormal(&A ,&C ,0,Color,Color,Color,Color);
	g_cGraphicsTool.DrawScreenRectNormal(&A1,&C1,0,Color,Color,Color,Color);

	return S_OK;
}
};
