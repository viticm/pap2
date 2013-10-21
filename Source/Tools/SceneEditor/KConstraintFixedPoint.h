#pragma once
#include "KConstraint.h"

namespace Physics
{

class KConstraintFixedPoint :
	public KConstraint
{
public:
	KConstraintFixedPoint(void);
	~KConstraintFixedPoint(void);
	int Initialise(LPRIGIDBODY pRigid1, D3DXVECTOR3 Rigid1Pos, LPRIGIDBODY pRigid2, D3DXVECTOR3 Rigid2Pos, float MaxDist );
    int PreApply(float TinyTime);
	BOOL Apply(float TinyTime);

private:
	LPRIGIDBODY m_pRigid1;		//约束体1
	LPRIGIDBODY m_pRigid2;		//约束体2
	D3DXVECTOR3 m_Rigid1Pos;	//约束体1的约束点(相对于世界坐标系)
	D3DXVECTOR3 m_Rigid2Pos;	//约束体2的约束点(相对于世界坐标系)
	float m_fMaxDist;			//两约束点分离的最大距离

//	D3DXVECTOR3 m_WorldPos;
	D3DXVECTOR3 m_R1;			//约束体1的约束点(相对于局部坐标系)
	D3DXVECTOR3 m_R2;			//约束体2的约束点(相对于局部坐标系)
	D3DXVECTOR3 m_CurRelPos1;   //约束体1的约束点与约束体2的约束点的差值(相对于世界坐标系)

};

};