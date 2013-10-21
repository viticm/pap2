// KModelSceneSFX.h: interface for the KModelSceneSFX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KMODELSCENESFX_H__6191FA81_4A66_4723_8FA2_3A5842E36C86__INCLUDED_)
#define AFX_KMODELSCENESFX_H__6191FA81_4A66_4723_8FA2_3A5842E36C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "XMesh.h"

enum BILLBOARD {NONE , CYLINDRICAL, SPHERICAL,FOG};

struct BBHeader
{
	int m_iWhichBB;
	DWORD	m_dwMaxParticle;
	int m_iMaxWidth,m_iMinWidth;
	int m_iMaxHeight,m_iMinHeight; 
	D3DXVECTOR3 m_fInitSpeed,m_v3AccSpeed;
	D3DXVECTOR3 m_v3InitMinPos,m_v3InitMaxPos;
	float m_fAlphaBase;
};


class KModelSceneSFX : public KModel  
{
public:
	typedef struct Particle
	{
		D3DXVECTOR3   Position;//世界坐标中的位置
		D3DXVECTOR3   Normal;//
		D3DXVECTOR3	  Right;
		D3DXVECTOR3   Up;
		D3DXVECTOR3   NormalOffset;//
		D3DCOLORVALUE Ambient;
		D3DCOLORVALUE Diffuse;
		D3DXVECTOR3   YawPitchRoll;
		D3DXVECTOR3   YawPitchRollSpeed;
		D3DXVECTOR3   YawPitchRollAccelerate;
		D3DXVECTOR3   Speed;
		D3DXVECTOR3   Accelerate;
		float         Size;
		int           TextureUV;
		float         Distance;
		float		  Width;
		float		  Height;
		float		  YInitSpeed;
		float         YAccSpeed;
		float		  XAccSpeed;
		float		  ZAxxSpeed;
		float		  angle;
		float		  DisToView;		//在视点空间里在观察方向上离原点的距离
		float		  SuaiJie;
		int			  PointMove[4];
		float		  MoveSize;
		int			  AlphaChange;		// 0 = alpha增加 1 = alpha减少

		Particle()
		{
			Position = D3DXVECTOR3(0,0,0);
			Normal = D3DXVECTOR3(0,0,0);
			NormalOffset = D3DXVECTOR3(0,1,0);
			Ambient.a = 0.0F;Ambient.r = 0.9F;Ambient.g = 0.9F;Ambient.b = 0.9F;
			Diffuse = Ambient;
			YawPitchRoll = D3DXVECTOR3(0,0,0);
			YawPitchRollSpeed = D3DXVECTOR3(0,0,0);
//			YawPitchRoll = D3DXVECTOR3(0,0,0);
			Size = 50;
			TextureUV = 4;
			Distance = 0;
			Width = 10;
			Height = 50;
			YInitSpeed = 0.5F;
			YAccSpeed = 0.05F;
			XAccSpeed = 0.01F + (3 - rand() % 6) / 500;
			ZAxxSpeed = 0.0;
			Accelerate = D3DXVECTOR3(XAccSpeed,YAccSpeed,ZAxxSpeed);			
//			Speed = D3DXVECTOR3(0,YInitSpeed,0);
			angle = 0.0;
			DisToView = 0.0;
			AlphaChange = 0;
		}
	}*LPPARTICLE;

	list<LPPARTICLE>m_listParticle;//粒子的列表
	list<LPPARTICLE>m_listParticleSort;//用来排序粒子的列表

	LPDIRECT3DVERTEXBUFFER9  m_lpVBuf; 

	DWORD  m_dwNumMaxParticle;//最大的粒子数目
	BOOL   m_bNeedSort;//是否需要排序
	D3DXVECTOR3 m_fInitSpeed;
	float	m_fYSpeed;		// y方向上的速度
	float	m_fCountTime;
	D3DXVECTOR3 m_v3AccSpeed;
	int		m_iWhichBB;
	int		m_iMinWidth;
	int		m_iMinHeight;
	int		m_iMaxWidth;
	int		m_iMaxHeight;
	D3DXVECTOR3 m_v3InitMinPos;
	D3DXVECTOR3 m_v3InitMaxPos;
	D3DXMATRIX m_mScale;

	GraphicsStruct::KCamera  m_camera;
	AABBOX m_AABBox;
	float m_fFogNearPos,m_fFogFarPos;
	float m_fTimeStep;
	float m_fTime[3];
	int m_iFPS;
	float m_fAlphaBase;

public:
	KModelSceneSFX();
	virtual ~KModelSceneSFX();

	HRESULT Render();
	HRESULT LoadMesh(LPSTR pFileName);
	HRESULT SaveMesh(LPSTR pFileName);

	HRESULT CreateVerticesBuffer();
	HRESULT UpdateVerticesBuffer();
	HRESULT UpdateParticle();

	LPPARTICLE Add1Particle();
	HRESULT DeleteParticle(LPPARTICLE pLeaf);
	HRESULT CleanUp();

	HRESULT SetMaxParticle(DWORD Num);
	void   CalDisToView(LPPARTICLE p,const D3DXVECTOR3 &front,const D3DXVECTOR3 &camerapos);
	void CalFPS();
	inline HRESULT SetAccSpeed(const D3DXVECTOR3 &accspeed)
	{
		m_v3AccSpeed = accspeed;

		return S_OK;
	}
	inline HRESULT SetWhichBB(const int &whichbb)
	{
		m_iWhichBB = whichbb;

		return S_OK;
	}
	inline HRESULT SetHeight(const int &minheight,const int &maxheight)
	{
		m_iMinHeight = minheight;
		m_iMaxHeight = maxheight;

		list<LPPARTICLE>::iterator i = m_listParticle.begin();
		LPPARTICLE pLeaf;

		while (i != m_listParticle.end())
		{
			pLeaf = *i;

			int dwidth = m_iMaxWidth - m_iMinWidth;
			int dheight = m_iMaxHeight - m_iMinHeight;

			if(dwidth > 0)
				pLeaf->Width = (float)m_iMinWidth + rand() % dwidth;
			else
				pLeaf->Width = (float)m_iMinWidth;

			if(dheight > 0)
				pLeaf->Height = (float)m_iMinHeight + rand() % dheight;
			else
				pLeaf->Height = (float)m_iMinHeight;

			i++;
		}

		return S_OK;

	}
	inline HRESULT SetWidth(const int &minwidth,const int &maxwidth)
	{
		m_iMinWidth = minwidth;
		m_iMaxWidth = maxwidth;

		list<LPPARTICLE>::iterator i = m_listParticle.begin();
		LPPARTICLE pLeaf;

		while (i != m_listParticle.end())
		{
			pLeaf = *i;

			pLeaf->Width = (float)(m_iMinWidth + rand() % (m_iMaxWidth - m_iMinWidth));
			pLeaf->Height = (float)(m_iMinHeight + rand() % (m_iMaxHeight - m_iMinHeight));

			i++;
		}

		return S_OK;
	}
	inline HRESULT SetInitSpeed(const D3DXVECTOR3 &speed)
	{
		m_fInitSpeed = speed;

		return S_OK;
	}
	inline HRESULT SetInitMinPos(const D3DXVECTOR3 &initminpos)
	{
		m_v3InitMinPos = initminpos;

		list<LPPARTICLE>::iterator i = m_listParticle.begin();
		LPPARTICLE pLeaf;

		while (i != m_listParticle.end())
		{
			pLeaf = *i;

			pLeaf->Position.x = (float)m_v3InitMinPos.x + rand() % (int)(m_v3InitMaxPos.x - m_v3InitMinPos.x);//rand()%2000 - 1000.0f;
			pLeaf->Position.y = (float)m_v3InitMinPos.y + rand() % (int)(m_v3InitMaxPos.y - m_v3InitMinPos.y);//rand() % 500 + 500;
			pLeaf->Position.z = (float)m_v3InitMinPos.z + rand() % (int)(m_v3InitMaxPos.z - m_v3InitMinPos.z);//rand()%2000 - 1000.0f;

			i++;
		}

		return S_OK;
	}
	inline HRESULT SetInitMaxPos(const D3DXVECTOR3 &initmaxpos)
	{
		m_v3InitMaxPos = initmaxpos;

		list<LPPARTICLE>::iterator i = m_listParticle.begin();
		LPPARTICLE pLeaf;

		while (i != m_listParticle.end())
		{
			pLeaf = *i;

			pLeaf->Position.x = (float)m_v3InitMinPos.x + rand() % (int)(m_v3InitMaxPos.x - m_v3InitMinPos.x);//rand()%2000 - 1000.0f;
			pLeaf->Position.y = (float)m_v3InitMinPos.y + rand() % (int)(m_v3InitMaxPos.y - m_v3InitMinPos.y);//rand() % 500 + 500;
			pLeaf->Position.z = (float)m_v3InitMinPos.z + rand() % (int)(m_v3InitMaxPos.z - m_v3InitMinPos.z);//rand()%2000 - 1000.0f;

			i++;
		}

		return S_OK;
	}
	inline HRESULT SetAlphaBase(const float alphabase)
	{
		m_fAlphaBase = alphabase;

		return S_OK;
	}

	void LoadData(const char *FileDir);


	D3DXVECTOR3 VectorMuliMatrix(const D3DXVECTOR3 *pV3,const D3DXMATRIX *pMatrix);
	void MakeViewM(D3DXMATRIX *matrix,const D3DXVECTOR3 *front,const D3DXVECTOR3 *left,
				   const D3DXVECTOR3 *up);
	void CalFogPosition(const float &Near,const float &Far);
};

#endif // !defined(AFX_KMODELSCENESFX_H__6191FA81_4A66_4723_8FA2_3A5842E36C86__INCLUDED_)
