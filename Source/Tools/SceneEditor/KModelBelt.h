#pragma once
//#include "xmesh.h"

namespace ModelSFX
{

class KModelBelt :
	public KModel
{
private:
	typedef struct _SrcLine
	{
		float Distance;
		D3DXVECTOR3 A;
		D3DXVECTOR3 B;

		_SrcLine()
		{
			A  = D3DXVECTOR3(0,0,0);
			B  = D3DXVECTOR3(0,0,0);
			Distance = 0;
		}
	}SRCLINE,* LPSRCLINE;

	HRESULT CreateVerticesBuffer();
	LPDIRECT3DVERTEXBUFFER9  m_lpVBuf;
	BOOL m_bGotOne;	//本变量用于初始化飘带,	控制只调用InitialBelt()函数一次
	SRCLINE  m_CurLine;  

	list<SRCLINE>m_SrcLineList;
	vector<SRCLINE>m_vecSrcLine;

	int m_nNumFaces;					//三角形带的图元数目，面的数目

public:
	float m_fMaxDist;					//插值点的间距最大值
	float m_fMaxLength;					//飘带长度的最大值
	SRCLINE  m_BaseLine;				//绑定飘带的线段

	int m_nMaxLineBuffer;				//顶点缓存为该值的两倍
	D3DXVECTOR3 m_WindForce;			//风力 
	D3DXVECTOR3 m_Gravity;				//重力 
	BOOL m_bShowWind;					//是否使用风力
	BOOL m_bExtend;						//是否放大飘带末端的运动
	BOOL m_bShowSin;					//是否使用正弦波

	BOOL m_bShow;						//该变量无用

	float m_fExtendNum;					//飘带末端飘动加剧系数
	float m_fSwing;						//正弦波振幅
	float m_fCyc;						//振动周期调整系数;

	int m_nListMaxSize;				//允许保存在m_SrcLineList的Line的数目


public:
	KModelBelt(void);
	virtual ~KModelBelt(void);
	HRESULT Render();
	HRESULT Acquire();
	HRESULT LoadMesh(LPSTR pFileName);

private:
	HRESULT InitialBelt(SRCLINE Line); 
	HRESULT UpdateVertices(void);	
	HRESULT AddLine(SRCLINE Line);
	HRESULT	UpdateBladeLine(void);
//	HRESULT UpdateAtRest(SRCLINE Line);
	HRESULT UpdateExtend(void);

};

};