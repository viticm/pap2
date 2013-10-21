#pragma once
namespace Physics
{
class KCollisionInfo
{
public:
	KCollisionInfo(void);
	KCollisionInfo(	LPRIGIDBODY pRigid1, LPRIGIDBODY pRigid2, D3DXPLANE* pPlane, const D3DXVECTOR3& DirTo1, const D3DXVECTOR3& ColPosition, float PenetrationDepth );
	~KCollisionInfo(void);

	LPRIGIDBODY m_pRigid1;		//碰撞体1
	LPRIGIDBODY m_pRigid2;		//碰撞体2
	D3DXPLANE* m_pPlane;		//碰撞平面
	
	D3DXVECTOR3 m_DirTo1;		//碰撞方向, 指向碰撞体1
	D3DXVECTOR3 m_ColPosition;	//碰撞点
	float m_fPenetrationDepth;	//嵌入深度

	D3DXVECTOR3 m_R1;			//相对于碰撞体1的碰撞点
	D3DXVECTOR3 m_R2;			//相对于碰撞体2的碰撞点
	
	float m_VrExtra;			

	float m_fStaticFriction;	//静摩擦系数
	float m_fDynamicFriction;	//动摩擦系数

	float m_fDenominator;		//碰撞冲量的分母
};

};