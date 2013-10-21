// KRepGirlNo1.h: interface for the KRepGirlNo1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KREPGIRLNO1_H__D0FE35D1_A8B2_433E_B704_6557FFEC771E__INCLUDED_)
#define AFX_KREPGIRLNO1_H__D0FE35D1_A8B2_433E_B704_6557FFEC771E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataObject.h"

class KRepGirlNo1 : public KComponentObject  
{
public:
	struct _SocketInfo
	{
		string    m_scSocketName;
		string    m_scBindMeshFile;
		string    m_scBindMtlFile;
		string    m_scBindAniFile;	
		LPMODEL   m_lpModelBind;
		_SocketInfo()
		{
			m_lpModelBind = NULL;
		}
	};
	struct _CharacterInfo
	{
		string    m_scMeshFile;
		string    m_scMtlFile;
		string    m_scAniFile;
		LPMODEL   m_lpModelCharactor;

		vector<_SocketInfo>m_vecSocket;

		vector<int>  m_vecStandAni;
		vector<float>m_vecStandAniRatio;

		int          m_nAniWalk;
		int          m_nAniRun;
		float        m_fRatioWalkSpeed;
		float        m_fRatioRunSpeed;

		vector<int>  m_vecAttackAni;
		int          m_nAniSit;

		_CharacterInfo()
		{
			m_nAniWalk = 0;
			m_nAniRun  = 0;
			m_fRatioWalkSpeed = 0;
			m_fRatioRunSpeed  = 0;
			m_nAniSit = 0;

			m_lpModelCharactor = NULL;
		}
	};

	vector<_CharacterInfo>m_vecCharacter;

	int  m_nCurCharacter;
typedef int ANIINDEX;

	ANIINDEX   m_Stand;
	ANIINDEX   m_Run;
	ANIINDEX   m_Walk;
	//ANIINDEX   m_Attack1;
	//ANIINDEX   m_Attack2;
	ANIINDEX   m_RideRun;
	ANIINDEX   m_RideStand;

	vector<ANIINDEX>m_vecStand;
	vector<ANIINDEX>m_vecAttack;

	BOOL       m_bRiding;
	BOOL       m_bRun;

	BOOL       m_bAttacking;
	DWORD      m_dwState;//0 stand,1 walk ,2 run ,3 pre attack ,4 attack ,5 attack waiting,6 end attack 
	
	float    m_fSpeed;
	float    m_fRotationSpeed;

	LPMODEL  m_pModelBody;
	LPMODEL  m_pModelSword;
	LPMODEL  m_pModelBlade;

	LPMODEL  m_pModelBelt;

	BOOL     m_bMoving2Position;
	D3DXVECTOR3 m_DestPosition;

	LPMODEL  m_pModelRideBody;
	LPMODEL  m_pModelHorse;

public:
	void    ToggleRun();
	HRESULT NextCharacter();
	virtual PropertyBase::KPropertyBase* Acquire();

	HRESULT RenderShadow(DWORD ShadowType);
	HRESULT ToggleRide();

	HRESULT SetDestPosition(D3DXVECTOR3& Pos);
	HRESULT FrameMove();
	HRESULT Render();
	HRESULT Load();
	HRESULT Attack();

	KRepGirlNo1();
	virtual ~KRepGirlNo1();

};

#endif // !defined(AFX_KREPGIRLNO1_H__D0FE35D1_A8B2_433E_B704_6557FFEC771E__INCLUDED_)
