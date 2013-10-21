#include "stdafx.h"
#include "ParticleSystem.h"
#include ".\particlesystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


namespace ModelSFX
{


//void GetPointOnCircle( D3DXVECTOR3 &vPt, int iOffset, float fStep, const D3DXVECTOR3 &vNormal, const D3DXVECTOR3 &vCenter, float fRadius)
//{
//	D3DXVECTOR3 tmp(0.0f, 1.0f, 0.0f);
//	D3DXVECTOR3 axis1, axis2;
//	if (vNormal == tmp)
//	{
//		tmp = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
//	}
//	D3DXVec3Cross(&axis1, &vNormal, &tmp);
//	D3DXVec3Cross(&axis2, &vNormal, &axis1);
//	vPt = sin(iOffset * fStep) * axis1 + cos(iOffset * fStep) * axis2;
//	vPt += vCenter;
//}



bool KParticleSystem::SaveLaunParToFile(LPCSTR strFileName, int iIndex)
{
	FILE* fp = NULL;
//	int i;
	if( !strFileName )
		return false;
	fp = fopen(strFileName, "w");
	if( fp == NULL )
	{
		return false;
	}
	fwrite(m_vecLauncher[iIndex], 200, 1, fp);

	//for ( i = 0; i < 5; i++)
	//{
	//	//Normally the initialized time_point contains 2 element, before user adding any time_point
	//	int iSize = m_vecLauncher[iIndex]->m_tpt[i].m_vecTP.size();
	//	fwrite(&iSize, sizeof(int), 1, fp);
	//	if ( !m_vecLauncher[iIndex]->m_cTimePoint[i].SaveToFile(fp))
	//		return false;
	//}
	fclose(fp);

	return true;

}


//////////////////////////////////////////////////////////////////////////////////////////////////
//Update();
//更新系统
void KParticleSystem::Update()
{
	float fTime = GetCurrentTime();
	//if ( fTime - m_fLastUpdate < 1 / 80 )//假设为80fps
	//	return;
	int i;
	int iLen = m_vecLauncher.size();
	for ( i = 0; i < iLen; i++ )
	{
		KLauncher *pLan= m_vecLauncher[i];

		//int iPLen = pLan->m_vecParticles.size();

		float t = fTime - pLan->m_fLastUpdate;
		pLan->m_fLastUpdate = fTime;
		pLan->m_vDis = t * pLan->m_vVelocity;
		pLan->m_vec3Translate += pLan->m_vDis;
		//Update ShapeParameter
		switch( pLan->m_eShape )
		{
		case L_POINT:
		case L_CIRCLE:
		case L_CIRCLE_FACE:
			break;
		case L_RECTANGLE_FACE:
		case L_RECTANGLE:
			{
				int j;
				for ( j = 0; j < 18; j+=3 )
				{
					pLan->m_fShapePar[j] += pLan->m_vDis.x;
					pLan->m_fShapePar[j] += pLan->m_vDis.y;
					pLan->m_fShapePar[j] += pLan->m_vDis.z;
				}
				D3DXVECTOR3 *A = (D3DXVECTOR3*)&pLan->m_fShapePar[0];
				D3DXVECTOR3 *B = (D3DXVECTOR3*)&pLan->m_fShapePar[3];
				D3DXVECTOR3 *C = (D3DXVECTOR3*)&pLan->m_fShapePar[6];
				D3DXVec3Cross(&pLan->m_vec3Direction, &(*A - *B), &(*A - *C));
			}
			break;
		}

		//刷新粒子

		pLan->UpdateParticles();
		pLan->m_fLastUpdate = fTime;

	}
	//m_fLastUpdate = fTime;
}


KParticleSystem::KParticleSystem()
{
	srand((unsigned int)GetTickCount());
	Init();
	//SetLauncher(0, false);
	m_fLastUpdate = GetCurrentTime();
	//fpdebug = fopen("d:\\debug.txt", "w");
}
KParticleSystem::~KParticleSystem()
{
	int iSize = static_cast<int>(m_vecLauncher.size());
	int i;
	for( i = 0; i < iSize; i++ )
		delete m_vecLauncher[i];
	iSize = static_cast<int>(m_vecBarrier.size());
	for( i = 0; i < iSize; i++ )
		delete m_vecBarrier[i];
	iSize = static_cast<int>(m_vecForce.size());
	for( i = 0; i < iSize; i++ )
		delete m_vecForce[i];
	m_vecLauncher.clear();
	m_vecBarrier.clear();
	m_vecForce.clear();
	//fclose(fpdebug);
	//m_ini.SaveToFile(".\ParticleSystem.ini");

}

///////////////////////////////////////////////////////////////////////////////////
//函数:Init();
//作用:初始化整个系统
//////////////////////////////////////////////////////////////////////////////////
void KParticleSystem::Init()
{
	//m_ini.Init();
	//m_ini.LoadFromFile(".\\ParticleSystem.ini");
	m_bForce = false;
	m_bBarrier = false;

	//InitLauncher
	KLauncher *pLan = new KLauncher;
	pLan->m_pBarrier = &m_vecBarrier;
	pLan->m_pForceField = &m_vecForce;
	pLan->SetStatus(TRUE);
	AddLauncher(pLan);
	//pLan->SetStatus()
	/*pLan = new KLauncher;
	lan1.m_pBarrier = &m_vecBarrier;
	lan1.m_pForceField = &m_vecForce;
	lan1.m_bLaunch = false;*/

	//AddLauncher(pLan);


	//InitBa
	//float Parameter[20] = { -400, 600, 400, 400, 600, 400, 400, 600, -400, -400, 600, -400};
	//KBarrier ba;//(Parameter, B_TREFLECT, B_PLANE);
	//m_vecBarrier.push_back(ba);
	KBarrier* pba = new KBarrier;
	AddBarrier(pba);

	m_bBarrier = true;

	//InitForce
	KForceField *ff = new KForceField(F_GRAVITY);
	AddForce(ff);
	m_bForce = true;
	//ff->SetForceField(false);

	/*KForceField* ff1 = new KForceField(F_RADIAL);
	AddForce(ff1);*/

}


bool KParticleSystem::LoadLaunFromFile(LPCSTR strFileName)
{
	KLauncher *pLan = new KLauncher;
	int i;
	TimePoint* tp;
	FILE* fp;
	fp = fopen( strFileName, "r");
	if ( !fp )
		return false;
	fread(pLan, 200, 1, fp);
	for (i = 0; i < 5; i++ )
	{
		int iSize;
		fread(&iSize, sizeof(int), 1, fp);
		if ( iSize )
		{
			tp = new TimePoint[iSize];
			if ( !tp )
			return false;
			fread(tp, sizeof(TimePoint), iSize, fp);
		}
		//pLan->m_cTimePoint[i].Insert(tp, iSize);
		delete[] tp;
	}
	fclose(fp);
	pLan->m_fLastUpdate = GetCurrentTime();
	AddLauncher(pLan);
	return true;

}

float KParticleSystem::Dif_Sin(float fTime, float fCycle,float fWing)
{
	return sin(fTime * fCycle) * fWing;
}






HRESULT KParticleSystem::FrameMove(void)
{
	Update();
	return S_OK;
}




vector<KBarrier*>* KParticleSystem::GetBarrier(void)
{
	return &m_vecBarrier;
}

vector<KForceField*>* KParticleSystem::GetForceField(void)
{
	return &m_vecForce;
}

};
