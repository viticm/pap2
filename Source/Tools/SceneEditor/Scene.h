// Scene.h: interface for the K3DScene class.
//
//////////////////////////////////////////////////////////////////////
#include <list>
using namespace std;

#if !defined(AFX_SCENE_H__04D7C28D_AE40_44C3_869A_F3505B306E5F__INCLUDED_)
#define AFX_SCENE_H__04D7C28D_AE40_44C3_869A_F3505B306E5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  SCENE_DEFAULT        0
#define  SCENE_SKINEDITOR     1
#define  SCENE_MESHEDITOR     2
#define  SCENE_OBJECTEDITOR   3

#define  MSG_PROCESSED   1

#define  SCENE_SWORDTEST    100

//#include "XMeshTable.h"
//#include "ObjectTable.h"
//#include "KEntity.h"
#include <algorithm>
//#include "KPhysicsForceField.h"
#include "IEKG3DScene.h"

typedef class K3DScene : public IEKG3DScene
{
public:
	class TimeControl
	{
	protected:
		float m_fSpeedBack;
		float m_fSpeedDest;
		float m_fSpeedIncrease;
	public:
		float m_fNowTime;
		float m_fLastTime;
		float m_fSpeed;
		DWORD m_dwTimeGetTimeLast;

		TimeControl()
		{
			m_fSpeedBack = 0;
			m_fNowTime   = 0;
			m_fLastTime  = 0;
			m_fSpeed     = 1.0f;
			m_fSpeedDest = 1.0f;
			m_dwTimeGetTimeLast = 0;
			m_fSpeedIncrease = 0.1f;
		}
		void FrameMove()
		{
			m_fSpeed += (m_fSpeedDest - m_fSpeed)*m_fSpeedIncrease;

			if(m_dwTimeGetTimeLast)
			{
				DWORD NowTime = timeGetTime();
				m_fLastTime = m_fNowTime;

				if(NowTime-m_dwTimeGetTimeLast<100)
				{
					m_fNowTime += (NowTime-m_dwTimeGetTimeLast)*m_fSpeed;
					//return;
				}
				m_dwTimeGetTimeLast = NowTime;
			}
			else
			{
				m_dwTimeGetTimeLast = timeGetTime();
			}
		}

		float GetNowTime()
		{
			return m_fNowTime;
		}

		float GetTinyTime()
		{
			return m_fNowTime - m_fLastTime;
		}
		void Pause()
		{
			m_fSpeedBack = m_fSpeed;
			m_fSpeed = 0;
		}
		void Restore()
		{
			m_fSpeed = m_fSpeedBack;
		}
		void SetDestSpeed(float Value,float Increate = 0.1f)
		{
			m_fSpeedIncrease = Increate;
			m_fSpeedDest = Value;
		}
	}m_TimeControl;
	struct _FogInfo
	{
		BOOL       m_bFogEnable;

		D3DCOLOR   m_FogColor;
		int        m_nFogMode;

		BOOL       m_bCanDoTableFog;
		BOOL       m_bCanDoVertexFog;
		BOOL       m_bCanDoWFog;
		BOOL       m_bDeviceUsesWFog;
		BOOL       m_bRangeBasedFog;
		BOOL       m_bUsingTableFog;

		FLOAT      m_fFogStartValue;
		FLOAT      m_fFogEndValue;
		FLOAT      m_fFogDensity;

		_FogInfo()
		{
			m_FogColor             = 0x00b5b5ff;
			m_nFogMode             = D3DFOG_LINEAR;
			m_bCanDoTableFog       = FALSE;
			m_bCanDoVertexFog      = FALSE;
			m_bCanDoWFog           = FALSE;
			m_bDeviceUsesWFog      = FALSE;
			m_bRangeBasedFog       = FALSE;
			m_bUsingTableFog       = FALSE;

			m_fFogStartValue       = 2000.0f;
			m_fFogEndValue         = 5000.0f;
			m_fFogDensity          = 0.0f;

			m_bFogEnable = FALSE;
		}

		HRESULT ApplyFog();
	}m_Fog;

	struct _GlowInfo
	{
		DWORD m_GateColor;
		DWORD m_AddColor;
		_GlowInfo()
		{
			m_GateColor = 0xFF808080;
			m_AddColor  = 0x80FFFFFF;
		}
	}m_Glow;

	typedef list<KSceneEntity>ENTITYLIST;
	//ENTITYLIST m_RenderEntityList;
	list<KSceneEntity>m_RenderEntityList;
	
	DWORD    ID;
	DWORD    m_dType;
	float    m_TimeLast;

	D3DCOLOR    m_Ambient;
	D3DLIGHT9   m_SunLight;

	D3DXVECTOR3 m_SunDirection;

	KPhysicsForceFieldManager  m_ForceFieldManager;//力场,重力

public:
	virtual HRESULT  GetHeight(D3DXVECTOR3* pOut,D3DXVECTOR2* pIN,DWORD Type);

	virtual HRESULT  RestoreDeviceObjects(void);
	virtual HRESULT  InvalidateDeviceObjects(void);

	virtual D3DXVECTOR3 GetSunDirection();
	virtual HRESULT SetLighting();

	BOOL     CheckRenderObject(LPOBJECT pObject);
	HRESULT  RemoveRenderMesh(LPMODEL pMesh);
	HRESULT  RemoveRenderObject(LPOBJECT pObject);
	HRESULT  AddRenderObject(LPOBJECT pObject);
	HRESULT  AddRenderMesh(KModel* pMesh);
	virtual  HRESULT DrawRenderEntity(LPSCENEENTITY pObj);
	virtual  HRESULT DrawRenderEntityShadow(LPSCENEENTITY pObj);
	virtual  HRESULT FrameMove();
	virtual  HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
	virtual  HRESULT Render();
	virtual  HRESULT Initialize(PVOID pSceneTable);

	K3DScene();
	virtual ~K3DScene();

	HRESULT AddRenderEntity(SCENEENTITYTYPE Type,PVOID pEntity);
	HRESULT RemoveRenderEntity(SCENEENTITYTYPE Type,PVOID pEntity);
	HRESULT K3DScene::RemoveRenderEntity(KSceneEntity Entity);

	virtual HRESULT CleanUp(void);
}* LPSCENE;

#endif // !defined(AFX_SCENE_H__04D7C28D_AE40_44C3_869A_F3505B306E5F__INCLUDED_)
