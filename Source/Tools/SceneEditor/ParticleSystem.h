#pragma once

#include <list>
#include "types.h"
#include "inireader.h"
#include "kbarrier.h"
#include "KLauncher.h"
#include "kforcefield.h"
#define MAX_LAUNCHER 20


namespace PropertyBase
{
class KDlg_PS_Main;
}

namespace ModelSFX
{

	//得到现在的时间， 以秒为单位
	inline float GetCurrentTime()
	{
		return (float)timeGetTime() / 1000;
	}

	//得到一个fMin<x<fMax随机浮点数
	inline float GetRandom(float fMin, float fMax)
	{
		float fRandNum = (float)rand() / RAND_MAX;
		return (fMin + (fMax - fMin ) * fRandNum);
	}

	
	//得到随机的符号（1 |　－１）
	inline int GetRandomSgn()
	{
		int iTemp = rand();
		if ( (iTemp % 2) == 1 )
			return -1;
		else return 1;
	}

class KParticleSystem
{
//////////////////////////////////////////////////////////////////////////
//For Editor
	friend class PropertyBase::KDlg_PS_Main;
//For Editor end

public:
	HRESULT LoadFFFromFile(LPCSTR strFileName);
	HRESULT SaveForFieldToFile(LPCSTR strFileName);

	float Dif_Sin(float, float,float);
	KParticleSystem();
	~KParticleSystem();
	
	bool SaveLaunParToFile(LPCSTR strFileName, int iIndex);
	bool LoadLaunFromFile(LPCSTR strFileName);
	virtual void Init();//初始化
	//控制发射器
	void SetLauncher(int iLaunOffset, bool bStatus) 
	{ 
		if (bStatus)
		{
			float fTime = GetCurrentTime(); 
			m_vecLauncher[iLaunOffset]->m_fStartTime = fTime; 
			m_vecLauncher[iLaunOffset]->m_fLastUpdate = fTime;
			m_vecLauncher[iLaunOffset]->SetStatus(TRUE);
		}
		else 
		{
			m_vecLauncher[iLaunOffset]->SetStatus(FALSE);
		}
	}
	void SetBarrier(int iBarOffset, bool bStatus){ m_vecBarrier[iBarOffset]->SetBarrier(bStatus);}
	void SetForce(int iForOffset, bool bStatus) { m_vecForce[iForOffset]->SetForceField(bStatus);}
	
	//增加删除发射器，阻挡板和力场
	void AddLauncher( ModelSFX::KLauncher* launcher) { m_vecLauncher.push_back(launcher); }
	void AddForce( ModelSFX::KForceField *ff){ m_vecForce.push_back (ff);}
	void AddBarrier( ModelSFX::KBarrier *b){ m_vecBarrier.push_back (b);}
	void RemoveLauncher(int iLaunOffset) { m_vecLauncher.erase(&m_vecLauncher[iLaunOffset]);}
	void RemoveForce( int iForOffset) { m_vecForce.erase( &m_vecForce[iForOffset]);}
	void RemoveBarrier( int iBarOffset) { m_vecBarrier.erase( &m_vecBarrier[iBarOffset]);}
	
	//更新系统
	void Update();//更新整个系统
	vector<ModelSFX::KLauncher*> m_vecLauncher;//发射器列表
private:
	
	
///////////////////////////////////////////////////////////////////////////////////////////
	//产生粒子
	
	//FILE* fpdebug;
	int m_TriggerIndex;
	float			m_fStartTime;//Particles System运行时间
	bool			m_bForce ;//particle system是否开启力场
	bool			m_bBarrier;//particles system是否开启阻挡
	float m_fLastUpdate;
protected:
	CString m_strLuanName[20];
	int m_iLaunCount;
	//KIniReader m_ini;
	
	vector< ModelSFX::KForceField*> m_vecForce;//力场列表
	vector< ModelSFX::KBarrier*> m_vecBarrier;//阻挡板列表
public:
	HRESULT FrameMove(void);
	vector<KBarrier*>* GetBarrier(void);
	vector<KForceField*>* GetForceField(void);
};



//////////////////////////////////////////////////////////////////////////
//函数：GetPointOnLine
//作用：得到直线段上的一个随机点
//参数：
//		vOutPoint[out]:D3DXVECTOR3,得到的点
//		vStart[in]:D3DXVECTOR3, 线段的起点
//		vEnd[in]:D3DXVECTOR3, 线段的终点
//返回值: vOutPoint: D3DXVECTOR3
//////////////////////////////////////////////////////////////////////////
inline D3DXVECTOR3 GetPointOnLine(D3DXVECTOR3& vOutPoint, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd)
{
	float fLength = GetRandom(0.0f, 1.0f);
	vOutPoint = vEnd - vStart;
	vOutPoint *= fLength;
	vOutPoint += vStart;
	return vOutPoint;
}

//////////////////////////////////////////////////////////////////////////
//函数：GetPointOnLine
//作用：得到直线段上的一个随机点
//参数：
//		vOutPoint[out]:D3DXVECTOR3,得到的点
//		vStart[in]:D3DXVECTOR3, 线段的起点
//		vEnd[in]:D3DXVECTOR3, 线段的终点
//返回值: vOutPoint: D3DXVECTOR3
//////////////////////////////////////////////////////////////////////////
inline D3DXVECTOR3 GetPointOnLine(D3DXVECTOR3& vOutPoint, D3DXVECTOR3* pLineList)
{
	float fLength = GetRandom(0.0f, 1.0f);
	vOutPoint = *(pLineList + 1)  - *pLineList;
	vOutPoint *= fLength;
	vOutPoint += *pLineList;
	return vOutPoint;
}

//////////////////////////////////////////////////////////////////////////
//函数：GetPointOnLine
//作用：得到三角形上的一个随机点
//参数：
//		vOutPoint[out]:D3DXVECTOR3,得到的点
//		pVertextList[in]:D3DXVECTOR3*, 顶点列表
//返回值: vOutPoint: D3DXVECTOR3
//////////////////////////////////////////////////////////////////////////
inline D3DXVECTOR3 GetPointOnTriangle(D3DXVECTOR3& vOutPoint, D3DXVECTOR3* pVertexList)
{
	float fFactor1 = GetRandom(0.0f, 1.0f);
	float fFactor2 = GetRandom(0.0f, 1.0f);
	D3DXVECTOR3 vtmp = (pVertexList[0] - pVertexList[1]) * fFactor1;
    vOutPoint =  vtmp +  (pVertexList[2] - pVertexList[1] - vtmp) * fFactor2 + 
				pVertexList[1];
	return vOutPoint;
}

////////////////////////////////////////////////////////////////////////////////////
//GetPointOnRect()
//作用：得到一个空间内一个四边形边上的随机点
//参数：
//		vPoint:[out]得到的点
//		vA,vB,vC,vD要顺时针
//		vA:[in]四边形的顶点
//		vB:[in]同上
//		vC:[in]同上
//		vD:[in]同上
///////////////////////////////////////////////////////////////////////////////////
inline D3DXVECTOR3 GetPointOnRect(D3DXVECTOR3 & vPoint, const D3DXVECTOR3 &vA, const D3DXVECTOR3 &vB, const D3DXVECTOR3 &vC, const D3DXVECTOR3 &vD, bool bFace)
{
	if ( bFace )
	{
		D3DXVECTOR3 vAB = vB - vA;
		D3DXVECTOR3 vAD = vD - vA;
		float fScale;
		fScale = GetRandom( 0.0f, 1.0f);
		vAB = vAB * fScale;
		fScale = GetRandom( 0.0f, 1.0f);
		vAD = vAD * fScale;
		vPoint = vAB + vAD + vA;
		return vPoint;
	}
	else
	{
		DWORD dwEdge = rand() % 4;
		float fScale = GetRandom(0.0f, 1.0f);
		switch (dwEdge)
		{
		case 0:
			{
				vPoint = (vB - vA) * fScale + vA;
				break;
			}
		case 1:
			{
				vPoint = (vC - vB) * fScale + vB;
				break;
			}
		case 2:
			{
				vPoint = (vD - vC) * fScale + vC;
				break;
			}
		case 3:
			{
				vPoint = (vA - vD) * fScale + vD;
				break;
			}
		}
		return vPoint;
	}
}



////////////////////////////////////////////////////////////////////////////////////
//GetPointOnCircle()
//作用：得到一个空间内一个圆面上的随机点
//参数：
//		vPoint:[out]得到的点
//		vCenter:[in]圆心
//		vNormal:[in]圆的法线
//		fRadius:[in]圆的半径
//////////////////////////////////////////////////////////////////////////
inline D3DXVECTOR3 GetPointOnCircle(D3DXVECTOR3 &vPoint, const D3DXVECTOR3 &vCenter, D3DXVECTOR3 &vNormal, const float fRadius, bool bFace)
{
	D3DXVECTOR3 vTemp;
	vTemp.x = GetRandom( -fRadius , fRadius);
	float fTemp = fRadius * fRadius - vTemp.x * vTemp.x;
	vTemp.y = GetRandom( -sqrt(fTemp) , sqrt(fTemp));
	vTemp.z = sqrt( fTemp  - vTemp.y * vTemp.y) * GetRandomSgn();
	D3DXVec3Normalize(&vNormal, &vNormal);
	D3DXVec3Cross(&vPoint, &vTemp, &vNormal);
	if ( !bFace )
	{
		D3DXVec3Normalize(&vPoint, &vPoint);
		vPoint *= fRadius;
	}
	vPoint += vCenter;
	return vPoint;
}

///////////////////////////////////////////////////////////////////
//GetVec3Len();
//作用：得到向量长度
//参数: vVector[in]:D3DXVECTOR3, 需要求长度的向量
//返回值: float, 向量长度
//////////////////////////////////////////////////////////////////////////
inline float GetVec3Len(const D3DXVECTOR3 &vVector)
{	
	return sqrt(vVector.x * vVector.x + vVector.y * vVector.y + vVector.z * vVector.z);
}

};
