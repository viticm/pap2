// KRepNPC.cpp: implementation of the KRepNPC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KRepNPC.h"
#include "sceneeditor.h"
#include "KSO3World.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KRepNPC::KRepNPC()
{
	m_lpModelBody = NULL;
	m_lpModelRHand = NULL;
	m_lpModelLHand = NULL;
	m_pNpcTemplate = NULL;
	m_dwLogicalScriptID = 0;

	m_nLogicalKind = 0;
	m_nLogicalLevel = 0;
	m_szLogicalScriptFile = "";
	m_szLogicalName = "";
}

KRepNPC::~KRepNPC()
{

}

HRESULT KRepNPC::Copy(KComponentObject** pNewObject)
{
	KRepNPC* pNPC = NULL;

	g_cObjectTable.Get1NewObject((KComponentObject**)(&pNPC),OBJECTTYPE_NPC);
	
	pNPC->m_lpModelBody = m_lpModelBody;
	pNPC->m_lpModelLHand = m_lpModelLHand;
	pNPC->m_lpModelRHand = m_lpModelRHand;

	pNPC->m_vecAniStand = m_vecAniStand;
	
	pNPC->m_Position = m_Position;
	pNPC->m_Rotation = m_Rotation;
	pNPC->m_Scaling = m_Scaling;

	pNPC->m_vecMeshKeeper = m_vecMeshKeeper;
	pNPC->ComputeBBox();

	pNPC->m_scIniFileName = m_scIniFileName;

	(*pNewObject) = (KComponentObject*) pNPC;

	pNPC->m_pNpcTemplate = m_pNpcTemplate;

	return S_OK;
}

HRESULT KRepNPC::FrameMove()
{
	if(!m_lpModelBody)
		return S_OK;
	if (m_lpModelBody->IsCurAnimationEnd())
	{
		float frand = rand()%1000*0.001f;
		for(DWORD i=0;i<m_vecAniStand.size();i++)
		{
			float T = m_vecAniStand[i].fRatio;
			if(T>=frand)
			{
				m_lpModelBody->PlayAnimation(m_vecAniStand[i].nAniIndex,1,1.0f);
				break;
			}
		}
	}
	return KComponentObject::FrameMove();
}

HRESULT KRepNPC::Render()
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
	
	SAFE_RENDER(m_lpModelBody);
	SAFE_RENDER(m_lpModelLHand);
	SAFE_RENDER(m_lpModelRHand);

	g_pd3dDevice->SetTransform(D3DTS_WORLD,&MatSave);

	return S_OK;
}

HRESULT KRepNPC::LoadByIni(IIniFile* pIniFile,LPSTR pFileName)
{
	DWORD MeshID = 0;
	TCHAR OutName[256];
	TCHAR InName[256];

	m_scIniFileName = pFileName;
	//////////////////////////////////////////////////////////////////////////
	//Body
	if(pIniFile->IsSectionExist("Body"))
	{
		if(pIniFile->GetString("Body","Mesh ","",InName,256))
		{
			::TrimString(InName);
			if(SUCCEEDED(g_cFileFinder.FindFile(OutName,InName)))
			{
				g_cMeshTable.Get1NewXMesh(&m_lpModelBody,0);

				m_lpModelBody->LoadMesh(OutName);
				AddMesh(m_lpModelBody->ID);
			}
		}
		if(pIniFile->GetString("Body","Material ","",InName,256))
		{
			::TrimString(InName);
			if(SUCCEEDED(g_cFileFinder.FindFile(OutName,InName)))
			{
				if (m_lpModelBody)
				{
					m_lpModelBody->LoadMaterial(OutName);
				}
			}
		}
		if(pIniFile->GetString("Body","Animation ","",InName,256))
		{
			::TrimString(InName);
			if(SUCCEEDED(g_cFileFinder.FindFile(OutName,InName)))
			{
				if (m_lpModelBody)
				{
					m_lpModelBody->LoadAnimation(OutName);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//R hand
	if(pIniFile->IsSectionExist("R_Hand"))
	{
		if(pIniFile->GetString("R_Hand","Mesh ","",InName,256))
		{
			::TrimString(InName);
			if(SUCCEEDED(g_cFileFinder.FindFile(OutName,InName)))
			{
				g_cMeshTable.Get1NewXMesh(&m_lpModelRHand,0);
				m_lpModelRHand->LoadMesh(OutName);

				//g_cMeshTable.LoadFromFile(&MeshID,OutName);
				//g_cMeshTable.GetXMesh(&m_lpModelRHand,MeshID);

				//AddMesh(m_lpModelRHand->ID);
			}
		}
		if(pIniFile->GetString("R_Hand","Material ","",InName,256))
		{
			::TrimString(InName);
			if(SUCCEEDED(g_cFileFinder.FindFile(OutName,InName)))
			{
				if ((m_lpModelBody)&&(m_lpModelRHand))
				{
					m_lpModelRHand->LoadMaterial(OutName);
				}
			}
		}
		if(pIniFile->GetString("R_Hand","Animation ","",InName,256))
		{
			::TrimString(InName);
			if(SUCCEEDED(g_cFileFinder.FindFile(OutName,InName)))
			{
				if ((m_lpModelBody)&&(m_lpModelRHand))
				{
					m_lpModelRHand->LoadAnimation(OutName);
				}
			}
		}
		if(pIniFile->GetString("R_Hand","SocketName ","",InName,256))
		{
			::TrimString(InName);

			if ((m_lpModelBody)&&(m_lpModelRHand))
			{
				m_lpModelRHand->Bind(m_lpModelBody,InName);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//L hand
	if(pIniFile->IsSectionExist("L_Hand"))
	{
		if(pIniFile->GetString("L_Hand","Mesh ","",InName,256))
		{
			::TrimString(InName);
			if(SUCCEEDED(g_cFileFinder.FindFile(OutName,InName)))
			{
				g_cMeshTable.Get1NewXMesh(&m_lpModelLHand,0);
				m_lpModelLHand->LoadMesh(OutName);
				//g_cMeshTable.GetXMesh(&m_lpModelLHand,MeshID);

				//AddMesh(m_lpModelLHand->ID);
			}
		}
		if(pIniFile->GetString("L_Hand","Material ","",InName,256))
		{
			::TrimString(InName);
			if(SUCCEEDED(g_cFileFinder.FindFile(OutName,InName)))
			{
				if((m_lpModelBody)&&(m_lpModelLHand))
				{
					m_lpModelLHand->LoadMaterial(OutName);
				}
			}
		}
		if(pIniFile->GetString("L_Hand","Animation ","",InName,256))
		{
			::TrimString(InName);
			if(SUCCEEDED(g_cFileFinder.FindFile(OutName,InName)))
			{
				if((m_lpModelBody)&&(m_lpModelLHand))
				{
					m_lpModelLHand->LoadAnimation(OutName);
				}
			}
		}
		if(pIniFile->GetString("L_Hand","SocketName ","",InName,256))
		{
			::TrimString(InName);

			if((m_lpModelBody)&&(m_lpModelLHand))
			{
				m_lpModelLHand->Bind(m_lpModelBody,InName);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//Stand Animation
	if(pIniFile->IsSectionExist("Ani_Stand"))
	{
		TCHAR KeyName[256];
		for(int i=1;i<100;i++)
		{
			wsprintf(KeyName,"Animation%d ",i);
			if(pIniFile->GetString("Ani_Stand",KeyName,"",InName,256))
			{
				::TrimString(InName);
				if(_strcmpi(InName,"")!=0)
				{
					_AnimationInfo Ani;
					Ani.scName = InName;
					
					wsprintf(KeyName,"Ratio%d ",i);
					pIniFile->GetFloat("Ani_Stand",KeyName,1.0f,&Ani.fRatio);

					if(m_lpModelBody)
					{
						Ani.nAniIndex = m_lpModelBody->FindAnimation(InName);
						if(Ani.nAniIndex==-1)
						{
							//MessageBox(g_hBaseWnd,InName,"No This Animation",0);
						}
						else
						{
							//m_lpModelBody->PlayAnimation(Ani.nAniIndex,1,1.0f);

							m_vecAniStand.push_back(Ani);
						}
					}

				}
			}
			else
			{
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//animation ratio
	float Sum = 0;
	for(DWORD i=0;i<m_vecAniStand.size();i++)
	{
		Sum += m_vecAniStand[i].fRatio;
	}
	float K =0;
	for(DWORD i=0;i<m_vecAniStand.size();i++)
	{
		float S = m_vecAniStand[i].fRatio/Sum;
		m_vecAniStand[i].fRatio = S + K;
		K += S;
	}
	//////////////////////////////////////////////////////////////////////////
	ComputeBBox();
	return S_OK;
}

BOOL KRepNPC::SelectByPickLine(float* Distance,D3DXVECTOR3* pOrg,D3DXVECTOR3* pDir)
{
	D3DXVECTOR3 A = *pOrg;
	D3DXVECTOR3 B = A + (*pDir)*100000;
	D3DXVECTOR3 Inter;

	D3DXVECTOR3 BoxPos[8];
	m_BBox.TransformCoord(BoxPos,&m_Matrix);
	if(::IsLineIntersectBox(&Inter,&A,&B,BoxPos))
	{
		D3DXVECTOR3 Len = A - Inter;
		float Length = D3DXVec3Length(&Len);
		(*Distance) = Length;
		return TRUE;
	}
	return FALSE;
}

HRESULT KRepNPC::GetLogicalPosition(int* pX,int* pZ,AABBOX* pBox)
{
	(*pX) = (int)((m_Position.x-pBox->A.x) * 256 / 100);
	(*pZ) = (int)((m_Position.z-pBox->A.z) * 256 / 100);	
	return S_OK;
}

HRESULT KRepNPC::GetLogicalKind(int* pKind)
{
	if(m_nLogicalKind!=0)
	{
		*pKind = m_nLogicalKind;
		return S_OK;
	}

	KGLOG_PROCESS_ERROR(pKind);
	KGLOG_PROCESS_ERROR(m_pNpcTemplate);

	*pKind = m_pNpcTemplate->m_Kind;

	return S_OK;

Exit0:

	return E_FAIL;
}

HRESULT KRepNPC::GetLogicalTemplateID(DWORD* pID)
{
	KGLOG_PROCESS_ERROR(pID);
	KGLOG_PROCESS_ERROR(m_pNpcTemplate);

	*pID = m_pNpcTemplate->m_dwTemplateID;

	return S_OK;

Exit0:

	return E_FAIL;
}

HRESULT KRepNPC::GetLogicalName(char* pName)
{
	if(_strcmpi(m_szLogicalName.c_str(),"")!=0)
	{
		strncpy(pName, m_szLogicalName.c_str(), _NAME_LEN);
		return S_OK;
	}
	KGLOG_PROCESS_ERROR(pName);
	KGLOG_PROCESS_ERROR(m_pNpcTemplate);

	strncpy(pName, m_pNpcTemplate->m_szName, _NAME_LEN);
	pName[_NAME_LEN - 1] = 0;

	return S_OK;

Exit0:

	return E_FAIL;
}

HRESULT KRepNPC::SetLogicalName(char* pName)
{
	m_szLogicalName = pName;
	return S_OK;
}

HRESULT KRepNPC::GetLogicalLevel(int* pLevel)
{
	if(m_nLogicalLevel!=0)
	{
		*pLevel = m_nLogicalLevel;
		return S_OK;
	}

	KGLOG_PROCESS_ERROR(pLevel);
	KGLOG_PROCESS_ERROR(m_pNpcTemplate);

	*pLevel = m_pNpcTemplate->m_nLevel;

	return S_OK;

Exit0:

	return E_FAIL;
}

HRESULT KRepNPC::SetLogicalLevel(int Level)
{
	m_nLogicalLevel = Level;
	return S_OK;
}

HRESULT KRepNPC::GetLogicalScriptID(DWORD* pdwScriptID)
{
	KGLOG_PROCESS_ERROR(pdwScriptID);
	KGLOG_PROCESS_ERROR(m_pNpcTemplate);

	*pdwScriptID = m_dwLogicalScriptID;

	return S_OK;

Exit0:

	return E_FAIL;
}

HRESULT KRepNPC::SetLogicalScript(char* pFileName)
{
	m_szLogicalScriptFile = pFileName;
	
	char szFileName[MAX_PATH];
	g_ExtractFileName(szFileName, (LPSTR)pFileName);
	m_dwLogicalScriptID = g_FileName2Id(szFileName);

	return S_OK;
}