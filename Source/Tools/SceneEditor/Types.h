#include <list>
#include <vector>
#include "TimePoint.h"

#ifndef _TYPES_H
#define _TYPES_H
using namespace std;
namespace ModelSFX
{

enum FieldType
{
	F_CENTR_SPHERE,//不影响速度， 只是影响其方向
	F_CENTR_CYLINDER,//Drag Field
	F_DRAG,
	F_GRAVITY,//影响一个方向的速度
	F_VOTEX,
	F_VOTEX1,
	F_RADIAL
};

enum BarrierType
{
	B_TREFLECT,
	B_TTRIGGER,
	B_TDESTRUCT

};
enum FieldShape
{
	F_SPHERE,
	F_CYLINDER,
	F_BOX
};

enum BarrierShape
{
	B_SPHERE,
	B_CYLINDER,
	B_PLANE,
	B_TRIANGLELIST,
	B_TRIANGLESTRIP
};

enum LifeSpanMode
{
	LSM_LIVE_FOR_EVER,
	LSM_CONSTANT,
	LSM_RANDOM_RANGE
};

enum ParticleRenderType
{
	PR_NUMERIC,//?
	PR_POINTS,//OK
	PR_SPRITES,//OK
	PR_STREAK//?
};

enum ParticleType
{
	PT_VANISH,
	PT_TIGGER
};

enum ParticleShape
{
	PS_POINT,
	PS_BILLBOARD
};

struct CYLINDER
{
	D3DXVECTOR3 BtCenter;
	D3DXVECTOR3 UpCenter;
	float Radius;
};

struct SPHERE
{
	D3DXVECTOR3 Center;
	float Radius;
};
struct Particle
{
	D3DXVECTOR3 m_vec3VotexInc;//spcially for rotation force velocity increasement
	float m_fVotexInc;
	bool m_bInsideVotex;
	int m_iForceMask[100]; //max force field count 100
	D3DXVECTOR3 m_vDiffuse;
	D3DXVECTOR3	m_vVelocity;//粒子的速度
	D3DXVECTOR3 m_vPosition;//粒子的位置
	ParticleType m_eType;
	float m_fLifeSpan;//粒子的生存时间
	float m_fLastUpdate;//上次更新时间
	float m_fStartTime;//粒子生成的时间
	float m_fScaleVel;//粒子放缩速度
	float m_fRadius[2];//半径（Height 0, Width 1）
	D3DCOLOR m_cColor;//粒子的颜色
	float m_fRotSpd;//自旋速度
	float m_fRot;//自旋的角度
	float m_fConserve;
	Particle()
	{
		memset(m_iForceMask, 0, sizeof(m_iForceMask));
		m_fVotexInc = 0.0f;
		m_bInsideVotex = false;
	}
};

//struct ForceField
//{
//	//D3DXVECTOR3	m_vVelocity;//力场的速度
//	//D3DXVECTOR3 m_vPosition;//力场中心的位置
//	//BOUNDING_BOX m_bbBox;//力场的空间作用范围;为NULL则是全空间
//	D3DXVECTOR3 m_vAcc;//力场的加速度,如果是向心力， 那么只取.x作为旋转的角速度
//	FieldType	m_eType;//力场的形状		
//	FieldShape m_eShape;
//	//float m_fScaleVel;//力场放缩速度
//	bool	m_bEffective;
//	float	m_fForcePar[10];
//};




//struct Barrier
//{
//	BarrierType m_btType;
//	BarrierShape m_btShape;//阻挡板的形状
//	float m_fBarPar[20];
//	bool m_bEffective;
//};
//	
//LAUNCHER_SHAPE是发射器的形状
enum LauncherShape
{	
	L_POINT,
	L_LINE_LIST,
	L_LINE_STRIP,
	L_CIRCLE_FACE,
	L_CIRCLE,
	L_RECTANGLE,
	L_RECTANGLE_FACE,
	L_SPHERE_SURFACE,
	L_TRIANGLE_LIST,
	L_TRIANGLE_STRIP,
	L_USER_DEFINE//用户定义发射器形状
};

enum LauncherType
{
	LT_DIRECTIONAL,
	LT_OMNI,
	LT_CURVE,
	LT_SURFACE
};

};
#endif