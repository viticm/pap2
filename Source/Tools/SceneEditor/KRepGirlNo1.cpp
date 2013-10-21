// KRepGirlNo1.cpp: implementation of the KRepGirlNo1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "KRepGirlNo1.h"
#include "MeshBlade.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KRepGirlNo1::KRepGirlNo1()
{
	m_bRun = TRUE; 
	m_nCurCharacter = -1;
	m_pModelBelt = NULL;
	m_bRiding = FALSE;

	m_bAttacking = FALSE;

	m_dwState = 0;

	m_Stand = -1;
	m_Run = -1;
	m_Walk = -1;

	m_RideRun = -1;
	m_RideStand = -1;

	m_fSpeed = 5;
	m_pModelBody = NULL;
	m_bMoving2Position = FALSE;
	m_fRotationSpeed = D3DX_PI/18;

	m_pModelRideBody = NULL;
	m_pModelHorse = NULL;
}

KRepGirlNo1::~KRepGirlNo1()
{

}


HRESULT KRepGirlNo1::Load()
{
	m_nCurCharacter = -1;

	DWORD MeshID = 0;

	TCHAR Name[256];
	TCHAR FileName[256];
	//wsprintf(Name,"%s\\%s",g_Def_ModelDirectory,"主角\\002.Mesh");
	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"编辑器数据\\主角.ini");

	if(FAILED(g_cFileFinder.FindFile(FileName,Name)))
	{
		MessageBox(NULL,Name,"找不到配置文件！",0);
		return S_OK;
	}

	g_cMeshTable.Get1NewXMesh(&m_pModelBlade,MODELTYPE_BLADE);
	m_pModelBlade->LoadMesh("模型文件\\001.blade");

	IIniFile* Ini = g_OpenIniFile(FileName);
	

	DWORD i_Char = 1;
	while(1)
	{
		TCHAR CharName[256];
		wsprintf(CharName,"Character%d",i_Char);
		if(!Ini->IsSectionExist(CharName))
			break;

		_CharacterInfo Info;
		Ini->GetString(CharName,"Mesh","",Name,256);
		::TrimString(Name);
		Info.m_scMeshFile = Name;

		Ini->GetString(CharName,"Mtl","",Name,256);
		::TrimString(Name);
		Info.m_scMtlFile = Name;

		Ini->GetString(CharName,"Ani","",Name,256);
		::TrimString(Name);
		Info.m_scAniFile = Name;	

		g_cMeshTable.Get1NewXMesh(&Info.m_lpModelCharactor,MODELTYPE_DEFAULT);
		
		if(SUCCEEDED(g_cFileFinder.FindFile(FileName,(LPSTR)Info.m_scMeshFile.c_str())))
		{
			Info.m_lpModelCharactor->LoadMesh(FileName);
		}
		if(SUCCEEDED(g_cFileFinder.FindFile(FileName,(LPSTR)Info.m_scMtlFile.c_str())))
		{
			Info.m_lpModelCharactor->LoadMaterial(FileName);
		}
		if(SUCCEEDED(g_cFileFinder.FindFile(FileName,(LPSTR)Info.m_scAniFile.c_str())))
		{
			Info.m_lpModelCharactor->LoadAnimation(FileName);
		}
		//////////////////////////////////////////////////////////////////////////		
		DWORD i_Socket = 1;
		while(1)
		{
			TCHAR SocketName[256];
			wsprintf(SocketName,"Socket%d",i_Socket);

			Ini->GetString(CharName,SocketName,"",Name,256);
			::TrimString(Name);
			if(_strcmpi(Name,"")==0)
				break;
			//////////////////////////////////////////////////////////////////////////
			_SocketInfo Socket;
			Socket.m_scSocketName = Name;
		
			wsprintf(SocketName,"BindMesh%d",i_Socket);
			Ini->GetString(CharName,SocketName,"",Name,256);
			::TrimString(Name);
			Socket.m_scBindMeshFile = Name;

			wsprintf(SocketName,"BindMtl%d",i_Socket);
			Ini->GetString(CharName,SocketName,"",Name,256);
			::TrimString(Name);
			Socket.m_scBindMtlFile = Name;

			wsprintf(SocketName,"BindAni%d",i_Socket);
			Ini->GetString(CharName,SocketName,"",Name,256);
			::TrimString(Name);
			Socket.m_scBindAniFile = Name;

			g_cMeshTable.Get1NewXMesh(&Socket.m_lpModelBind,MODELTYPE_DEFAULT);
			if(SUCCEEDED(g_cFileFinder.FindFile(FileName,(LPSTR)Socket.m_scBindMeshFile.c_str())))
			{
				Socket.m_lpModelBind->LoadMesh(FileName);
			}
			if(SUCCEEDED(g_cFileFinder.FindFile(FileName,(LPSTR)Socket.m_scBindMtlFile.c_str())))
			{
				Socket.m_lpModelBind->LoadMaterial(FileName);
			}
			if(SUCCEEDED(g_cFileFinder.FindFile(FileName,(LPSTR)Socket.m_scBindAniFile.c_str())))
			{
				Socket.m_lpModelBind->LoadAnimation(FileName);
			}

			Socket.m_lpModelBind->Bind(Info.m_lpModelCharactor,(LPSTR)Socket.m_scSocketName.c_str());

			Info.m_vecSocket.push_back(Socket);

			i_Socket++;
		}

		//////////////////////////////////////////////////////////////////////////
		DWORD i_Stand = 1;
		while(1)
		{
			TCHAR StandName[256];
			wsprintf(StandName,"AniStand%d",i_Stand);

			Ini->GetString(CharName,StandName,"",Name,256);
			::TrimString(Name);
			if(_strcmpi(Name,"")==0)
				break;
			//////////////////////////////////////////////////////////////////////////
			if(Info.m_lpModelCharactor)
			{
				int Index = Info.m_lpModelCharactor->FindAnimation(Name);
				float Ratio = 1.0f;
				
				wsprintf(StandName,"RatioStand%d",i_Stand);
				Ini->GetFloat(CharName,StandName,1.0f,&Ratio);

				Info.m_vecStandAni.push_back(Index);
				Info.m_vecStandAniRatio.push_back(Ratio);

				Info.m_lpModelCharactor->PlayAnimation(Index,PLAY_CIRCLE,1.0f);
			}

			i_Stand++;
		}

		//////////////////////////////////////////////////////////////////////////
		if(Info.m_lpModelCharactor)
		{
			Ini->GetString(CharName,"Walk","",Name,256);
			::TrimString(Name);
			if(_strcmpi(Name,"")==0)
				break;
			Info.m_nAniWalk = Info.m_lpModelCharactor->FindAnimation(Name);

			Ini->GetFloat(CharName,"WalkSpeed",1.0f,&Info.m_fRatioWalkSpeed);
		}

		//////////////////////////////////////////////////////////////////////////
		if(Info.m_lpModelCharactor)
		{
			Ini->GetString(CharName,"Run","",Name,256);
			::TrimString(Name);
			if(_strcmpi(Name,"")==0)
				break;
			Info.m_nAniRun = Info.m_lpModelCharactor->FindAnimation(Name);

			Ini->GetFloat(CharName,"RunSpeed",1.0f,&Info.m_fRatioRunSpeed);
		}

		//////////////////////////////////////////////////////////////////////////
		DWORD i_Attack = 1;
		while(1)
		{
			TCHAR AttackName[256];
			wsprintf(AttackName,"Attack%d",i_Attack);

			Ini->GetString(CharName,AttackName,"",Name,256);
			::TrimString(Name);
			if(_strcmpi(Name,"")==0)
				break;
			//////////////////////////////////////////////////////////////////////////
			if(Info.m_lpModelCharactor)
			{
				int Index = Info.m_lpModelCharactor->FindAnimation(Name);

				Info.m_vecAttackAni.push_back(Index);
			}

			i_Attack++;
		}

		m_vecCharacter.push_back(Info);
		i_Char++;
	}

	Ini->Release();


	if(m_vecCharacter.size()>0)
		m_nCurCharacter = 0;

	/*g_cMeshTable.LoadFromFile(&MeshID,Name);
	g_cMeshTable.GetXMesh(&m_pModelBody,MeshID);
	wsprintf(Name,"%s\\%s",g_Def_ModelDirectory,"主角\\002.Mtl");
	m_pModelBody->LoadMaterial(Name);
	wsprintf(Name,"%s\\%s",g_Def_ModelDirectory,"主角\\002.Ani");
	m_pModelBody->LoadAnimation(Name);

	//m_Scaling = D3DXVECTOR3(1.1f,1.1f,1.1f);

	AddMesh(m_pModelBody->ID);
	//////////////////////////////////////////////////////////////////////////
	//
	m_Stand = m_pModelBody->FindAnimation("站立");
	m_Run   = m_pModelBody->FindAnimation("跑");
	int Attack1 = m_pModelBody->FindAnimation("砍");
	int Attack2 = m_pModelBody->FindAnimation("跳");
	//int Attack3 = m_pModelBody->FindAnimation("攻击03");
	//int Attack4 = m_pModelBody->FindAnimation("攻击04");
	//int Attack5 = m_pModelBody->FindAnimation("攻击05");
	//int Attack6 = m_pModelBody->FindAnimation("攻击06");
	m_vecAttack.push_back(Attack1);
	m_vecAttack.push_back(Attack2);
	//m_vecAttack.push_back(Attack3);
	//m_vecAttack.push_back(Attack4);
	//m_vecAttack.push_back(Attack5);
	//m_vecAttack.push_back(Attack6);

	m_pModelBody->PlayAnimation(m_Stand,1,1.0f);

	wsprintf(Name,"%s\\%s",g_Def_ModelDirectory,"主角\\剑.Mesh");
	g_cMeshTable.LoadFromFile(&MeshID,Name);
	g_cMeshTable.GetXMesh(&m_pModelSword,MeshID);
	wsprintf(Name,"%s\\%s",g_Def_ModelDirectory,"主角\\剑.Mtl");
	m_pModelSword->LoadMaterial(Name);

	m_pModelSword->Bind(m_pModelBody,"R");

	//////////////////////////////////////////////////////////////////////////

	g_cMeshTable.Get1NewXMesh(&m_pModelBelt,MODELTYPE_BELT);
	m_pModelBelt->LoadMesh("");
	m_pModelBelt->Bind(m_pModelBody,"R");

	wsprintf(Name,"%s\\%s",g_Def_ModelDirectory,"骑马\\人.Mesh");
	g_cMeshTable.LoadFromFile(&MeshID,Name);
	g_cMeshTable.GetXMesh(&m_pModelRideBody,MeshID);

	wsprintf(Name,"%s\\%s",g_Def_ModelDirectory,"骑马\\人.Mtl");
	m_pModelRideBody->LoadMaterial(Name);
	wsprintf(Name,"%s\\%s",g_Def_ModelDirectory,"骑马\\人.Ani");
	m_pModelRideBody->LoadAnimation(Name);

	wsprintf(Name,"%s\\%s",g_Def_ModelDirectory,"骑马\\马.Mesh");
	g_cMeshTable.LoadFromFile(&MeshID,Name);
	g_cMeshTable.GetXMesh(&m_pModelHorse,MeshID);

	wsprintf(Name,"%s\\%s",g_Def_ModelDirectory,"骑马\\马.Mtl");
	m_pModelHorse->LoadMaterial(Name);
	wsprintf(Name,"%s\\%s",g_Def_ModelDirectory,"骑马\\马.Ani");
	m_pModelHorse->LoadAnimation(Name);

	m_RideRun = m_pModelRideBody->FindAnimation("跑");
	m_RideStand = m_pModelRideBody->FindAnimation("站立");*/

	return S_OK;
}

HRESULT KRepGirlNo1::FrameMove()
{
	LPSCENE pScene = g_cGraphicsTool.GetCurScene();
	if(!pScene)
		return S_OK;

	if(m_bRiding)
	{
		if(m_bAttacking)
		{
			/*if(m_pModelBody->IsCurAnimationEnd())
			{
				//m_bAttacking = FALSE;
				//m_pModelBody->PlayAnimation(m_Stand,1,1.0f);
			}*/
		}
		else if(m_bMoving2Position)
		{
			D3DXVECTOR3 vector = m_DestPosition - m_Position;
			float beta = 0;

			if ( vector.x < 0.0f )
				beta = -atanf(vector.z/vector.x) + D3DX_PI/2 ;
			else
				beta = -atanf(vector.z/vector.x) - D3DX_PI/2 ;
			
			m_Rotation.x = beta;


			D3DXVECTOR3 Vec(0,0,-m_fSpeed*5);
			
			if(m_bRun)
			{
				Vec.z = -m_fSpeed*5*pScene->m_TimeControl.GetTinyTime();
			}
			else
			{
				Vec.z = -m_fSpeed*2*pScene->m_TimeControl.GetTinyTime();
			}

			D3DXMATRIX Mat;
			D3DXMatrixRotationY(&Mat,m_Rotation.x);
			D3DXVec3TransformCoord(&Vec,&Vec,&Mat);

			m_Position += Vec;

			D3DXVECTOR3 PPP = m_Position-m_DestPosition;
			PPP.y = 0;

			float L = D3DXVec3LengthSq(&PPP);
			if(L<200)
			{
				m_bMoving2Position = FALSE;
				//m_pModelRideBody->PlayAnimation("立",1,1.0f);
				//m_pModelHorse->PlayAnimation("站",1,1.0f);

			}
		}
	}
	else
	{
		if(m_bAttacking)
		{
			if((m_nCurCharacter>=0)&&(m_nCurCharacter<(int)m_vecCharacter.size()))
			{
				_CharacterInfo* pCharacter = &m_vecCharacter[m_nCurCharacter];

				if(!pCharacter->m_lpModelCharactor)
					return S_OK;

				if(pCharacter->m_lpModelCharactor->IsCurAnimationEnd())
				{
					m_bAttacking = FALSE;
					pCharacter->m_lpModelCharactor->PlayAnimation(pCharacter->m_vecStandAni[0],PLAY_CIRCLE,1.0f);
				}
			}
		}
		else if(m_bMoving2Position)
		{
			D3DXVECTOR3 vector = m_DestPosition - m_Position;
			float Ll = D3DXVec3LengthSq(&vector);
			if(Ll>2500)
			{
				float beta = 0;

				if ( vector.x < 0.0f )
					beta = -atanf(vector.z/vector.x) + D3DX_PI/2 ;
				else
					beta = -atanf(vector.z/vector.x) - D3DX_PI/2 ;

				m_Rotation.x = beta;

				m_Position += vector*0.1F;
			}
			else
			{
				m_bMoving2Position = FALSE;

				if((m_nCurCharacter>=0)&&(m_nCurCharacter<(int)m_vecCharacter.size()))
				{
					_CharacterInfo* pCharacter = &m_vecCharacter[m_nCurCharacter];

					if(!pCharacter->m_lpModelCharactor)
						return S_OK;

					pCharacter->m_lpModelCharactor->PlayAnimation(pCharacter->m_vecStandAni[0],PLAY_CIRCLE,1.0f);
				}
			}





			/*D3DXVECTOR3 Vec(0,0,-m_fSpeed*2);
			
			float TinyTime = pScene->m_TimeControl.GetTinyTime();
			if(TinyTime<0.01)
			{
				int s = 0;
			}
			if(m_bRun)
			{
				Vec.z = -m_fSpeed* 0.1f * TinyTime;
			}
			else
			{
				Vec.z = -m_fSpeed* 0.07f * TinyTime;
			}

			D3DXMATRIX Mat;
			D3DXMatrixRotationY(&Mat,m_Rotation.x);
			D3DXVec3TransformCoord(&Vec,&Vec,&Mat);

			m_Position += Vec;

			D3DXVECTOR3 PPP = m_Position-m_DestPosition;
			PPP.y = 0;*/
		}
	}
	
	return S_OK;
}

HRESULT KRepGirlNo1::SetDestPosition(D3DXVECTOR3& Pos)
{
	if(m_bRiding)
	{
		if(!m_bAttacking)
		{
			m_bMoving2Position = TRUE;
			m_DestPosition = Pos;

			/*if((m_pModelRideBody->m_nCurAnimationIndex!=m_RideRun)||m_pModelRideBody->IsCurAnimationEnd())
			{
				m_pModelRideBody->PlayAnimation(m_RideRun,1,1);
				m_pModelHorse->PlayAnimation(m_RideRun,1,1);
			}*/	
		}
	}
	else
	{
		if(!m_bAttacking)
		{
			m_bMoving2Position = TRUE;
			m_DestPosition = Pos;

			if((m_nCurCharacter>=0)&&(m_nCurCharacter<(int)m_vecCharacter.size()))
			{
				_CharacterInfo* pCharacter = &m_vecCharacter[m_nCurCharacter];
				
				if(!pCharacter->m_lpModelCharactor)
					return S_OK;

				if(m_bRun)
				{
					if((pCharacter->m_lpModelCharactor->m_nCurAnimationIndex!=pCharacter->m_nAniRun)||
						pCharacter->m_lpModelCharactor->IsCurAnimationEnd())
					{
						pCharacter->m_lpModelCharactor->PlayAnimation(pCharacter->m_nAniRun,1,pCharacter->m_fRatioRunSpeed*m_fSpeed/5);
					}
				}
				else
				{
					if((pCharacter->m_lpModelCharactor->m_nCurAnimationIndex!=pCharacter->m_nAniWalk)||
						pCharacter->m_lpModelCharactor->IsCurAnimationEnd())
					{
						pCharacter->m_lpModelCharactor->PlayAnimation(pCharacter->m_nAniWalk,1,pCharacter->m_fRatioWalkSpeed*m_fSpeed/5);
					}
				}

			}
			
			//m_Position = Pos;
			/*if((m_pModelBody->m_nCurAnimationIndex!=m_Run)||m_pModelBody->IsCurAnimationEnd())
			{
				m_pModelBody->PlayAnimation(m_Run,1,1);
			}*/	
		}
	}
	
	return S_OK;
}

HRESULT KRepGirlNo1::RenderShadow(DWORD ShadowType)
{
	D3DXMATRIX MatScale,MatRot,MatSave;
	D3DXMatrixScaling(&MatScale,m_Scaling.x,m_Scaling.y,m_Scaling.z);
	D3DXMatrixRotationYawPitchRoll(&MatRot,m_Rotation.x,m_Rotation.y,m_Rotation.z);
	D3DXMatrixMultiply(&m_Matrix,&MatScale,&MatRot);
	m_Matrix._41 = m_Position.x;
	m_Matrix._42 = m_Position.y;
	m_Matrix._43 = m_Position.z;
	g_pd3dDevice->GetTransform(D3DTS_WORLD,&MatSave);
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&m_Matrix);
	
	/*if(m_bRiding)
	{
		m_pModelRideBody->m_dwShadow = 1;
		m_pModelHorse->m_dwShadow = 1;
		m_pModelRideBody->RenderShadow();
		m_pModelHorse->RenderShadow();
	}
	else
	{
		m_pModelBody->m_dwShadow = 1;
		//m_pModelSword->m_dwShadow = 1;
		m_pModelBody->RenderShadow();
	}*/

	g_pd3dDevice->SetTransform(D3DTS_WORLD,&MatSave);
	
	//m_pModelSword->RenderShadow();

	return S_OK;
}

PropertyBase::KPropertyBase* KRepGirlNo1::Acquire()
{
	return m_pModelBlade;
//	return m_pModelBelt;
}

void    KRepGirlNo1::ToggleRun()
{
	m_bRun = !m_bRun;

	if(m_bMoving2Position)
	{
		if((m_nCurCharacter>=0)&&(m_nCurCharacter<(int)m_vecCharacter.size()))
		{
			_CharacterInfo* pCharacter = &m_vecCharacter[m_nCurCharacter];

			if(!pCharacter->m_lpModelCharactor)
				return ;

			if(m_bRun)
			{
				if((pCharacter->m_lpModelCharactor->m_nCurAnimationIndex!=pCharacter->m_nAniRun)||
					pCharacter->m_lpModelCharactor->IsCurAnimationEnd())
				{
					pCharacter->m_lpModelCharactor->PlayAnimation(pCharacter->m_nAniRun,1,pCharacter->m_fRatioRunSpeed);
				}
			}
			else
			{
				if((pCharacter->m_lpModelCharactor->m_nCurAnimationIndex!=pCharacter->m_nAniWalk)||
					pCharacter->m_lpModelCharactor->IsCurAnimationEnd())
				{
					pCharacter->m_lpModelCharactor->PlayAnimation(pCharacter->m_nAniWalk,1,pCharacter->m_fRatioWalkSpeed);
				}
			}
		}
	}
}

HRESULT KRepGirlNo1::Render()
{
	//TCHAR str[30];
	//sprintf(str,"X:%.2f Y:%.2f     ", m_Position.x, m_Position.z);
	//g_cGraphicsTool.DrawText(300,200,1,1,0xFFFF00FF,20,"宋体",str,0);

	D3DXMATRIX MatScale,MatRot,MatSave;
	D3DXMatrixScaling(&MatScale,m_Scaling.x,m_Scaling.y,m_Scaling.z);
	D3DXMatrixRotationYawPitchRoll(&MatRot,m_Rotation.x,m_Rotation.y,m_Rotation.z);
	D3DXMatrixMultiply(&m_Matrix,&MatScale,&MatRot);
	m_Matrix._41 = m_Position.x;
	m_Matrix._42 = m_Position.y;
	m_Matrix._43 = m_Position.z;

	g_pd3dDevice->GetTransform(D3DTS_WORLD,&MatSave);
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&m_Matrix);
	
	if(m_bRiding)
	{
		SAFE_RENDER(m_pModelRideBody);
		SAFE_RENDER(m_pModelHorse);
	}
	else
	{
		if((m_nCurCharacter>=0)&&(m_nCurCharacter<(int)m_vecCharacter.size()))
		{
			_CharacterInfo* pCharacter = &m_vecCharacter[m_nCurCharacter];

			SAFE_RENDER(pCharacter->m_lpModelCharactor);

			for(DWORD i=0;i<pCharacter->m_vecSocket.size();i++)
			{
				_SocketInfo* pSocket = &pCharacter->m_vecSocket[i];
				SAFE_RENDER(pSocket->m_lpModelBind);
			}
		}
		/*SAFE_RENDER(m_pModelBody);
		SAFE_RENDER(m_pModelSword);

		
		ModelSFX::KModelBelt* pBelt = (ModelSFX::KModelBelt*)m_pModelBelt;
		pBelt->m_bShow = m_bAttacking;
		{
		}*/
//		SAFE_RENDER(m_pModelBelt);

		ModelSFX::KModelSFXBlade* pBlade = (ModelSFX::KModelSFXBlade*)m_pModelBlade;
		if(pBlade)
			pBlade->m_bShow = m_bAttacking;
		SAFE_RENDER(m_pModelBlade);

	}
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&MatSave);

	return S_OK;
}

HRESULT KRepGirlNo1::ToggleRide()
{
	m_bRiding = !m_bRiding;
	m_bAttacking = FALSE;
	m_bMoving2Position = FALSE;
	return S_OK;
}
HRESULT KRepGirlNo1::Attack()
{
	if(!m_bRiding)
	{
		if (!m_bAttacking)
		{
			if((m_nCurCharacter>=0)&&(m_nCurCharacter<(int)m_vecCharacter.size()))
			{
				_CharacterInfo* pCharacter = &m_vecCharacter[m_nCurCharacter];

				if(!pCharacter->m_lpModelCharactor)
					return S_OK;

				if(pCharacter->m_vecAttackAni.size()==0)
					return S_OK;

				if((m_pModelBlade)&&(pCharacter->m_vecSocket.size()>0))
				{
					m_pModelBlade->Bind(pCharacter->m_lpModelCharactor,
						(LPSTR)pCharacter->m_vecSocket[0].m_scSocketName.c_str());

				}

				int k = rand()%pCharacter->m_vecAttackAni.size();
				int Attack = pCharacter->m_vecAttackAni[k];

				pCharacter->m_lpModelCharactor->PlayAnimation(Attack,0,1.0f);

				m_bAttacking = TRUE;
				m_bMoving2Position = FALSE;

				TCHAR Name[256];
				wsprintf(Name,"%s\\Music\\Act_A.wav",g_Def_AppDirectory);
				g_pSoundSys->PlayAmusic(Name,PlayNormal);
			}
		}
	}
	
	return S_OK;
}

HRESULT KRepGirlNo1::NextCharacter()
{
	if(m_vecCharacter.size()==0)
	{
		m_nCurCharacter = -1;
	}
	else
	{
		m_nCurCharacter++;
		if(m_nCurCharacter>=(int)m_vecCharacter.size())
		{
			m_nCurCharacter = 0;
		}
	}
	return S_OK;
}
