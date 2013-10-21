#pragma once

// File: VolumeFog.h
// Desc: 体积雾，暂时限定体积雾为长方体，基于轴向

#ifndef VOLUMEFOG_H
#define VOLUMEFOG_H

#include "plane.h"
#include<vector>
using std::vector;

const float COS_PI_4 = cosf(PI / 4);

const unsigned int FOG_VERTEX_FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;



struct KFogVertex
{
	float x, y, z;
	DWORD diffuse;
	float u, v;
};

class KFogPlane
{
public:
	enum
	{
		NUM_OF_INDICES = 6
	};

public:
	unsigned int m_VertexOffset;
	unsigned int m_NumOfVertices;

public:
	KFogPlane(): m_VertexOffset(0), m_NumOfVertices(0) {}
	KFogPlane(const KFogPlane& fp): m_VertexOffset(fp.m_VertexOffset), m_NumOfVertices(fp.m_NumOfVertices) {}
	KFogPlane& operator = (const KFogPlane& fp);
	~KFogPlane() {}

	unsigned int GetVertexOffset() const { return m_VertexOffset; }
	unsigned int GetNumOfVertices() const { return m_NumOfVertices; }
	void SetVertexOffset(unsigned int VertexOffset) { m_VertexOffset = VertexOffset; }
	void SetNumOfVertices(unsigned int NumOfVertices) { m_NumOfVertices = NumOfVertices; }

	KFogVertex* GetVertex(KFogVertex* pFVBuffer, unsigned index) const;

	HRESULT Render();
};

class KVolumeFog: public KModel
{
public:
	IDirect3DVertexBuffer9* m_pVB;
	IDirect3DVertexBuffer9* m_pClippedVB;
	unsigned int m_MaxNumOfVertices;
	unsigned int m_BaseNumOfVertices;
	unsigned int m_ClippedNumOfVertices;  // The number of vertices in m_ClippedFogPlanes

	float m_HalfXSpan, m_HalfZSpan;

	vector<KFogPlane*> m_FogPlanes;
	vector<KFogPlane*> m_ClippedFogPlanes;

	Plane m_TopPlane;  // 该平面垂直于Y轴，其法向量是Y轴的负方向
	Plane m_LeftPlane;  // 该平面垂直于X轴，其法向量是X轴的正方向
	Plane m_RightPlane;  // 该平面垂直于X轴，其法向量是X轴的负方向
	Plane m_ZUpPlane;  // 该平面垂直于Z轴，其法向量是Z轴的负方向
	Plane m_ZBottomPlane;  // 该平面垂直于Z轴，其法向量是Z轴的正方向

	D3DXVECTOR3 m_TopFogPlaneCenter;  // 第一层FogPlane的中心点，可用于移动FogPlanes
	float m_ViewOffset;  // 照相机位于TopPlane上面时，ViewOffset是最顶层的FogPlane到TopPlane的距离
	                             // 照相机位于TopPlane下面时，ViewOffset是离照相机最近的FogPlane到照相机的距离

	DWORD m_Color;  // 雾的颜色， ARGB，包含Alpha
	float m_DistBetweenFPs;  // 雾平面之间的距离

	bool m_bInFogVolume;  // 位于雾体积内

	string m_FilePath;
	string m_Filename;

public:
	KVolumeFog(): m_pVB(0), m_pClippedVB(0), m_MaxNumOfVertices(0), m_ClippedNumOfVertices(0) {}
	virtual ~KVolumeFog() { Destroy(); }

	virtual HRESULT LoadMesh(LPSTR pFileName);
	virtual HRESULT SaveMesh(FILE* pFile);
	virtual HRESULT FrameMove();
	virtual HRESULT Render();

	bool Create(const D3DXVECTOR3& TopFogPlaneCenter, float HalfSpanX, float HalfSpanZ, float TopPlaneY, unsigned int NumOfFogPlanes, DWORD dwColor, float ViewOffset, float DistBetweenFPs, const string& sPath, const string& sName);
	void Destroy();
	
	bool IsCameraInside() const;


public:
	HRESULT Acquire();//暴露属性

	// 编辑函数
	void SetTopFogPlaneCenter(const D3DXVECTOR3& v);
	void SetHalfSpanX(float HalfSpanX);
	void SetHalfSpanZ(float HalfSpanZ);
	void SetTopPlaneY(float TopPlaneY);
	void SetNumOfFogPlanes(unsigned int NumOfFPs);
	void SetDiffuse(unsigned int Diffuse);
	void SetColorR(unsigned char r);
	void SetColorG(unsigned char g);
	void SetColorB(unsigned char b);
	void SetColorA(unsigned char a);
	void SetViewOffset(float ViewOffset);
	void SetDistBetweenFPs(float DistBetweenFPs);

protected:
	D3DXVECTOR3 m_PrevTopFogPlaneCenter;
	D3DXVECTOR3 m_CurrTopFogPlaneCenter;
	float m_PrevHalfSpanX;
	float m_CurrHalfSpanX;
	float m_PrevHalfSpanZ;
	float m_CurrHalfSpanZ;
	float m_PrevTopPlaneY;
	float m_CurrTopPlaneY;
	unsigned int m_PrevNumOfFPs;
	unsigned int m_CurrNumOfFPs;
	unsigned int m_PrevColorR;
	unsigned int m_CurrColorR;
	unsigned int m_PrevColorG;
	unsigned int m_CurrColorG;
	unsigned int m_PrevColorB;
	unsigned int m_CurrColorB;
	unsigned int m_PrevColorA;
	unsigned int m_CurrColorA;
	float m_PrevViewOffset;
	float m_CurrViewOffset;
	float m_PrevDistBetweenFPs;
	float m_CurrDistBetweenFPs;

	void CalBBox(KFogVertex* pFVBuffer);
	void CalBoundingPlanes();
	void Update();
};

#endif