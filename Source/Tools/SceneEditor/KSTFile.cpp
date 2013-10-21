#include "stdafx.h"
#include "particlesystem.h"
#include "kstfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


extern LPDIRECT3DDEVICE9  g_pd3dDevice;
extern KGraphicsTool      g_cGraphicsTool;
extern D3DCAPS9           g_D3DCaps;
extern HWND               g_hBaseWnd;

extern TCHAR              g_Def_AppDirectory[256];
extern TCHAR              g_Def_ModelDirectory[256];
extern TCHAR              g_Def_TextureDirectory[256];
extern TCHAR              g_Def_ModelTextureDirectory[256];
extern IMusicApplication  *g_pSoundSys;

KSTBranch::KSTBranch()
{
	Clear();
}

KSTBranch::~KSTBranch()
{
}

void KSTBranch::Clear()
{
	m_Texture = 0;
	m_Bone = 0;
	D3DXMatrixIdentity(&m_CurrMatrix);
	m_Center.x = 0;
	m_Center.y = 0;
	m_Center.z = 0;
	m_Angle = 0;
	m_Velocity = 0;
	m_VertexOffset = 0;
}

//void KSTBranch::Transform(const Maths::Matrix& mtx)
//{
//	Maths::Vector3F v;
//
//	BranchVertex* pVB = 0;
//	m_Vertices->Lock(0, 0, (void**)&pVB, 0);
//	v.x = pVB->x;
//	v.y = pVB->y;
//	v.z = pVB->z;
//	v = v * mtx;
//	pVB->x = v.x;
//	pVB->y = v.y;
//	pVB->z = v.z;
//
//	v.x = (pVB + 1)->x;
//	v.y = (pVB + 1)->y;
//	v.z = (pVB + 1)->z;
//	v = v * mtx;
//	(pVB + 1)->x = v.x;
//	(pVB + 1)->y = v.y;
//	(pVB + 1)->z = v.z;
//
//	v.x = (pVB + 2)->x;
//	v.y = (pVB + 2)->y;
//	v.z = (pVB + 2)->z;
//	v = v * mtx;
//	(pVB + 2)->x = v.x;
//	(pVB + 2)->y = v.y;
//	(pVB + 2)->z = v.z;
//
//	v.x = (pVB + 3)->x;
//	v.y = (pVB + 3)->y;
//	v.z = (pVB + 3)->z;
//	v = v * mtx;
//	(pVB + 3)->x = v.x;
//	(pVB + 3)->y = v.y;
//	(pVB + 3)->z = v.z;
//	m_Vertices->Unlock();
//
//	m_Position = m_Position * mtx;
//}

void KSTBranch::SetDiffuse(D3DCOLOR diffuse, BranchVertex* pVB)
{
	if(!pVB)
	{
		return;
	}
	BranchVertex* pMyVB = pVB + m_VertexOffset;

	pMyVB->diffuse = diffuse;
	(pMyVB + 1)->diffuse = diffuse;
	(pMyVB + 2)->diffuse = diffuse;
	(pMyVB + 3)->diffuse = diffuse;
}

void KSTBranch::SetDimensions(float Width, float Height, BranchVertex* pVB)
{
	if(!pVB)
	{
		return;
	}
	BranchVertex* pMyVB = pVB + m_VertexOffset;

	m_Width = Width;
	m_Height = Height;
	float hw = Width * 0.5f;
	float hh = Height * 0.5f;

	pMyVB->x = -hw;
	pMyVB->y = hh;
	pMyVB->z = 0;
	(pMyVB + 1)->x = hw;
	(pMyVB + 1)->y = hh;
	(pMyVB + 1)->z = 0;
	(pMyVB + 2)->x = hw;
	(pMyVB + 2)->y = -hh;
	(pMyVB + 2)->z = 0;
	(pMyVB + 3)->x = -hw;
	(pMyVB + 3)->y = -hh;
	(pMyVB + 3)->z = 0;


	int iv = (float)ModelSFX::GetRandom(0, 3);
	switch(iv)
	{
	case 0:
		{
			pMyVB->u = 0; pMyVB->v = 0;
			(pMyVB + 1)->u = 0.5; (pMyVB + 1)->v = 0;
			(pMyVB + 2)->u = 0.5; (pMyVB + 2)->v = 0.5;
			(pMyVB + 3)->u = 0; (pMyVB + 3)->v = 0.5;
		}
		break;

	case 1:
		{
			pMyVB->u = 0.5; pMyVB->v = 0;
			(pMyVB + 1)->u = 1; (pMyVB + 1)->v = 0;
			(pMyVB + 2)->u = 1; (pMyVB + 2)->v = 0.5;
			(pMyVB + 3)->u = 0.5; (pMyVB + 3)->v = 0.5;
		}
		break;

	case 2:
		{
			pMyVB->u = 0.5; pMyVB->v = 0.5;
			(pMyVB + 1)->u = 1; (pMyVB + 1)->v = 0.5;
			(pMyVB + 2)->u = 1; (pMyVB + 2)->v = 1;
			(pMyVB + 3)->u = 0.5; (pMyVB + 3)->v = 1;
		}
		break;

	case 3:
		{
			pMyVB->u = 0; pMyVB->v = 0.5;
			(pMyVB + 1)->u = 0.5; (pMyVB + 1)->v = 0.5;
			(pMyVB + 2)->u = 0.5; (pMyVB + 2)->v = 1;
			(pMyVB + 3)->u = 0; (pMyVB + 3)->v = 1;
		}
		break;
	}
}