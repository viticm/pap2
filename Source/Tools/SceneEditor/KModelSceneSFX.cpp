// KModelSceneSFX.cpp: implementation of the KModelSceneSFX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "KModelSceneSFX.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KModelSceneSFX::KModelSceneSFX()
{
	m_dwNumMaxParticle = 100;
	m_lpVBuf = NULL;
	m_bNeedSort = true;
	m_fYSpeed = 0.5;
	m_iWhichBB = 2;
//	m_fYAccSpeed = 0.0;//0.05;
//	m_fXAccSpeed = 0.0;//0.01 + (3 - rand() % 6) / 500;
//	m_fZAccSpeed = 0.0;
	m_iMinWidth = 10;
	m_iMaxWidth = 20;
	m_iMinHeight = 50;
	m_iMaxHeight = 60;
	m_fInitSpeed = D3DXVECTOR3(0.0,0.0,0.0);
	m_v3InitMinPos = D3DXVECTOR3(-1000.0,500.0,-1000.0);
	m_v3InitMaxPos = D3DXVECTOR3(1000.0,1000.0,1000.0);
	m_v3AccSpeed = D3DXVECTOR3(0.0,0.5,0.0);
	m_fCountTime = 0.0;

	m_camera = g_GraphiceEngine.m_cCamera;
	m_fFogNearPos = 500.0;
	m_fFogFarPos =700.0;

//	m_camera.Position = D3DXVECTOR3(1000.1000,0.0,1000.0);
//	m_camera.ViewPoint = D3DXVECTOR3(10.0,10.0,10.0);
	CalFogPosition(m_fFogNearPos,m_fFogFarPos);

	m_v3InitMinPos = m_AABBox.A;
	m_v3InitMaxPos = m_AABBox.B;
	m_fTimeStep = 50.0;
	m_fTime[0] = 0.0;
	m_iFPS = 0;
	m_fAlphaBase = 200.0;
}

KModelSceneSFX::~KModelSceneSFX()
{

}

void KModelSceneSFX::CalFPS()
{
	if(m_fTime[0] == 0.0)
		m_fTime[0] = (float)timeGetTime();

	m_fTime[1] = (float)timeGetTime();
	m_fTime[2] = m_fTime[1] - m_fTime[0];

	static int fps = 0;

	fps++;

	if(m_fTime[2] > 1000.0)
	{
		m_iFPS = fps;
		fps = 0;
		m_fTime[0] = m_fTime[1];
	}
}

HRESULT KModelSceneSFX::Render()
{
	SetMatrix();

	m_camera = g_GraphiceEngine.m_cCamera;
	CalFogPosition(m_fFogNearPos,m_fFogFarPos);

	UpdateParticle();

	UpdateVerticesBuffer();

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);

//	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET,
//		D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

	
	g_pd3dDevice->SetTransform( D3DTS_WORLD , &mat);
	if(m_lpVBuf)
	{
		KModel::SetMaterial(0);

		g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);

		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		g_pd3dDevice->SetStreamSource( 0, m_lpVBuf,0, sizeof( VFormat::FACES_DIFFUSE_TEXTURE1 ) );
		g_pd3dDevice->SetFVF( D3DFVF_Faces_Diffuse_Texture1 );

		g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLELIST, 0 , m_listParticle.size()*2);
	}

	RestoreMatrix();
	CalFPS();

	TCHAR str [256];

	wsprintf(str,"FPS: %d ",m_iFPS);
	g_cGraphicsTool.DrawText(200,100,1,1,0xFFFF00FF,20,"宋体",str,0);

	return S_OK;
}
HRESULT KModelSceneSFX::LoadMesh(LPSTR pFileName)
{
	if((pFileName==NULL)||
		(_strcmpi(pFileName,"")==0))
	{
		m_dNumMaterial = 1;
		m_lpMaterial = new MATERIAL[m_dNumMaterial];
		ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);

		DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
			MATERIAL_OPTION_FILL_SOLID|
			MATERIAL_OPTION_SHADE_GOURAUD|
			MATERIAL_OPTION_CULL_NONE|
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
		wsprintf(Name,"%s\\SpeedTree.Mtl",g_Def_ModelDirectory);
		LoadMaterial(Name);

		wsprintf(m_szMeshFileName,"Scene SFX");

		CreateVerticesBuffer();

		for(DWORD i=0;i<m_dwNumMaxParticle;i++)
		{
			Add1Particle();
		}
		
	}
	else
	{
		
		wsprintf(m_szMeshFileName,pFileName);

	}
	return S_OK;
}
HRESULT KModelSceneSFX::SaveMesh(LPSTR pFileName)
{
	return S_OK;
}

HRESULT KModelSceneSFX::CreateVerticesBuffer()
{
	SAFE_RELEASE(m_lpVBuf);
	DWORD NumFaces = m_dwNumMaxParticle * 2;

	if(!NumFaces)
		return S_OK;

	if ( FAILED( g_pd3dDevice->CreateVertexBuffer( NumFaces * 3 * sizeof(VFormat::FACES_DIFFUSE_TEXTURE1),
		0,D3DFVF_Faces_Diffuse_Texture1,
		D3DPOOL_MANAGED, &m_lpVBuf,NULL ) ) )
	{
		return E_FAIL;
	}
	return S_OK;
}

bool Small(KModelSceneSFX::LPPARTICLE A,KModelSceneSFX::LPPARTICLE B)
{
//	return (A->YawPitchRoll.x>B->YawPitchRoll.x);
	return (A->DisToView > B->DisToView);
}

D3DXVECTOR3 KModelSceneSFX::VectorMuliMatrix(const D3DXVECTOR3 *pV3,const D3DXMATRIX *pMatrix)
{
	D3DXVECTOR3 temp;

	temp.x = pV3->x * pMatrix->_11 + pV3->y * pMatrix->_21 + pV3->z * pMatrix->_31;
	temp.y = pV3->x * pMatrix->_12 + pV3->y * pMatrix->_22 + pV3->z * pMatrix->_32;
	temp.z = pV3->x * pMatrix->_13 + pV3->y * pMatrix->_23 + pV3->z * pMatrix->_33;

	return temp;
}

void KModelSceneSFX::MakeViewM(D3DXMATRIX *matrix,const D3DXVECTOR3 *front,const D3DXVECTOR3 *left,
							   const D3DXVECTOR3 *up)
{
	matrix->_11 = left->x; matrix->_21 = left->y; matrix->_31 = left->z; matrix->_41 = 0.0;
	matrix->_12 = up->x; matrix->_22 = up->y; matrix->_32 = up->z; matrix->_42 = 0.0;
	matrix->_13 = front->x; matrix->_23 = front->y; matrix->_33 = front->z; matrix->_43 = 0.0;
	matrix->_14 = 0.0; matrix->_24 = 0.0; matrix->_34 = 0.0; matrix->_44 = 1.0;
}

HRESULT KModelSceneSFX::UpdateVerticesBuffer()
{
	if(!m_lpVBuf)
		return S_OK;

	D3DXVECTOR3 Left,Up,Front;//left,up,front;
	g_cGraphicsTool.GetCameraFront(&Front);
	g_cGraphicsTool.GetCameraLeftUp(&Left,&Up);

	VFormat::FACES_DIFFUSE_TEXTURE1 * pVer = NULL;
	if ( FAILED( m_lpVBuf->Lock( 0, 0, (void**)&pVer, 0 ) ) )
		return E_FAIL;

	D3DXVECTOR3 SunDirection(0,1,0);

	LPSCENE pScene = g_cGraphicsTool.GetCurScene();
	if(pScene)
	{
		SunDirection = pScene->m_SunLight.Direction;
	}

	if(m_bNeedSort)
		m_listParticleSort.sort(Small);

	D3DXMATRIX RotateM,ViewM;
	//float angle,newx,newy;
	D3DXVECTOR3 newspeed;
	D3DXVECTOR3 A,B,C,D;
	D3DXVECTOR3 NewFront;

	MakeViewM(&ViewM,&Front,&Left,&Up);

	int S = 0;
	list<LPPARTICLE>::iterator i = m_listParticleSort.begin();
	while(i!=m_listParticleSort.end())
	{
		LPPARTICLE pPat = *i;
		
		newspeed = VectorMuliMatrix(&pPat->Speed, &ViewM);

		if(m_iWhichBB == CYLINDRICAL)
		{
			NewFront = D3DXVECTOR3(Front.x,0.0,Front.z);
		}

		if(m_iWhichBB != NONE)
		{
			if(m_iWhichBB != FOG)
			{
				if(newspeed.y <= 0.00001F)
					newspeed.y = 0.00001F;

				D3DXMatrixRotationAxis(&RotateM,&Front,asinf(-newspeed.x / newspeed.y));
			}
		}
		else
		{
			D3DXMatrixRotationAxis(&RotateM,&Front/*&pPat->Normal*/,pPat->angle);	//angle在UpDataPatticle中变化
		}


		float Sin = sinf(pPat->YawPitchRoll.z);
		float Cos = cosf(pPat->YawPitchRoll.z);

		D3DXVECTOR3 L = Left*Cos + Up*Sin;
		D3DXVECTOR3 U = -Left*Sin + Up*Cos;
		D3DXVECTOR3 tempvec3(0.0,pPat->Height,0.0),temp2vec3(pPat->Width,0.0,0.0),
					temp3vec3(0.0,0.0,pPat->Height);

		switch(m_iWhichBB)
		{
		case NONE:

			A = pPat->Position - pPat->Right * pPat->Width + pPat->Up * pPat->Height;
			B = pPat->Position + pPat->Right * pPat->Width + pPat->Up * pPat->Height;
			C = pPat->Position + pPat->Right * pPat->Width - pPat->Up * pPat->Height;
			D = pPat->Position - pPat->Right * pPat->Width - pPat->Up * pPat->Height;

			break;

		case SPHERICAL:

			A = pPat->Position - L*pPat->Width + U*pPat->Height + Front*pPat->YawPitchRoll.y*pPat->Size;
			B = pPat->Position + L*pPat->Width + U*pPat->Height - Front*pPat->YawPitchRoll.y*pPat->Size;
			C = pPat->Position + L*pPat->Width - U*pPat->Height - Front*pPat->YawPitchRoll.y*pPat->Size;
			D = pPat->Position - L*pPat->Width - U*pPat->Height + Front*pPat->YawPitchRoll.y*pPat->Size;
			break;

		case CYLINDRICAL:

			A = pPat->Position - L*pPat->Width + tempvec3 + Front*pPat->YawPitchRoll.y*pPat->Size;
			B = pPat->Position + L*pPat->Width + tempvec3 - Front*pPat->YawPitchRoll.y*pPat->Size;
			C = pPat->Position + L*pPat->Width - tempvec3 - Front*pPat->YawPitchRoll.y*pPat->Size;
			D = pPat->Position - L*pPat->Width - tempvec3 + Front*pPat->YawPitchRoll.y*pPat->Size;
			break;

		case FOG:
			
			float a = pPat->PointMove[0] * pPat->MoveSize;
			float b = pPat->PointMove[1] * pPat->MoveSize;
			float c = pPat->PointMove[2] * pPat->MoveSize;
			float d = pPat->PointMove[3] * pPat->MoveSize;

			
			A = pPat->Position - L*(pPat->Width + a) + U*(pPat->Height + a);
			B = pPat->Position + L*(pPat->Width + b) + U*(pPat->Height + b);
			C = pPat->Position + L*(pPat->Width + c) - U*(pPat->Height + c);
			D = pPat->Position - L*(pPat->Width + d) - U*(pPat->Height + d);

			/*
			A = pPat->Position - L*pPat->Width + U*pPat->Height;
						B = pPat->Position + L*pPat->Width + U*pPat->Height;
						C = pPat->Position + L*pPat->Width - U*pPat->Height;
						D = pPat->Position - L*pPat->Width - U*pPat->Height;*/
			
			
			break;

		}

		if(m_iWhichBB != FOG)
		{
			A = VectorMuliMatrix(&A,&RotateM);
			B = VectorMuliMatrix(&B,&RotateM);
			C = VectorMuliMatrix(&C,&RotateM);
			D = VectorMuliMatrix(&D,&RotateM);
		}

		float tuA,tuB,tuC,tuD;
		float tvA,tvB,tvC,tvD;

		switch(pPat->TextureUV)
		{
		case 0:
			{
				tuA = 0   ;tvA = 0   ;
				tuB = 0.5F;tvB = 0   ;
				tuC = 0.5F;tvC = 0.5F;
				tuD = 0   ;tvD = 0.5F;
				break;
			}
		case 1:
			{
				tuA = 0.5F;tvA = 0   ;
				tuB = 1.0F;tvB = 0   ;
				tuC = 1.0F;tvC = 0.5F;
				tuD = 0.5F;tvD = 0.5F;
				break;
			}
		case 2:
			{
				tuA = 0   ;tvA = 0.5F;
				tuB = 0.5F;tvB = 0.5F;
				tuC = 0.5F;tvC = 1.0F;
				tuD = 0   ;tvD = 1.0F;
				break;
			}
		case 3:
			{
				tuA = 0.5F;tvA = 0.5F;
				tuB = 1.0F;tvB = 0.5F;
				tuC = 1.0F;tvC = 1.0F;
				tuD = 0.5F;tvD = 1.0F;
				break;
			}
		case 4:
			{
				tuA = 0.0F;tvA = 0.0F;
				tuB = 1.0F;tvB = 0.0F;
				tuC = 1.0F;tvC = 1.0F;
				tuD = 0.0F;tvD = 1.0F;
			}
		}

		D3DCOLORVALUE End;
		if(m_iWhichBB != FOG)
		{
			float Light = max(0,D3DXVec3Dot(&SunDirection,&pPat->Normal));
			
			End.a = min(1,pPat->Ambient.a + pPat->Diffuse.a * Light);
			End.r = min(1,pPat->Ambient.r + pPat->Diffuse.r * Light);
			End.g = min(1,pPat->Ambient.g + pPat->Diffuse.g * Light);
			End.b = min(1,pPat->Ambient.b + pPat->Diffuse.b * Light);		
		}
		else
		{
/*			LPSCENE pScene = g_cGraphicsTool.GetCurScene();
			float TinyTime = 0;
			if(pScene)
			{
				TinyTime = pScene->m_TimeControl.GetTinyTime();
			}
*/
			End = pPat->Ambient;
			
/*			if(m_fCountTime > 100)
			{
				if(pPat->AlphaChange == 0)
				{
					pPat->Ambient.a += pPat->SuaiJie;
					if(pPat->Ambient.a >= 1.0)
					{
						pPat->AlphaChange = 1;
					}
				}
				else
				{
					pPat->Ambient.a -= pPat->SuaiJie;
				}
				
				pPat->MoveSize += 5.0;
			}*/
		}

		D3DCOLOR color = ::ColorValue(End);

		pVer[S*6  ].p = A;pVer[S*6  ].diffuse=color;pVer[S*6  ].tu1=tuA;pVer[S*6  ].tv1=tvA;
		pVer[S*6+1].p = D;pVer[S*6+1].diffuse=color;pVer[S*6+1].tu1=tuD;pVer[S*6+1].tv1=tvD;
		pVer[S*6+2].p = B;pVer[S*6+2].diffuse=color;pVer[S*6+2].tu1=tuB;pVer[S*6+2].tv1=tvB;

		pVer[S*6+3].p = B;pVer[S*6+3].diffuse=color;pVer[S*6+3].tu1=tuB;pVer[S*6+3].tv1=tvB;
		pVer[S*6+4].p = D;pVer[S*6+4].diffuse=color;pVer[S*6+4].tu1=tuD;pVer[S*6+4].tv1=tvD;
		pVer[S*6+5].p = C;pVer[S*6+5].diffuse=color;pVer[S*6+5].tu1=tuC;pVer[S*6+5].tv1=tvC;


		i++;
		S++;
	}

	m_lpVBuf->Unlock();

	if(m_fCountTime > m_fTimeStep)
		m_fCountTime = 0.0;

	return S_OK;
}

HRESULT KModelSceneSFX::UpdateParticle()
{
	D3DXVECTOR3 Left,Up,Front,CPos;//left,up,front;
	g_cGraphicsTool.GetCameraFront(&Front);
	g_cGraphicsTool.GetCameraLeftUp(&Left,&Up);
	g_cGraphicsTool.GetCameraPos(&CPos);

//	CalFogPosition(m_fFogNearPos,m_fFogFarPos);

	if(m_iWhichBB == FOG)
	{
		m_v3InitMinPos = m_AABBox.A;
		m_v3InitMaxPos = m_AABBox.B;
	}

	LPSCENE pScene = g_cGraphicsTool.GetCurScene();
	float TinyTime = 0;
	if(pScene)
	{
		TinyTime = pScene->m_TimeControl.GetTinyTime();

		m_fCountTime += TinyTime;
	}

	list<LPPARTICLE>::iterator i = m_listParticle.begin();
	while(i!=m_listParticle.end())
	{
		LPPARTICLE pLeaf = *i;

		if(m_bNeedSort)
		{
//			D3DXVECTOR3 PC = pLeaf->Position - CPos;
//			pLeaf->Distance = D3DXVec3Dot(&PC,&Front);
			CalDisToView(pLeaf,Front,CPos);
		}
		
		if(m_fCountTime > m_fTimeStep)
		{
			float f = m_iMaxWidth * m_iMinHeight / (pLeaf->Width * pLeaf->Height);
			pLeaf->Speed += D3DXVECTOR3(pLeaf->Accelerate.x * f,pLeaf->Accelerate.y,
										pLeaf->Accelerate.z * f); 
			pLeaf->angle += 0.02F;

			if(pLeaf->AlphaChange == 0)
			{
				pLeaf->Ambient.a += pLeaf->SuaiJie;
				if(pLeaf->Ambient.a >= 1.0F)
				{
					pLeaf->AlphaChange = 1;
					pLeaf->Ambient.a = 1.0F;
				}
			}
			else
			{
				pLeaf->Ambient.a -= pLeaf->SuaiJie;
			}

			pLeaf->MoveSize += 1;
		}
		
		pLeaf->Position -= pLeaf->Speed;

		if(m_iWhichBB != FOG)
		{
			if(pLeaf->Position.y<0)
			{
				pLeaf->Position.x = (float)m_v3InitMinPos.x + rand() % (int)(m_v3InitMaxPos.x - m_v3InitMinPos.x);//rand()%2000 - 1000.0f;
				pLeaf->Position.y = (float)m_v3InitMinPos.y + rand() % (int)(m_v3InitMaxPos.y - m_v3InitMinPos.y);//rand() % 500 + 500;
				pLeaf->Position.z = (float)m_v3InitMinPos.z + rand() % (int)(m_v3InitMaxPos.z - m_v3InitMinPos.z);

				pLeaf->Speed = D3DXVECTOR3(0.0,pLeaf->YInitSpeed,0.0);
				pLeaf->Width = (float)m_iMinWidth + rand() % (m_iMaxWidth - m_iMinWidth);
				pLeaf->Height = (float)m_iMinHeight + rand() % (m_iMaxHeight - m_iMinHeight);
				pLeaf->Accelerate = m_v3AccSpeed;

				pLeaf->Normal.x = (float)(rand() % 30);
				pLeaf->Normal.y = (float)(rand() % 30);
				pLeaf->Normal.z = (float)(rand() % 30);

				D3DXVec3Normalize(&pLeaf->Normal,&pLeaf->Normal);
				D3DXVec3Cross(&pLeaf->Right,&D3DXVECTOR3(0.0,1.0,0.0),&pLeaf->Normal);
				D3DXVec3Normalize(&pLeaf->Right,&pLeaf->Right);
				D3DXVec3Cross(&pLeaf->Up,&pLeaf->Normal,&pLeaf->Up);
				D3DXVec3Normalize(&pLeaf->Up,&pLeaf->Up);

				pLeaf->Speed = m_fInitSpeed;
		
				pLeaf->TextureUV = 4;//rand()%4;
							
				pLeaf->angle = 0.0;
//				pLeaf->SuaiJie = (rand() % 6 + 5) / 100.0;
			}
		}
		else
		{
//			m_camera = g_GraphiceEngine.m_cCamera;
			if(m_camera.Position != g_GraphiceEngine.m_cCamera.Position)
				return S_FALSE;

			if(pLeaf->Ambient.a < 0.00001 && pLeaf->AlphaChange == 1)
			{
				pLeaf->Position.x = (float)m_v3InitMinPos.x + rand() % (int)(m_v3InitMaxPos.x - m_v3InitMinPos.x > 1.0 ? m_v3InitMaxPos.x - m_v3InitMinPos.x : 1.0);//rand()%2000 - 1000.0f;
				pLeaf->Position.y = (float)m_v3InitMinPos.y + rand() % (int)(m_v3InitMaxPos.y - m_v3InitMinPos.y > 1.0 ? m_v3InitMaxPos.y - m_v3InitMinPos.y : 1.0);//rand() % 500 + 500;
				pLeaf->Position.z = (float)m_v3InitMinPos.z + rand() % (int)(m_v3InitMaxPos.z - m_v3InitMinPos.z > 1.0 ? m_v3InitMaxPos.z - m_v3InitMinPos.z : 1.0);//rand()%2000 - 1000.0f;

				pLeaf->Speed = D3DXVECTOR3(0.0,pLeaf->YInitSpeed,0.0);
				pLeaf->Width = (float)m_iMinWidth + rand() % (m_iMaxWidth - m_iMinWidth);
				pLeaf->Height = (float)m_iMinHeight+ rand() % (m_iMaxHeight - m_iMinHeight);
				pLeaf->Accelerate = m_v3AccSpeed;

				pLeaf->Normal.x = (float)(rand() % 30);
				pLeaf->Normal.y = (float)(rand() % 30);
				pLeaf->Normal.z = (float)(rand() % 30);

				D3DXVec3Normalize(&pLeaf->Normal,&pLeaf->Normal);
				D3DXVec3Cross(&pLeaf->Right,&D3DXVECTOR3(0.0,1.0,0.0),&pLeaf->Normal);
				D3DXVec3Normalize(&pLeaf->Right,&pLeaf->Right);
				D3DXVec3Cross(&pLeaf->Up,&pLeaf->Normal,&pLeaf->Up);
				D3DXVec3Normalize(&pLeaf->Up,&pLeaf->Up);

				pLeaf->Speed = m_fInitSpeed;
				pLeaf->SuaiJie = (float)((rand() % 6 + 5) / m_fAlphaBase);
				pLeaf->angle = 0.0;
				pLeaf->TextureUV = rand()%4;
				pLeaf->Ambient.a = 0.0;
//				pLeaf->Diffuse.a = 0.0;
				pLeaf->MoveSize = 0.0;
				pLeaf->AlphaChange = 0;

				int i;

				for(i = 0;i < 4;++i)
				{
					pLeaf->PointMove[i] = rand() % 3 + 1;
				}				
			}
		}

		i++;
	}

//	m_camera = g_GraphiceEngine.m_cCamera;

	return S_OK;
}

KModelSceneSFX::LPPARTICLE KModelSceneSFX::Add1Particle()
{
	if(m_listParticle.size()>m_dwNumMaxParticle)
		return NULL;

	D3DXVECTOR3 Front,CPos;//left,up,front;
	g_cGraphicsTool.GetCameraFront(&Front);
	g_cGraphicsTool.GetCameraPos(&CPos);

	LPPARTICLE pLeaf = new Particle;

	pLeaf->Normal.x = (float)(rand() % 30);
	pLeaf->Normal.y = (float)(rand() % 30);
	pLeaf->Normal.z = (float)(rand() % 30);

	D3DXVec3Normalize(&pLeaf->Normal,&pLeaf->Normal);
	D3DXVec3Cross(&pLeaf->Right,&D3DXVECTOR3(0.0,1.0,0.0),&pLeaf->Normal);
	D3DXVec3Normalize(&pLeaf->Right,&pLeaf->Right);
	D3DXVec3Cross(&pLeaf->Up,&pLeaf->Normal,&pLeaf->Up);
	D3DXVec3Normalize(&pLeaf->Up,&pLeaf->Up);

	
	D3DXVECTOR3 cpos;
	g_cGraphicsTool.GetCameraPos(&cpos);
	pLeaf->Position.x = (float)m_v3InitMinPos.x + rand() % (int)(m_v3InitMaxPos.x - m_v3InitMinPos.x);//(rand()%60 - 30);
	pLeaf->Position.y = (float)m_v3InitMinPos.y + rand() % (int)(m_v3InitMaxPos.y - m_v3InitMinPos.y);//(rand()%60 + 500);
	pLeaf->Position.z = (float)m_v3InitMinPos.z + rand() % (int)(m_v3InitMaxPos.z - m_v3InitMinPos.z);
	
	pLeaf->Size = (float)(rand()%20+50);

	if(m_iWhichBB != FOG)
		pLeaf->TextureUV = 4;//rand()%4;
	else
		pLeaf->TextureUV = rand()%4;

	pLeaf->Width = m_iMinWidth + rand() % (m_iMaxWidth - m_iMinWidth);
	pLeaf->Height = m_iMinHeight + rand() % (m_iMaxHeight - m_iMinHeight);

	pLeaf->SuaiJie = (float)((rand() % 6 + 5) / 100.0);
	pLeaf->MoveSize = 0.0;

	pLeaf->Speed = m_fInitSpeed;
	pLeaf->Accelerate = m_v3AccSpeed;

	int i;

	for(i = 0;i < 4;++i)
	{
		pLeaf->PointMove[i] = rand() % 3;
	}

	pLeaf->YawPitchRollAccelerate = D3DXVECTOR3(0,0,0.00001f);
	D3DXVec3Normalize(&pLeaf->NormalOffset,&pLeaf->Position);
	m_listParticle.push_back(pLeaf);

	m_listParticleSort = m_listParticle;
	return pLeaf;
}

void KModelSceneSFX::CalFogPosition(const float &Near,const float &Far)
{
	D3DXMATRIX view;
	g_pd3dDevice->GetTransform(D3DTS_VIEW,&view);

	{
		D3DXVECTOR3 tFront,tUp,tLeft,tPos;
		g_cGraphicsTool.GetCameraFront(&tFront);
		g_cGraphicsTool.GetCameraLeftUp(&tLeft,&tUp);
		g_cGraphicsTool.GetCameraPos(&tPos);
	}
	
	D3DXVECTOR3 ViewDiec = m_camera.ViewPoint - m_camera.Position;
	D3DXVECTOR3 Up = m_camera.DefUp;
	D3DXVECTOR3 Right;
	D3DXVec3Normalize(&ViewDiec,&ViewDiec);
	D3DXVec3Cross(&Right,&Up,&ViewDiec);
	D3DXVec3Normalize(&Right,&Right);
	D3DXVec3Cross(&Up,&ViewDiec,&Right);
	D3DXVec3Normalize(&Up,&Up);
/*

	ViewDiec = D3DXVECTOR3(view._13,view._23,view._33);
	Up = D3DXVECTOR3(view._12,view._22,view._32);
	Right = D3DXVECTOR3(view._11,view._21,view._31);
	m_camera.Position = D3DXVECTOR3(view._41,view._42,view._43);
*/

	D3DXVECTOR3 FogNearCenter,FogFarCenter;
	FogNearCenter = m_camera.Position + Near * ViewDiec;
	FogFarCenter = m_camera.Position + Far * ViewDiec;	

	float NearHalfWidth,NearHalfHeight,FarHalfWidth,FarHalfHeight;
	NearHalfHeight = tan(m_camera.Field / 2.0) * Near; 
	FarHalfHeight = tan(m_camera.Field / 2.0) * Far; 
	NearHalfWidth = NearHalfHeight * m_camera.Aspect;
	FarHalfWidth = FarHalfHeight * m_camera.Aspect;

	D3DXVECTOR3 Vertex[8];
	D3DXVECTOR3 RNW = Right * NearHalfWidth;
	D3DXVECTOR3 UNH = Up * NearHalfHeight;
	D3DXVECTOR3 RFW = Right * FarHalfWidth;
	D3DXVECTOR3 UFH = Up * FarHalfHeight;
	
	Vertex[0] = FogNearCenter - RNW + UNH;
	Vertex[1] = FogNearCenter + RNW + UNH;
	Vertex[2] = FogNearCenter + RNW - UNH;
	Vertex[3] = FogNearCenter - RNW - UNH;

	Vertex[4] = FogFarCenter - RFW + UFH;
	Vertex[5] = FogFarCenter + RFW + UFH;
	Vertex[6] = FogFarCenter + RFW - UFH;
	Vertex[7] = FogFarCenter - RFW - UFH;

	m_AABBox.AddPosition(Vertex,8);

		m_AABBox.A = m_AABBox.B = Vertex[0];
		for(int i = 1;i < 8;++i)
		{
			D3DXVec3Minimize(&m_AABBox.A,&m_AABBox.A,&Vertex[i]);
			D3DXVec3Maximize(&m_AABBox.B,&m_AABBox.B,&Vertex[i]);
		}
	
}

void KModelSceneSFX::LoadData(const char *FileDir)
{
	BBHeader bbheader;

	FILE *fp;
	DWORD version;

	fp = fopen(FileDir,"rb");

	if(!fp)
		return;

	fread(&version,sizeof(DWORD),1,fp);
	fread(&bbheader,sizeof(BBHeader),1,fp);

	fclose(fp);

//	SetMaxParticle(bbheader.m_dwMaxParticle);
	SetAccSpeed(bbheader.m_v3AccSpeed);
	SetInitMinPos(bbheader.m_v3InitMinPos);
	SetInitMaxPos(bbheader.m_v3InitMaxPos);
	SetInitSpeed(bbheader.m_fInitSpeed);
	SetHeight(bbheader.m_iMinHeight,bbheader.m_iMaxHeight);
	SetWidth(bbheader.m_iMinWidth,bbheader.m_iMaxWidth);
	SetWhichBB(bbheader.m_iWhichBB);
	SetMaxParticle(bbheader.m_dwMaxParticle);

	m_dNumMaterial = 1;
	m_lpMaterial = new MATERIAL[m_dNumMaterial];
	ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);

	DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
		MATERIAL_OPTION_FILL_SOLID|
		MATERIAL_OPTION_SHADE_GOURAUD|
		MATERIAL_OPTION_CULL_NONE|
		MATERIAL_OPTION_SPECULARENABLE;

	for(DWORD i=0;i<m_dNumMaterial;i++)
	{
		m_lpMaterial[i].m_sMaterial9.Diffuse.r = 1.0f ;
		m_lpMaterial[i].m_sMaterial9.Diffuse.g = 1.0f ;
		m_lpMaterial[i].m_sMaterial9.Diffuse.b = 1.0f ;
		m_lpMaterial[i].m_sMaterial9.Diffuse.a = 1.0f ;
		m_lpMaterial[i].m_sMaterial9.Ambient = m_lpMaterial[i].m_sMaterial9.Diffuse;
		m_lpMaterial[i].m_sMaterial9.Specular = m_lpMaterial[i].m_sMaterial9.Diffuse;
		m_lpMaterial[i].m_sMaterial9.Power = 15;
		m_lpMaterial[i].m_dOption = Def_Option;
	}
}

void KModelSceneSFX::CalDisToView(LPPARTICLE p,const D3DXVECTOR3 &front,
								  const D3DXVECTOR3 &camerapos)
{

	p->DisToView = p->Position.x * front.x + p->Position.y * front.y + p->Position.z * front.z + 
				   camerapos.z;
}

HRESULT KModelSceneSFX::DeleteParticle(LPPARTICLE pLeaf)
{
	m_listParticle.remove(pLeaf);
	m_listParticleSort = m_listParticle;

	SAFE_DELETE(pLeaf);
	return S_OK;
}

HRESULT KModelSceneSFX::CleanUp()
{
	list<LPPARTICLE>::iterator i = m_listParticle.begin();
	while(i!=m_listParticle.end())
	{
		LPPARTICLE pLeaf = *i;
		SAFE_DELETE(pLeaf);
		i++;
	}
	m_listParticle.clear();
	m_listParticleSort.clear();
	return S_OK;
}

HRESULT KModelSceneSFX::SetMaxParticle(DWORD Num)
{
	CleanUp();
	m_dwNumMaxParticle = Num;
	CreateVerticesBuffer();

	for(DWORD i=0;i<m_dwNumMaxParticle;i++)
	{
		Add1Particle();
	}
	return S_OK;
}