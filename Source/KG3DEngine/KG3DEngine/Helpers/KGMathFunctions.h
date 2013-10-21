////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGMathFunctions.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-22 17:21:37
//  Comment     : MathTool.cpp和很多东西有关系，所以不想搅和在一次增加耦合，
//这个单元要求尽量少包含别的东西,MathTool.h包含这个
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGMATHFUNCTIONS_H_
#define _INCLUDE_KGMATHFUNCTIONS_H_
#include "KGCommonWidgets.h"

////////////////////////////////////////////////////////////////////////////////
D3DCOLORVALUE Color2Value(D3DCOLOR Color);
VOID Color2Value(D3DCOLOR Color, D3DCOLORVALUE& Value);

///字节操作
#define D3DCOLOR_GETRED(SRC)   (((SRC)&0x00ff0000)>>16)
#define D3DCOLOR_GETGREEN(SRC) (((SRC)&0x0000ff00)>> 8)
#define D3DCOLOR_GETBLUE(SRC)  ((SRC)&0x000000ff)
#define D3DCOLOR_GETALPHA(SRC) ((SRC)>>24)

#define D3DCOLOR_GETRED_FLOAT(SRC)   ((((SRC)&0x00ff0000)>>16) / 255.0f)
#define D3DCOLOR_GETGREEN_FLOAT(SRC) ((((SRC)&0x0000ff00)>> 8) / 255.0f)
#define D3DCOLOR_GETBLUE_FLOAT(SRC)  (((SRC)&0x000000ff) / 255.0f)
#define D3DCOLOR_GETALPHA_FLOAT(SRC) (((SRC)>>24) / 255.0f)
#define D3DCOLOR_SETALPHA(SRC, ALPHA) (SRC = (((SRC)&0x00ffffff)|((ALPHA)<<24)))

///值域，用于替代max和min宏的不确定性
#define KG3DMT_MAX(a, b)  (((a) > (b)) ? (a) : (b)) 
#define KG3DMT_MIN(a, b)  (((a) < (b)) ? (a) : (b))

///限制到范围内
#define MT_LIMIT_TO_RANGE(snum, smax, smin)	\
	do	\
{	\
	_ASSERTE((smin)<=(smax));	\
	(snum) > (smax) ? (snum) = (smax) : ((snum) < (smin) ? (snum) = (smin) : NULL);	\
}while(false)
#define MT_LIMIT_D3DCOLOR_RANGE(snum)	MT_LIMIT_TO_RANGE((snum), 255, 0)

template<typename _Type>
inline BOOL MT_TInRange(_Type Object, _Type LowerBount, _Type HigherBound)///类型安全的版本
{
	if (Object >= LowerBount && Object <= HigherBound)
	{
		return TRUE;
	}
	return FALSE;
}

#define IS_FLOAT_NAN(fNum)	(_isnan(fNum))		//用数学函数的时候，如果数无法在浮点空间标识，就会出，如sqrt(-1.f);分正的 1.#IND000和负的1#IND000
#define IS_FLOAT_INFINITE(fNum)	(! _finite(fNum))//除0的时候，会得到正无限 1.#INF000或者负无限 -1.#INF000

#define MT_MAX_VALUE_FLOAT	(1000000.0f)
#define MT_MIN_VALUE_FLOAT	(-MT_MAX_VALUE_FLOAT)
#define IS_FLOAT_VALID(fNum)	(! IS_FLOAT_INFINITE(fNum) && ! IS_FLOAT_NAN(fNum))	//必须同时判断两个

#define ASSERT_FLOAT_IS_VALIED(fNum) \
	ASSERT(IS_FLOAT_VALID(fNum) && _T("Float Not Valid"))

///转换
inline DWORD FtoDW(FLOAT f){return *((DWORD*)&f);}
BOOL LPCTSTR2D3DCOLOR(LPCTSTR czSource, int nMaxStringLen, D3DCOLOR& ResultColor);//ARGB
BOOL D3DCOLOR2LPTSTR(D3DCOLOR Color, LPTSTR czResult, int nBufferSize);//ARGB
void LPCTSTR2IntInRange( LPCTSTR czSource, int* pnDes, int nLowerBound, int nUpperBound );
D3DCOLOR ColorValue(D3DCOLORVALUE co);
COLORREF ColorValue2ref(D3DCOLORVALUE co);
D3DCOLORVALUE Colorref2value(COLORREF Color,float a);
D3DCOLORVALUE Colorref2value_GDI(COLORREF Color,float a);
D3DCOLOR Value2Color(const D3DCOLORVALUE& D3DColorValue);
inline void Value2Color( const D3DCOLORVALUE &D3DColorValue, D3DCOLOR &D3DColor ){D3DColor = Value2Color(D3DColorValue);}
void Color2Ref( const D3DCOLOR &D3DColor, COLORREF &CRColor );
//void Color2Ints(const D3DCOLOR& D3DColor, int& nRed, int& nGreen, int& nBlue, int& nAlpha);

inline UCHAR D3DCOLOR2Gray( D3DCOLOR Color )
{
	INT nGray = ((D3DCOLOR_GETRED(Color))*38 + (D3DCOLOR_GETGREEN(Color))*75 + (D3DCOLOR_GETBLUE(Color))*15) >> 7;
	//INT nGray = (nR*38 + nG*75 + nB*15) >> 7;	///灰度移位公式
	MT_LIMIT_D3DCOLOR_RANGE(nGray);
	return *(UCHAR*)(&nGray);
}
inline D3DCOLOR D3DCOLOR2GrayColor( D3DCOLOR Color, BOOL bAlphaTo255 = TRUE)
{
	INT nGray = ((D3DCOLOR_GETRED(Color))*38 + (D3DCOLOR_GETGREEN(Color))*75 + (D3DCOLOR_GETBLUE(Color))*15) >> 7;
	MT_LIMIT_D3DCOLOR_RANGE(nGray);
	return bAlphaTo255 ? D3DCOLOR_XRGB(nGray, nGray, nGray) : D3DCOLOR_ARGB(D3DCOLOR_GETALPHA(Color), nGray, nGray, nGray);
}
#define D3DCOLOR2INTS(color, nA, nR, nG, nB)	\
	do	\
{	\
	(nA) = D3DCOLOR_GETALPHA((color));	\
	(nR) = D3DCOLOR_GETRED((color));	\
	(nG) = D3DCOLOR_GETGREEN((color));	\
	(nB) = D3DCOLOR_GETBLUE((color));	\
} while(false)

template<size_t _Size>
inline VOID D3DCOLOR2IntArray(D3DCOLOR color, INT (&Array)[_Size])//ARGB
{
	_KG3D_DEBUG_STATIC_CHECK_T(_Size >= 4, SizeNotEnough);
	D3DCOLOR2INTS(color, Array[0], Array[1], Array[2], Array[3]);	
}

//矩阵
VOID D3DXMatrixViewPort(const D3DVIEWPORT9& ViewPort, D3DXMATRIX& matRet);//由Viewport构建Viewport矩阵
D3DXVECTOR3& D3DXMatrixGetTranslationPart(D3DXMATRIX& mat);	//得到矩阵的位置的部分，那样就不用老41,42的赋值了，比较清晰
const D3DXMATRIX& D3DXMatrixGetIdentity();
D3DXVECTOR3*	D3DXMatrixGetPosOfView(const D3DXMATRIX* pMatView, D3DXVECTOR3* pOut);//返回值是NULL或者pOut
D3DXVECTOR3*	D3DXMatrixGetDirOfView(const D3DXMATRIX* pMatView, D3DXVECTOR3* pOut);//返回值是NULL或者pOut
D3DXVECTOR3		D3DXMatrixGetRightOfView(const D3DXMATRIX& matView);
D3DXVECTOR3		D3DXMatrixGetUpOfView(const D3DXMATRIX& matView);
BOOL	D3DXMatrixProjIsOrtho(const D3DXMATRIX& mat);
BOOL	D3DXMatrixGetZNearFromProj(const D3DXMATRIX& mat, FLOAT& Ret);	//全部假设是左手坐标系为准
BOOL	D3DXMatrixGetZFarFromProj(const D3DXMATRIX& mat, FLOAT& Ret);
BOOL	D3DXMatrixProjResetZFar(D3DXMATRIX& mat, FLOAT newZFar);	//用来做Clipping正好。ZFar等于ZNear的时候会失败
BOOL	D3DXMatrixProjResetZNearFar(D3DXMATRIX& mat, FLOAT newZNear, FLOAT newZFar);	//用来做Clipping，同时可以做放缩。(用法1.把ZNear和ZFar放到物体的远端和近端，看上去就像用了正交矩阵)
const D3DXVECTOR3& D3DXVec3GetNormalOfPlane(size_t i);//0,1,2分别是xyz
const D3DXMATRIX& D3DXMatrixGetRotationMatrixOfAxis(size_t i);//0,1,2分别是xyz轴对应的，把xz平面上的物体旋转到对应轴的矩阵

//三维碰撞
D3DXVECTOR2 PointDistanceToLine(const D3DXVECTOR2& vP, const D3DXVECTOR2& vSrc, const D3DXVECTOR2& vNormalizedDir);//求2D，点到线的最近点
D3DXVECTOR3 PointDistanceToLine(const D3DXVECTOR3& vP, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir);//求3D，点到线最近点

FLOAT	TriangleArea(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3);
size_t	GetVertexStrideFromFVF(DWORD FVF);//从顶点FVF得到顶点大小，错误返回0

//投射系列
__forceinline D3DXVECTOR3 ProjectVectorToDirection(const D3DXVECTOR3& vVec, const D3DXVECTOR3& vDir)//向量投射，张量乘运算，vDir要是单位向量
{
	return D3DXVec3Dot(&vVec, &vDir) * vDir;
}
D3DXVECTOR3 ProjectVectorToPlane(const D3DXVECTOR3& vVec, const D3DXVECTOR3& vPlaneNormal);	//向量到面的投射，vDir要是单位向量
D3DXVECTOR3 ProjectPointToPlane(const D3DXVECTOR3& vP, const D3DXPLANE& plane);		//点到面的投射
D3DXVECTOR3 ProjectPointToPlane(const D3DXVECTOR3& vP, const D3DXVECTOR3& vOnePointOnPlane, const D3DXVECTOR3& vNormalOfPlane);
D3DXVECTOR3 ProjectPointToRay(const D3DXVECTOR3& vP, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vDir);	//点到线的投射，vDir要是单位向量

HRESULT	CalculatePickRay(const D3DXVECTOR2& vScreenPos, const D3DXMATRIX& matViewProjection
						 , const D3DVIEWPORT9& viewport, D3DXVECTOR3& vSrcRet, D3DXVECTOR3& vNormalizedDirRet);
HRESULT CalculatePickRay(const D3DXVECTOR2& vScreenPos, const D3DXMATRIX& matViewProjViewport
							, D3DXVECTOR3& vSrcRet, D3DXVECTOR3& vNormalizedDirRet);	//矩阵是View*Proj*ViewportMatrix.其中ViewportMatrix可以用D3DXMatrixViewPort得到

///线性插值
void TwoColorBlend(const D3DCOLOR& Color1, const D3DCOLOR& Color2, D3DCOLOR& ColorResult, float fPercent, bool bBlendAlpha = false);
#define	MT_INT_BLEND(n1, n2, nr, fPercent)	\
	do	\
	{	\
		if ((n1) == (n2))	\
			(nr) = (n1);	\
		else				\
			((nr) = (int)((n1)*(1-(fPercent)) + (n2)*(fPercent)));	\
	} while(false)

#define	MT_FLOAT_BLEND(f1, f2, fr, fPercent)	\
	do	\
	{	\
		if ((f1) == (f2))	\
			(fr) = (f1);	\
		else				\
			((fr) = ((f1)*(1-(fPercent)) + (f2)*(fPercent)));	\
	} while(false)
#define  MTAbsolute(num)	((num) = (num) >= 0 ? (num) : 0 - (num))

#define MT_FLOAT_TRIM_TAIL(F)	((INT)F)//不同系统是不同的

///FLOAT和Vector的偏移比较，FLOAT直接!=效果不好，这样很接近的也会认为是不等于
#define  MT_FLOAT_UNEQUAL_BIAS(F1, F2, bias)	((F2)-(F1) > (bias) || (F1)-(F2) > (bias))
#define  MT_FLOAT_EQUAL_BIAS(F1, F2, bias) (!MT_FLOAT_UNEQUAL_BIAS((F1), (F2), (bias)))
#define  MT_FLOAT_UNEQUAL(F1, F2)	(MT_FLOAT_UNEQUAL_BIAS((F1), (F2), FLT_EPSILON))
#define  MT_FLOAT_EQUAL(F1, F2)		(MT_FLOAT_EQUAL_BIAS((F1), (F2), FLT_EPSILON))

#define MT_VECTOR_UNEQUAL(V1, V2, bias)	( MT_FLOAT_UNEQUAL_BIAS((V1).x, (V2).x, (bias))	\
	|| MT_FLOAT_UNEQUAL_BIAS((V1).y, (V2).y, (bias))	\
	|| MT_FLOAT_UNEQUAL_BIAS((V1).z, (V2).z, (bias)))

#define MT_VECTOR_UNEQUAL_2D(V1, V2, bias)	( MT_FLOAT_UNEQUAL_BIAS((V1).x, (V2).x, (bias))	\
											|| MT_FLOAT_UNEQUAL_BIAS((V1).y, (V2).y, (bias)))	
#define MT_VECTOR_EQUAL(V1, V2, bias)	(!(MT_VECTOR_UNEQUAL((V1), (V2), (bias))))
#define MT_VECTOR_EQUAL_2D(V1, V2, bias)	(!(MT_VECTOR_UNEQUAL_2D((V1), (V2), (bias))))

#define MT_FLOAT_IS_A_LESS_THAN_B(A, B)	((B) - (A) > 0)
#define MT_FLOAT_IS_A_LESS_OR_EQUAL_B(A, B)	((B) - (A) > -FLT_EPSILON)

#define MT_FLOAT_IS_IN_RANGE(SRC, MIN_NUM, MAX_NUM)	MT_FLOAT_IS_A_LESS_OR_EQUAL_B((MIN_NUM), (SRC)) && MT_FLOAT_IS_A_LESS_THAN_B((SRC), (MAX_NUM))

///四舍五入
#define MT_ROUND_HELPER(num)	( (num) > floor((num))+0.5-FLT_EPSILON ? (INT)(num)+1 : (INT)(num) )
inline INT MT_Round(double num)	//因为float可以自动转型为double，所以无需提供float的版本
{
	if ((num) < 0)
	{
		num = -num;
		INT nRet = MT_ROUND_HELPER(num);
		return -nRet;
	}
	return MT_ROUND_HELPER(num);
}
__forceinline INT FourCullFiveAdd(double num)
{
	return MT_Round(num);
}

//其它数值辅助函数

///得到离散值，模板参数要手工填
template<typename _ReturnType, _ReturnType _Min, _ReturnType _Max, UINT uSteps>
static _ReturnType MT_GetDiscreted(_ReturnType Src)	
{
	_KG3D_DEBUG_STATIC_CHECK_T(uSteps > 0 && _Max > _Min, ParamMistake);
	static const _ReturnType GapPerStep = (_Max - _Min)/uSteps;
	if (Src <= _Min)
		return _Min;
	if (Src >= _Max)
		return _Max;
	return (_ReturnType)(INT)((Src - _Min)/GapPerStep)*GapPerStep + _Min;
}

FLOAT GetRectDiagonalLength(RECT rect);

//线结构
struct KG3DLine 
{
	D3DXVECTOR3 m_vSrc;
	D3DXVECTOR3 m_vDes;

	KG3DLine()	{ZeroMemory(this, sizeof(KG3DLine)); }
	KG3DLine(const KG3DLine& Other)	{memcpy_s(this, sizeof(KG3DLine), &Other, sizeof(KG3DLine));}
	KG3DLine(const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vDes)	:m_vSrc(vSrc),m_vDes(vDes){}

	D3DXVECTOR3& operator[](size_t uWhichPoint){return (reinterpret_cast<D3DXVECTOR3 *>(this))[uWhichPoint];}		//支持用[]语法直接访问其中的点，如果lineA[0] = XX
	const D3DXVECTOR3&  operator[](size_t uWhichPoint) const{return (reinterpret_cast<const D3DXVECTOR3 *>(this))[uWhichPoint];}
};

struct KG3DRay 
{
	D3DXVECTOR3 m_vSrc;
	D3DXVECTOR3 m_vNormalizedDir;

	KG3DRay()	{ZeroMemory(this, sizeof(KG3DRay)); m_vNormalizedDir.y = -1;}
	KG3DRay(const KG3DRay& Other)	{memcpy_s(this, sizeof(KG3DRay), &Other, sizeof(KG3DRay));}
	KG3DRay(const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir)	:m_vSrc(vSrc),m_vNormalizedDir(vNormalizedDir){}
};

enum
{
	EM_RAY_INTERSECTION_MAX_RANGE = 25600,//最大的碰撞的检测范围
};

struct VisitorForD3DXVECTOR3
{
	virtual HRESULT Accept(const D3DXVECTOR3& pos) = 0;
	virtual ~VisitorForD3DXVECTOR3() = 0{}
};

#endif //_INCLUDE_KGMATHFUNCTIONS_H_
