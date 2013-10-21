// MeshParticleSystem.cpp: implementation of the KModelSFXParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshParticleSystem.h"
#include "kforcefield.h"
#include "KFileFinder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define EXPECTED_FPS 30
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern LPDIRECT3DDEVICE9  g_pd3dDevice;
extern KGraphicsTool       g_cGraphicsTool;
extern D3DCAPS9           g_D3DCaps;
extern HWND               g_hRenderWnd;
extern TCHAR    g_Def_AppDirectory[256];
extern TCHAR    g_Def_ModelDirectory[256];
extern TCHAR    g_Def_TextureDirectory[256];
extern TCHAR    g_Def_ModelTextureDirectory[256];
extern int		BaIndex;


extern KFileFinder g_cFileFinder;
namespace ModelSFX
{

KModelSFXParticleSystem::KModelSFXParticleSystem()
{
	m_bDepthSort = TRUE;
}

KModelSFXParticleSystem::~KModelSFXParticleSystem()
{
	
}

HRESULT KModelSFXParticleSystem::Render()
{

	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;

	g_pd3dDevice->SetMaterial(&mtrl);

	//DrawLauncher();
    //DrawBarrier();
    //DrawForceField();
	
	if(!m_lpVBuf)
		return S_OK;

	SetMatrix();

	KModel::SetMaterial(0);

	
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE2X);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	//g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_);
	
	//g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_)
	
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);


	DWORD save[10];
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->GetRenderState(D3DRS_COLORVERTEX, &save[0]);
	g_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
	g_pd3dDevice->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &save[1]);
	g_pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	g_pd3dDevice->SetStreamSource( 0, m_lpVBuf, 0, sizeof( VFormat::FACES_DIFFUSE_TEXTURE1 ) );
	g_pd3dDevice->SetFVF( D3DFVF_Faces_Diffuse_Texture1 );
	
	g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_nNumFaces);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, save[0]);
	g_pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, save[1]);

	RestoreMatrix();
	return S_OK;
}

HRESULT KModelSFXParticleSystem::LoadMesh(LPSTR pFileName)
{
	//////////////////////////////////////////////////////////////////////////
	//
	int iLaucherCount = m_vecLauncher.size();
	DWORD dwActiveParCount = 0;
	DWORD i;
	/*for ( i = 0; i < m_vecLauncher.size(); i++ )
	{
		dwActiveParCount += m_vecLauncher[i]->m_iActivePar;
	}*/
	/*AddVerGroup("粒子系统选项",0);
	for(DWORD L =0;L<m_vecLauncher.size();L++)
	{
		AddVarProperty(0,"发射器位置",PropertyBase::_VarProperty::VAR_POSITION,&m_vecLauncher[L]->m_vPosition);
	}
	for( L =0;L<m_vecForce.size();L++)
	{
		KForceField* FF = m_vecForce[L];
		AddVarProperty(0,"力场位置A",PropertyBase::_VarProperty::VAR_POSITION,&m_vecForce[L]->m_fForcePar[0]);
		AddVarProperty(0,"力场位置B",PropertyBase::_VarProperty::VAR_POSITION,&m_vecForce[L]->m_fForcePar[3]);
	}	*/
	//////////////////////////////////////////////////////////////////////////
	//首先创建运动方式，2种，0为不做任何改变，直接使用粒子的位置
	LPMOTION pMotion = new MOTION;
	m_lpMotion.push_back(pMotion);
	pMotion = new _MotionCircle;
	m_lpMotion.push_back(pMotion);
		

	///添加一些粒子
	//DWORD m_dwNumObject = dwParCount;
	
	/*for ( iIndex = 0; iIndex < dwActiveParCount; iIndex++ )
	{
		LPSPRITE pSpr = new SPRITE;
		memset(pSpr, 0, sizeof(SPRITE));
		KModelSFXBillBoardBase::AddSprite(pSpr);
	}*/

	
	//当改变了粒子的数目后一定要调用这个函数。
	int nNumFacesSave = m_nNumFaces;
	m_nNumFaces = MAX_PARTICLE_COUNT * m_vecLauncher.size() * 2;
	CreateVerticesBuffer();
	m_nNumFaces = nNumFacesSave;

	UpdateVertices();//更新绘制的信息。
	
	
	m_nBillBoardMethod = BILLBOARD_POSITION_ONLY;
	wsprintf(m_szMeshFileName,"Particle System");
	
	//////////////////////////////////////////////////////////////////////////
	//准备一个默认的材质。
	
	m_dNumMaterial = 1;
	m_lpMaterial = new MATERIAL[m_dNumMaterial];
	ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);
	
	DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
		MATERIAL_OPTION_FILL_SOLID|
		MATERIAL_OPTION_SHADE_GOURAUD|
		MATERIAL_OPTION_CULL_CW|
		MATERIAL_OPTION_SPECULARENABLE;
	
	for( i=0;i<m_dNumMaterial;i++)
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
	wsprintf(Name,"%s\\Particle.Mtl",g_Def_ModelDirectory);
	g_cFileFinder.FindFile(Name, Name);
	LoadMaterial(Name);//(LPSTR)m_strMtlName.GetString());

	return S_OK;
}

HRESULT KModelSFXParticleSystem::FrameMove()
{
	int i;
	int iLuancherCnt = m_vecLauncher.size();
	int iSpriteCnt = m_lpSprite.size();
	
	//Update Particles System
	Update();

	int iIndex;
	int iActiveParCount = 0;
	//for ( iIndex = 0; iIndex < iLen; iIndex++ )
	//{
	//	iActiveParCount += m_vecLauncher[iIndex]->m_iActivePar;
	//}

	//int iSpriteInc = iActiveParCount - iSize;
	//
	//for ( iIndex = 0; iIndex < iSpriteInc; iIndex++ )
	//{
	//	SPRITE* pSpr = new SPRITE;
	//	//memset(pSpr, 0, sizeof(SPRITE));
	//	AddSprite(pSpr);
	//}

	//int iCount = 0;
	//for (iIndex = 0; iIndex < iLen; iIndex++ )
	//{
	//	int iSize = m_vecLauncher[iIndex]->m_iActivePar;
	//	for( i = 0; i< iSize; i++)
	//	{
	//		LPSPRITE &pSprite = m_lpSprite[i + iCount];
	//		pSprite->Position = m_vecLauncher[iIndex]->m_vecParticles[i]->m_vPosition + 
	//			                m_vecLauncher[iIndex]->m_vecParticles[i]->m_vDiffuse; 
	//		pSprite->Width = pSprite->Height = m_vecLauncher[iIndex]->m_vecParticles[i]->m_fRadius;
	//		pSprite->Color = m_vecLauncher[iIndex]->m_vecParticles[i]->m_cColor;
	//		pSprite->Rotation = m_vecLauncher[iIndex]->m_vecParticles[i]->m_fRot;

	//	}
	//	iCount += iSize;
	//}

	for ( iIndex = 0; iIndex < iLuancherCnt; iIndex++ )
	{
		iActiveParCount += m_vecLauncher[iIndex]->m_iActivePar;
	}

	int iSpriteInc = iActiveParCount - iSpriteCnt;

	for ( iIndex = 0; iIndex < iSpriteInc; iIndex++ )
	{
		SPRITE* pSpr = new SPRITE;
		//memset(pSpr, 0, sizeof(SPRITE));
		AddSprite(pSpr);
	}
	
	int iCount = 0;
    for (iIndex = 0; iIndex < iLuancherCnt; iIndex++ )
	{
		int iSizeIndex[2];
		int iPerLaunSprCnt = m_vecLauncher[iIndex]->m_iActivePar;
		switch(m_vecLauncher[iIndex]->m_RenderType)
		{
		case PR_SPRITES:
			{
				iSizeIndex[0] = 0;
				iSizeIndex[1] = 1;
				for( i = 0; i< iPerLaunSprCnt; i++)
				{
					LPSPRITE &pSprite = m_lpSprite[i + iCount];
					pSprite->RenderType = m_vecLauncher[iIndex]->m_RenderType;
					pSprite->Position = m_vecLauncher[iIndex]->m_pParticles[i].m_vPosition;// + m_vecLauncher[iIndex]->m_pParticles[i].m_vDiffuse; 
					pSprite->Height = m_vecLauncher[iIndex]->m_pParticles[i].m_fRadius[iSizeIndex[0]];
					pSprite->Width = m_vecLauncher[iIndex]->m_pParticles[i].m_fRadius[iSizeIndex[1]];
					pSprite->Color = m_vecLauncher[iIndex]->m_pParticles[i].m_cColor;
					pSprite->Rotation = m_vecLauncher[iIndex]->m_pParticles[i].m_fRot;
				}
			}
			break;
		case PR_NUMERIC:
		case PR_POINTS:
			{
				iSizeIndex[0] = iSizeIndex[1] = 0;
				for( i = 0; i< iPerLaunSprCnt; i++)
				{
					LPSPRITE &pSprite = m_lpSprite[i + iCount];
					pSprite->RenderType = m_vecLauncher[iIndex]->m_RenderType;
					pSprite->Position = m_vecLauncher[iIndex]->m_pParticles[i].m_vPosition;// + m_vecLauncher[iIndex]->m_pParticles[i].m_vDiffuse; 
					pSprite->Height = m_vecLauncher[iIndex]->m_pParticles[i].m_fRadius[iSizeIndex[0]];
					pSprite->Width = m_vecLauncher[iIndex]->m_pParticles[i].m_fRadius[iSizeIndex[1]];
					pSprite->Color = m_vecLauncher[iIndex]->m_pParticles[i].m_cColor;
					pSprite->Rotation = m_vecLauncher[iIndex]->m_pParticles[i].m_fRot;
				}
			}
			break;
		case PR_STREAK:
			{
				for( i = 0; i< iPerLaunSprCnt; i++)
				{
					LPSPRITE &pSprite = m_lpSprite[i + iCount];
					pSprite->RenderType = m_vecLauncher[iIndex]->m_RenderType;
					pSprite->Position = m_vecLauncher[iIndex]->m_pParticles[i].m_vPosition;// + m_vecLauncher[iIndex]->m_pParticles[i].m_vDiffuse; 
					pSprite->Height = m_vecLauncher[iIndex]->m_fLineWidth;
					pSprite->Width = m_vecLauncher[iIndex]->m_fTailSize;
					pSprite->Fade = m_vecLauncher[iIndex]->m_fTailFade;
					pSprite->Color = m_vecLauncher[iIndex]->m_pParticles[i].m_cColor;
					pSprite->Rotation = m_vecLauncher[iIndex]->m_pParticles[i].m_fRot;
				}
				
			}
			break;
		
		}
		
		iCount += iPerLaunSprCnt;
	}
	
	//系统在绘制之前调用这个函数,更新顶点。
	KModelSFXBillBoardBase::FrameMove();

	return S_OK;
}

void KModelSFXParticleSystem::DrawLauncher()
{
	
	int iCount = m_vecLauncher.size();
	int i;
	D3DCOLOR LaunColor;
	for (i = 0; i < iCount; i++)
	{
		KLauncher *pLan = m_vecLauncher[i];
		//如果是在发射得发射器，那么就设为红色，反之为兰色
		pLan->m_bLaunch ? LaunColor = 0xffff0000 : LaunColor = 0xff0000ff;
		LauncherShape a = pLan->m_eShape;
		switch(pLan->m_eShape)
		{
			int iIndex;
		case L_POINT:
			{
				D3DXVECTOR3	vBottom = pLan->m_vec3Direction;
				D3DXVECTOR3 vTop = pLan->m_vec3Translate;
				D3DXVec3Normalize(&vBottom, &vBottom);
				vBottom *= 20.0f;
				vTop -= vBottom;
				vBottom += vTop;
				g_cGraphicsTool.DrawCone( vTop, vBottom, 10.0f, 12, LaunColor);
			}
			break;
		case L_CIRCLE_FACE:
			{
				float fRadius = pLan->m_fShapePar[0];
				g_cGraphicsTool.DrawCircleFace(pLan->m_vec3Translate, pLan->m_vec3Direction,fRadius, 36, LaunColor, true, pLan->m_fParMaxSpread);

			}
			break;
		case L_CIRCLE:
			{
				float fRadius = pLan->m_fShapePar[0];
				g_cGraphicsTool.DrawCircle(pLan->m_vec3Translate, pLan->m_vec3Direction,fRadius, 36, LaunColor, true, pLan->m_fParMaxSpread);
			}
			break;
		case L_RECTANGLE:
			{
				D3DXVECTOR3 vA = D3DXVECTOR3(pLan->m_fShapePar[0], pLan->m_fShapePar[1], pLan->m_fShapePar[2]);
				D3DXVECTOR3 vB = D3DXVECTOR3(pLan->m_fShapePar[3], pLan->m_fShapePar[4], pLan->m_fShapePar[5]);
				D3DXVECTOR3 vC = D3DXVECTOR3(pLan->m_fShapePar[6], pLan->m_fShapePar[7], pLan->m_fShapePar[8]);
				D3DXVECTOR3 vD = D3DXVECTOR3(pLan->m_fShapePar[9], pLan->m_fShapePar[10], pLan->m_fShapePar[11]);
				g_cGraphicsTool.DrawLine(&vA, &vB,LaunColor, LaunColor);
				g_cGraphicsTool.DrawLine(&vB, &vC,LaunColor, LaunColor);
				g_cGraphicsTool.DrawLine(&vC, &vD,LaunColor, LaunColor);
				g_cGraphicsTool.DrawLine(&vD, &vA,LaunColor, LaunColor);

			}
			break;
		case L_RECTANGLE_FACE:
			{
				D3DXVECTOR3 vA = D3DXVECTOR3(pLan->m_fShapePar[0], pLan->m_fShapePar[1], pLan->m_fShapePar[2]);
				D3DXVECTOR3 vB = D3DXVECTOR3(pLan->m_fShapePar[3], pLan->m_fShapePar[4], pLan->m_fShapePar[5]);
				D3DXVECTOR3 vC = D3DXVECTOR3(pLan->m_fShapePar[6], pLan->m_fShapePar[7], pLan->m_fShapePar[8]);
				D3DXVECTOR3 vD = D3DXVECTOR3(pLan->m_fShapePar[9], pLan->m_fShapePar[10], pLan->m_fShapePar[11]);
				DWORD state;
				g_pd3dDevice->GetRenderState(D3DRS_CULLMODE, &state);
				g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
				g_cGraphicsTool.DrawRect(&vB, &vA, &vC, LaunColor,0);
				g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, state);
			}
			break;
		case L_LINE_LIST:
			{
				for ( iIndex = 0; iIndex < pLan->m_iPrimitiveCount; iIndex++ )
				{
					g_cGraphicsTool.DrawLine(pLan->m_pPrimitive + iIndex * 2, 
						                     pLan->m_pPrimitive + iIndex * 2 + 1,
											 LaunColor, LaunColor);
				}
				break;
			}
		case L_LINE_STRIP:
			{
				for ( iIndex = 0; iIndex < pLan->m_iPrimitiveCount; iIndex++ )
				{
					g_cGraphicsTool.DrawLine(pLan->m_pPrimitive + iIndex , 
					                         pLan->m_pPrimitive + iIndex + 1,
											 LaunColor, LaunColor);
				}
    			break;
			}
		case L_TRIANGLE_LIST:
			{
				/*for ( iIndex = 0; iIndex < pLan->m_iPrimitiveCount, iIndex++ )
				{
					g_cGraphicsTool.DrawTran
				}*/
				break;
			}
		case L_TRIANGLE_STRIP:
			{
				break;
			}
		case L_USER_DEFINE: //用户定义发射器形状,这个，没有做
			break;
		}
	}

}

void KModelSFXParticleSystem::DrawBarrier()
{
	int iCount = m_vecBarrier.size();
	int i;
	D3DCOLOR color;
	for (i = 0; i < iCount; i++)
	{
		if ( m_vecBarrier[i]->IsEff())
			color = 0xffff0000;
		else
			color = 0xff0000ff;
		switch(m_vecBarrier[i]->m_enumShape)
		{
		case B_SPHERE:
			{
				//球形
				//m_fBarPar数组的结构： 圆心位置0， 1， 2
				//						半径3
				D3DXVECTOR3 vCenter = D3DXVECTOR3(  m_vecBarrier[i]->m_fBarPar[0], 
													m_vecBarrier[i]->m_fBarPar[1], 
													m_vecBarrier[i]->m_fBarPar[2]
												  );
				float fRadius = m_vecBarrier[i]->m_fBarPar[3];
				g_cGraphicsTool.DrawSphere(vCenter, fRadius, 32, color);
				
			}
			break;
		case B_CYLINDER:
			{
				//m_fBarPar数组的结构：
				//上面圆面的圆心:0, 1, 2
				//下面圆面的圆心:3, 4, 5
				//半径: 6
                D3DXVECTOR3 vUpCenter = D3DXVECTOR3( 
                    m_vecBarrier[i]->m_fBarPar[0], 
                    m_vecBarrier[i]->m_fBarPar[1], 
                    m_vecBarrier[i]->m_fBarPar[2]
                );
                D3DXVECTOR3 vBtCenter = D3DXVECTOR3(
                    m_vecBarrier[i]->m_fBarPar[3],
                    m_vecBarrier[i]->m_fBarPar[4],
                    m_vecBarrier[i]->m_fBarPar[5]
                );
				float fRadius = m_vecBarrier[i]->m_fBarPar[6];
                g_cGraphicsTool.DrawCylinder(vUpCenter, vBtCenter, fRadius, 32, color);
			}
			break;
		case B_PLANE:
			{
				//平面， 矩形平面
				//m_fBarPar数组结构， 按顺时针排列顶点：v1(0, 1, 2), v2( 3, 4, 5), v3( 6, 7, 8), v4( 9, 10 , 11)
    	        D3DXVECTOR3 v1 = D3DXVECTOR3( m_vecBarrier[i]->m_fBarPar[0], m_vecBarrier[i]->m_fBarPar[1], m_vecBarrier[i]->m_fBarPar[2]);
		        D3DXVECTOR3 v2 = D3DXVECTOR3( m_vecBarrier[i]->m_fBarPar[3], m_vecBarrier[i]->m_fBarPar[4], m_vecBarrier[i]->m_fBarPar[5]);
		        D3DXVECTOR3 v3 = D3DXVECTOR3( m_vecBarrier[i]->m_fBarPar[6], m_vecBarrier[i]->m_fBarPar[7], m_vecBarrier[i]->m_fBarPar[8]);
		        D3DXVECTOR3 v4 = D3DXVECTOR3( m_vecBarrier[i]->m_fBarPar[9], m_vecBarrier[i]->m_fBarPar[10], m_vecBarrier[i]->m_fBarPar[11]);
			    g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
                g_cGraphicsTool.DrawRect(&v1, &v2, &v4, color, 0);    
			}
			break;
		case B_TRIANGLELIST:
			{
				g_cGraphicsTool.DrawTriangleList(m_vecBarrier[i]->m_pTriangle, m_vecBarrier[i]->m_iTriangleCount, color);
				break;
			}
		}

	}

}


void KModelSFXParticleSystem::DrawForceField()
{
	int iCount = m_vecForce.size();
	int i;
    D3DCOLOR color;
	for ( i = 0; i < iCount; i++)
	{
        if ( m_vecForce[i]->IsEff())
            color = 0xff00ff00;
        else color = 0xff00ffff;
        switch(m_vecForce[i]->m_eType)
        {
        case F_CENTR_SPHERE:
            {
                
			//m_fForcePar的结构:
			//0, 1, 2 - 球心
			//3       - 半径
                D3DXVECTOR3 vCenter = D3DXVECTOR3( m_vecForce[i]->m_fForcePar[0], 
                                                   m_vecForce[i]->m_fForcePar[1], 
                                                   m_vecForce[i]->m_fForcePar[2]
                                                  );
                //g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
                g_cGraphicsTool.DrawSphere(vCenter, m_vecForce[i]->m_fForcePar[3], 32, color );
                //g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,  D3DFILL_SOLID);
            }
            break;
        case F_CENTR_CYLINDER:
            {
                //m_fForcePar的结构:
			    //0,1,2 -- 轴线起点
			    //3,4,5 -- 轴线终点
			    //6     -- 力场半径
			    //7     -- 旋转放大系数
                D3DXVECTOR3 vUpCenter = D3DXVECTOR3( 
                    m_vecForce[i]->m_fForcePar[0], 
                    m_vecForce[i]->m_fForcePar[1], 
                    m_vecForce[i]->m_fForcePar[2]
                );
                D3DXVECTOR3 vBtCenter = D3DXVECTOR3(
                    m_vecForce[i]->m_fForcePar[3],
                    m_vecForce[i]->m_fForcePar[4],
                    m_vecForce[i]->m_fForcePar[5]
                );
				float fRadius = m_vecForce[i]->m_fForcePar[6];
                g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
                g_cGraphicsTool.DrawCylinder(vUpCenter, vBtCenter, fRadius, 32, color);
                g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
            }
            break;
        case F_DRAG:
            {

            }
            break;
        case F_GRAVITY:
            {
				break;
            }
        
        }

	}

}

HRESULT KModelSFXParticleSystem::SortSprites()
{
	//这里是否需要覆盖SortSprites主要在于是否存在使用不同材质的可能.
	return KModelSFXBillBoardBase::SortSprites();
}

HRESULT KModelSFXParticleSystem::AddLauncher(ModelSFX::KLauncher* pLan)
{
	KParticleSystem::AddLauncher(pLan);
	HRESULT hr = E_FAIL;
	char* vbsave = 0;
	char* vb = 0;
	DWORD dwParCount = 0;
	int iNumFace = m_nNumFaces;
	/*int i;*/
	LPDIRECT3DVERTEXBUFFER9 lpVBSave = m_lpVBuf;
	LPDIRECT3DVERTEXBUFFER9 lpVB;

	hr = g_pd3dDevice->CreateVertexBuffer( m_vecLauncher.size() * MAX_PARTICLE_COUNT *3*sizeof(VFormat::FACES_DIFFUSE_TEXTURE1),
		0,D3DFVF_Faces_Diffuse_Texture1,D3DPOOL_MANAGED, &lpVB,NULL ); 
	if ( FAILED(hr)) 
		goto e_createvb_err;

	hr = lpVBSave->Lock(0, 0, (void**)&vbsave, 0);
	if ( FAILED(hr)) 
		goto e_exit;
	hr = lpVB->Lock(0, 0, (void**)&vb, 0);
	if ( FAILED( hr ))
		goto e_exit;
	memcpy(vb, vbsave, sizeof( VFormat::FACES_DIFFUSE_TEXTURE1) * 3 * iNumFace);
	lpVB->Unlock();
	lpVBSave->Unlock();
	m_lpVBuf = lpVB;
	SAFE_RELEASE(lpVBSave);

	UpdateVertices();//更新绘制的信息。
	
	return S_OK;

e_createvb_err:
	return hr;
e_exit:
	SAFE_RELEASE(lpVB);
	return hr;
}


HRESULT KModelSFXParticleSystem::UpdateVertices()
{
	if(!m_lpVBuf)
		return S_OK;

	VFormat::FACES_DIFFUSE_TEXTURE1 * pVer = NULL;
	if ( FAILED( m_lpVBuf->Lock( 0, 0, (void**)&pVer, 0 ) ) )
		return E_FAIL;

	int nPoly = 9;
	float Radius = 130;
	D3DXVECTOR3 X_Base(1,0,0);
	D3DXVECTOR3 Y_Base(0,0,1);

	if (m_nBillBoardMethod==BILLBOARD_POSITION_ONLY)
	{
		g_cGraphicsTool.GetCameraLeftUp(&X_Base,&Y_Base);
	}
	if ( m_bDepthSort )
		SortSprites();

	D3DXVECTOR3 vDir;
	g_cGraphicsTool.GetCameraFront(&vDir);
	D3DXVec3Normalize(&vDir, &vDir);
	for(int i =0;i<(int)m_lpSprite.size();i++)
	{
		LPSPRITE pSprite = m_lpSprite[i];
		switch(pSprite->RenderType)
		{
		case PR_SPRITES:
		case PR_POINTS:
			{
			
				D3DXVECTOR3  PolyCenter = pSprite->Position;
				D3DXVECTOR3 XBASE = X_Base * pSprite->Width;
				D3DXVECTOR3 YBASE = Y_Base * pSprite->Height;

				float fSin = sin(pSprite->Rotation);
				float fCos = cos(pSprite->Rotation);
				D3DXVECTOR3 XXX = XBASE * fCos + YBASE * fSin;
				D3DXVECTOR3 YYY = YBASE * fCos - XBASE * fSin;

				int VI = i*6;
				pVer[VI  ].p = PolyCenter - XXX - YYY;pVer[VI  ].diffuse = pSprite->Color;pVer[VI  ].tu1 = 0;pVer[VI  ].tv1 = 1;
				pVer[VI+1].p = PolyCenter + XXX - YYY;pVer[VI+1].diffuse = pSprite->Color;pVer[VI+1].tu1 = 1;pVer[VI+1].tv1 = 1;
				pVer[VI+2].p = PolyCenter - XXX + YYY;pVer[VI+2].diffuse = pSprite->Color;pVer[VI+2].tu1 = 0;pVer[VI+2].tv1 = 0;
				pVer[VI+3].p = PolyCenter - XXX + YYY;pVer[VI+3].diffuse = pSprite->Color;pVer[VI+3].tu1 = 0;pVer[VI+3].tv1 = 0;
				pVer[VI+4].p = PolyCenter + XXX - YYY;pVer[VI+4].diffuse = pSprite->Color;pVer[VI+4].tu1 = 1;pVer[VI+4].tv1 = 1;
				pVer[VI+5].p = PolyCenter + XXX + YYY;pVer[VI+5].diffuse = pSprite->Color;pVer[VI+5].tu1 = 1;pVer[VI+5].tv1 = 0;
			}
			break;
		case PR_STREAK:
			{
				LPSPRITE pSprite = m_lpSprite[i];
				D3DXVECTOR3  PolyCenter = pSprite->Position;
				D3DXVec3Normalize(&X_Base, &pSprite->Velocity);
				D3DXVECTOR3 XXX = X_Base * pSprite->Width;
				//D3DXVec3Cross(&Y_Base, &X_Base, &vDir);
				Y_Base.x = 0.0f;
				Y_Base.y = 1.0f;
				Y_Base.z = 0.0f;
				D3DXVECTOR3 YYY = Y_Base * pSprite->Height;
				int VI = i*6;
				//Tail
				D3DCOLOR d3dColor = (pSprite->Color & 0x00ffffff) + DWORD((DWORD((BYTE(pSprite->Color & 0xff000000)>>24) * pSprite->Fade))<<24);
				pVer[VI  ].p = PolyCenter - XXX - YYY;pVer[VI  ].diffuse = d3dColor;pVer[VI  ].tu1 = 0;pVer[VI  ].tv1 = 1;
				pVer[VI+2].p = PolyCenter - XXX + YYY;pVer[VI+2].diffuse = d3dColor;pVer[VI+2].tu1 = 0;pVer[VI+2].tv1 = 0;
				pVer[VI+3].p = PolyCenter - XXX + YYY;pVer[VI+3].diffuse = d3dColor;pVer[VI+3].tu1 = 0;pVer[VI+3].tv1 = 0;
				
				//Head
				pVer[VI+1].p = PolyCenter + XXX - YYY;pVer[VI+1].diffuse = pSprite->Color;pVer[VI+1].tu1 = 1;pVer[VI+1].tv1 = 1;
				pVer[VI+4].p = PolyCenter + XXX - YYY;pVer[VI+4].diffuse = pSprite->Color;pVer[VI+4].tu1 = 1;pVer[VI+4].tv1 = 1;
				pVer[VI+5].p = PolyCenter + XXX + YYY;pVer[VI+5].diffuse = pSprite->Color;pVer[VI+5].tu1 = 1;pVer[VI+5].tv1 = 0;

			}
			break;
		}
	}
	m_lpVBuf->Unlock();
	m_nNumFaces = m_lpSprite.size() * 2;
	return S_OK;
}

HRESULT KModelSFXParticleSystem::Acquire(void)
{
	if (!m_bNeedReAcquire)
		return S_OK;

	PropertyBase::_PropertyPage* pPage = NULL;
	int PageIndex = -1;

	PageIndex = this->AddPage("Information");

	pPage = this->FindPage(PageIndex);
	if(pPage)
	{
		static int index = 0;
		PropertyBase::_PropertyGroup Group(0,"粒子系统",FALSE);
		int index_Group = pPage->AddGroup(Group);
		PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);
		int iLanSize = m_vecLauncher.size();
		//pGroup->AddPropertyInt("发射器个数",&iLanSize, 1, 100);
		//pGroup->AddPropertyInt("发射器索引",&index, 0, m_vecLauncher.size() - 1);
		KLauncher* pLan = m_vecLauncher[index];
		//pGroup->AddPropertyBool("是否发生扰动", (BOOL*)&pLan->m_bDiffuse);
		//pGroup->AddPropertyFloat("扰动幅度",&pLan->m_fSwing, 0, 99999.0f);
		//pGroup->AddPropertyFloat("扰动周期",&pLan->m_fCycle, 0, 99999.0f);
		//pGroup->AddPropertyBool("是否累加发射器速度", (BOOL*)&pLan->m_bAccumlate);
		////pGroup->AddPropertyTest("hahahahah", &index, 0, 100);
		////与发射器空间关系相关的成员
		//pGroup->AddPropertyPosition("发射器位置", &pLan->m_vec3Translate);
		//pGroup->AddPropertyPosition("发射器速度", &pLan->m_vVelocity);
		//pGroup->AddPropertyPosition("发射器方向", &pLan->m_vec3Direction);

		////LaunchingRule m_Rule;
		////与发射器形状相关的参数
		//LauncherShape m_eShape;
		//float m_fShapePar[20];
		
		//float m_fLaunchingSpan;//防止在开始的时候所有的粒子都同时被发射出去，在这个时间间隔内随机的发射
		////粒子，直到达到额定的粒子个数
		/*pGroup->AddPropertyInt("Rate", &pLan->m_iRate, 0, 99999);
		pGroup->AddPropertyFloat("发射最大角度", &pLan->m_fParMaxSpread, 0, 3.14, 0.1f);
		pGroup->AddPropertyFloat("发射最小角度", &pLan->m_fParMinSpread, 0, 3.14, 0.1f);

		pGroup->AddPropertyFloat("发射速度", &pLan->m_fParSpeed, 0, pLan->m_fParSpeed, 1.0f);
		pGroup->AddPropertyFloat("发射速度随机值", &pLan->m_fParSpeedRandom, 0, 999999.9f, 1.0f);

		pGroup->AddPropertyFloat("粒子最大生存时间", &pLan->m_fParMaxLifeSpan, 0, 9999999.9f, 0.1f);
		pGroup->AddPropertyFloat("粒子最小生存时间", &pLan->m_fParMinLifeSpan, 0, pLan->m_fParMaxLifeSpan, 0.1f);
	*/
		//pGroup->AddPropertyTimePoint("TimePoint", &m_vecLauncher[0]->m_cTimePoint[1]);
		//static int a;
		//KTimePointColor *kp = new KTimePointColor;
		//pGroup->AddPropertyTimePointColor("test", &pLan->m_tpColor);
		//pGroup->AddPropertyParticle("aaa",this);
		//pGroup->AddPropertyTimePoint("AAA", &pLan->m_tpScale);

		//PropertyBase::_PropertyGroup Group1(0,"粒子dffd系统",FALSE);
		//index_Group = pPage->AddGroup(Group1);

		//////////////////////////////////////////////////////////////////////////
		//test
		//KSceneEditBase* pScene = (KSceneEditBase*) g_cGraphicsTool.GetCurScene();
		//pScene->AddRenderEntity(SCENEENTITY_POSITION,&pLan->m_vec3Translate);
		//pScene->AddSelectableEntity(SCENEENTITY_POSITION,&pLan->m_vec3Translate);

	}
	m_bNeedReAcquire = FALSE;
	return S_OK;
}


};
