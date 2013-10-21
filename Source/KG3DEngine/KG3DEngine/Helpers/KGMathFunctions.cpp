////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGMathFunctions.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-22 17:23:30
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KGMathFunctions.h"
#include "KGTester.h"
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL D3DCOLOR2LPTSTR( D3DCOLOR color, LPTSTR czResult, int nBufferSize )
{
	if (!czResult || nBufferSize < 3*4+4+1)
	{
		return FALSE;
	}

	INT ARGB[4];
	D3DCOLOR2IntArray(color, ARGB);
	int nRet = _stprintf_s(czResult, nBufferSize, _T("%d,%d,%d,%d"), ARGB[0], ARGB[1], ARGB[2], ARGB[3]);
	return nRet > 4? TRUE : FALSE;///sprintf返回的是拷贝的字符长度
}

BOOL LPCTSTR2D3DCOLOR( LPCTSTR czSource, int nMaxStringLen, D3DCOLOR& ResultColor )
{
	KG_PROCESS_ERROR(czSource);
	int ColorElements[4] = {0, 0, 0, 0};
	TCHAR LocalSource[128];
	int nRet = _tcscpy_s(LocalSource, _countof(LocalSource), czSource);
	KG_PROCESS_ERROR(nRet == 0);
	TCHAR tczSeps[] = _T(" ,\t\n");
	TCHAR* pToken1 = NULL;
	TCHAR* pNext_Token = NULL;
	pToken1 = _tcstok_s(LocalSource, tczSeps, &pNext_Token);
	for(int i = 0; i < 4; i++ )
	{
		KG_PROCESS_ERROR(NULL != pToken1);///不能全部解释出来也是错误
		int nTemp = _ttoi(pToken1);
		KG_PROCESS_ERROR(nTemp >= 0 && nTemp <= 255);
		ColorElements[i] = nTemp;
		pToken1 = _tcstok_s(NULL, tczSeps, &pNext_Token);
	}
	ResultColor = D3DCOLOR_ARGB(ColorElements[0], ColorElements[1], ColorElements[2], ColorElements[3]);
	return TRUE;
Exit0:
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

D3DCOLORVALUE Color2Value(D3DCOLOR Color)
{
	D3DCOLORVALUE nColor;
	nColor.a = ((float)(Color>>24))/255.0f;
	nColor.r = ((float)((Color&0x00ff0000)>>16))/255.0f;
	nColor.g = ((float)((Color&0x0000ff00)>>8))/255.0f;
	nColor.b = ((float)((Color&0x000000ff)))/255.0f;
	return nColor;
}

VOID Color2Value( D3DCOLOR Color, D3DCOLORVALUE& Value )
{
	Value.a = ((float)(Color>>24))/255.0f;
	Value.r = ((float)((Color&0x00ff0000)>>16))/255.0f;
	Value.g = ((float)((Color&0x0000ff00)>>8))/255.0f;
	Value.b = ((float)((Color&0x000000ff)))/255.0f;
}
D3DCOLOR ColorValue(D3DCOLORVALUE co)
{
	return D3DCOLOR_ARGB((int)(co.a*255),(int)(co.r*255),
		(int)(co.g*255),(int)(co.b*255));
}
COLORREF ColorValue2ref(D3DCOLORVALUE co)
{
	return RGB(co.r*255,co.g*255,co.b*255);
}
D3DCOLORVALUE Colorref2value(COLORREF Color,float a)
{
	D3DCOLORVALUE nColor;
	nColor.r = 1.0f;
	nColor.g = 1.0f;
	nColor.b = 1.0f;
	nColor.a = a;
	nColor.b = ((float)(Color>>16))/255.0f;
	nColor.g = ((float)((Color&0x00ff00)>>8))/255.0f;
	nColor.r = ((float)((Color&0x0000ff)))/255.0f;
	return nColor;
}
D3DCOLORVALUE Colorref2value_GDI(COLORREF Color,float a)
{
	D3DCOLORVALUE nColor;
	nColor.r = 1.0f;
	nColor.g = 1.0f;
	nColor.b = 1.0f;
	nColor.a = a;
	nColor.r = ((float)(Color>>16))/255.0f;
	nColor.g = ((float)((Color&0x00ff00)>>8))/255.0f;
	nColor.b = ((float)((Color&0x0000ff)))/255.0f;
	return nColor;
}
D3DCOLOR Value2Color( const D3DCOLORVALUE& D3DColorValue )
{
	return D3DCOLOR_COLORVALUE( D3DColorValue.r, D3DColorValue.g, D3DColorValue.b, D3DColorValue.a );
}
void Color2Ref( const D3DCOLOR &D3DColor, COLORREF &CRColor )//CTH Added
{
	CRColor = 0x00000000 //COLORREF的高位衡为0
		| ((D3DColor&0x00ff0000)>>16) //R
		| (D3DColor&0x0000ff00) 
		| ((D3DColor&0x000000ff)<<16);//B
}
/*
void Color2Ints( const D3DCOLOR& D3DColor, int& nRed, int& nGreen, int& nBlue, int& nAlpha )
{
nRed = D3DCOLOR_GETRED(D3DColor);
nGreen = D3DCOLOR_GETGREEN(D3DColor);
nBlue = D3DCOLOR_GETBLUE(D3DColor);
nAlpha = D3DCOLOR_GETALPHA(D3DColor);
}*/

void LPCTSTR2IntInRange( LPCTSTR czSource, int* pnDes, int nLowerBound, int nUpperBound )//CTH Added
{
	_ASSERTE( nLowerBound <= nUpperBound );
	int nTemp = _tstoi( czSource );
	if ( nTemp <= nLowerBound )
	{nTemp = nLowerBound;}
	else if( nTemp >= nUpperBound )
	{nTemp = nUpperBound;}
	if ( NULL !=  pnDes )
	{
		*pnDes = nTemp;
	}
}

void TwoColorBlend( const D3DCOLOR& Color1, const D3DCOLOR& Color2, D3DCOLOR& ColorResult, float fPercent, bool bBlendAlpha /* false*/)
{
	int RAlpha = D3DCOLOR_GETALPHA(Color1);
	if (fPercent <= 0.0)
	{
		ColorResult = Color1;
		goto Exit1;
	}
	else if (fPercent >= 1.0)
	{
		ColorResult = Color2;
		goto Exit1;
	}
	if (Color1 == Color2)
	{
		ColorResult = Color2;
		goto Exit1;
	}
	int C1Red, C1Green, C1Blue, C1Alpha, C2Red, C2Green, C2Blue, C2Alpha;
	C1Red = D3DCOLOR_GETRED(Color1);
	C1Green = D3DCOLOR_GETGREEN(Color1);
	C1Blue = D3DCOLOR_GETBLUE(Color1);
	C1Alpha = D3DCOLOR_GETALPHA(Color1);
	C2Red = D3DCOLOR_GETRED(Color2);
	C2Green = D3DCOLOR_GETGREEN(Color2);
	C2Blue = D3DCOLOR_GETBLUE(Color2);
	C2Alpha = D3DCOLOR_GETALPHA(Color2);
	int RRed, RGreen, RBlue;
	RRed = (int)(C1Red*(1.0-fPercent) + C2Red*fPercent);
	RGreen = (int)(C1Green*(1.0-fPercent) + C2Green*fPercent);
	RBlue = (int)(C1Blue*(1.0-fPercent) + C2Blue*fPercent);
	MT_LIMIT_D3DCOLOR_RANGE(RRed);
	MT_LIMIT_D3DCOLOR_RANGE(RGreen);
	MT_LIMIT_D3DCOLOR_RANGE(RBlue);
	if (bBlendAlpha == true)
	{
		RAlpha = (int)(C1Alpha*(1.0-fPercent) + C2Alpha*fPercent);
		MT_LIMIT_D3DCOLOR_RANGE(RAlpha);
	}
	ColorResult = D3DCOLOR_ARGB(RAlpha, RRed, RGreen, RBlue);
	return;
Exit1:
	if (!bBlendAlpha)
	{
		ColorResult &= 0x00ffffff;///清掉最高位
		DWORD dwRAlpha = static_cast<DWORD>(RAlpha);
		dwRAlpha = dwRAlpha << 24;
		ColorResult |= dwRAlpha;
	}
	return;
}

VOID D3DXMatrixViewPort( const D3DVIEWPORT9& ViewPort, D3DXMATRIX& matRet )
{
	/*
		Width/2	0		0		0
				0		-Height/2	0		0
				0		0		MaxZ-MinZ	0
				X+Width/2	Y+Height/2	MinZ		1*/
		
	ZeroMemory(&matRet, sizeof(D3DXMATRIX));
	matRet._11 = (FLOAT)(ViewPort.Width)/2;
	matRet._22 = -(FLOAT)(ViewPort.Height)/2;
	matRet._33 = ViewPort.MaxZ - ViewPort.MinZ;
	matRet._41 = ViewPort.X + matRet._11;
	matRet._42 = ViewPort.Y - matRet._22;
	matRet._43 = ViewPort.MinZ;
	matRet._44 = 1;

	return;
}

//BOOL MT_IsFloatEqualBias( FLOAT f1, FLOAT f2, FLOAT fBias )
//{
//	/*FLOAT fNum = abs((f1 - f2)/f1);
//	return MT_FLOAT_EQUAL_BIAS(fNum, 0, fBias);*/
//	//正确的原理是上面这样的。参考CSDN的相关讨论，因为除法比较容易精度误判，所以这样子：
//
//	if(abs(f1) < abs(f2))
//		std::swap(f1, f2);	//f1总是比较大
//
//	if(f2 == 0)
//		return MT_FLOAT_EQUAL_BIAS(f1, f2, fBias);
//
//	return abs(f1 - f2) < fBias * abs(f2);
//}
D3DXVECTOR3& D3DXMatrixGetTranslationPart( D3DXMATRIX& mat )
{
	return *(D3DXVECTOR3*)&mat._41;
}

D3DXMATRIX g_stMatIdentity(1,0,0,0
								,0,1,0,0
								,0,0,1,0
								,0,0,0,1);

const D3DXMATRIX& D3DXMatrixGetIdentity()
{
	return g_stMatIdentity;
}

//交点Vq = vSrc + (vDir * (vP - vSrc)) * vDir
D3DXVECTOR2 PointDistanceToLine( const D3DXVECTOR2& vP, const D3DXVECTOR2& vSrc, const D3DXVECTOR2& vNormalizedDir )
{
	D3DXVECTOR2 vTemp = vP - vSrc;
	return vSrc + D3DXVec2Dot(&vNormalizedDir, &vTemp) * vNormalizedDir;
}

D3DXVECTOR3 PointDistanceToLine( const D3DXVECTOR3& vP, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir )
{
	D3DXVECTOR3 vTemp = vP - vSrc;
	return vSrc + D3DXVec3Dot(&vNormalizedDir, &vTemp) * vNormalizedDir;
}

FLOAT TriangleArea( const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3 )
{
	D3DXVECTOR3 vRet;
	return D3DXVec3Length(D3DXVec3Cross(&vRet, &(p2-p1), &(p3-p1)))/2;
}

size_t GetVertexStrideFromFVF( DWORD dwFVF )
{
	typedef std::pair<DWORD, unsigned>	TypeElem;
	static TypeElem elems[] =	
	{
		TypeElem(D3DFVF_XYZ,	sizeof(FLOAT)*3),
		TypeElem(D3DFVF_XYZRHW,	sizeof(FLOAT)*4),

		TypeElem(D3DFVF_NORMAL, sizeof(FLOAT)*3),
		TypeElem(D3DFVF_PSIZE, sizeof(FLOAT)*1),
		TypeElem(D3DFVF_DIFFUSE, sizeof(FLOAT)*1),
		TypeElem(D3DFVF_SPECULAR, sizeof(FLOAT)*1),		
	};

	std::vector<unsigned> vecParsedFVF;
	vecParsedFVF.reserve(10);
	for (size_t i = 0; i < _countof(elems); ++i)
	{
		TypeElem& elem = elems[i];
		if(elem.first & dwFVF)
			vecParsedFVF.push_back(i);
	}

	DWORD ParsedFVF = 0;
	size_t	Ret = 0;
	for (size_t i =0; i < vecParsedFVF.size(); ++i)
	{
		TypeElem& elem =elems[vecParsedFVF[i]];
		ParsedFVF |= elem.first;
		Ret += elem.second;
	}

	//贴图要另外分解
	{
		DWORD dwTexCoordCount = (dwFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
		ParsedFVF |= (dwFVF & D3DFVF_TEXCOUNT_MASK);
		Ret += (size_t)dwTexCoordCount * sizeof(FLOAT) *2;

		//D3DFVF_TEXCOORDSIZE3 解释不了
	}

	if(ParsedFVF == dwFVF)
	{
		_ASSERTE(Ret != 0);
		return (size_t)Ret;
	}

	return 0;
}
BOOL D3DXMatrixProjIsOrtho(const D3DXMATRIX& mat)
{
	return MT_FLOAT_EQUAL(mat._34, 0);
}
BOOL D3DXMatrixGetZNearFromProj( const D3DXMATRIX& mat, FLOAT& Ret )
{
	if(mat._33 == 0)
		return FALSE;

	/*D3DXMatrixOrthoLH 
	2/w  0    0           0
	0    2/h  0           0
	0    0    1/(zf-zn)   0
	0    0    zn/(zn-zf)  1


	D3DXMatrixPerspectiveFovLH
	2*zn/w  0       0              0
	0       2*zn/h  0              0
	0       0       zf/(zf-zn)     1
	0       0       zn*zf/(zn-zf)  0
	*/


	Ret = -mat._43 / mat._33;
	return TRUE;
}
BOOL D3DXMatrixGetZFarFromProj(const D3DXMATRIX& mat, FLOAT& Ret)
{
	FLOAT ZNear = 0;
	BOOL bRet = D3DXMatrixGetZNearFromProj(mat, ZNear);
	if(! bRet)
		return FALSE;

	FLOAT f33 = mat._33;
	_ASSERTE(ZNear != 0);
	if (! D3DXMatrixProjIsOrtho(mat))
	{
		if (f33 == 1)
		{
			Ret = FLT_MAX;
		}
		else
			Ret = f33 * ZNear / (f33 - 1);//用33逆推可以得到Zf = 33 * Zn / (33 -1)
	}
	else
	{
		Ret = 1/f33 + ZNear;
	}
	return TRUE;
}

BOOL D3DXMatrixProjResetZFar( D3DXMATRIX& mat, FLOAT newZFar )
{
	FLOAT ZNear = 0;
	BOOL bRet = D3DXMatrixGetZNearFromProj(mat, ZNear);
	if(! bRet)
		return FALSE;
	return D3DXMatrixProjResetZNearFar(mat, ZNear, newZFar);
}
BOOL D3DXMatrixProjResetZNearFar( D3DXMATRIX& mat, FLOAT newZNear, FLOAT newZFar )
{
	FLOAT ZFarMinusZNear = newZFar - newZNear;
	if(ZFarMinusZNear == 0)
		return FALSE;

	if (! D3DXMatrixProjIsOrtho(mat))
	{
		mat._33 = newZFar / ZFarMinusZNear;
		mat._43 = -newZNear * newZFar / ZFarMinusZNear;
		return TRUE;
	}

	mat._33 = 1 / ZFarMinusZNear;
	mat._43 = -newZNear / ZFarMinusZNear;
	return TRUE;
}
D3DXVECTOR3* D3DXMatrixGetPosOfView( const D3DXMATRIX* pMatView, D3DXVECTOR3* pOut )
{
	/*
	假设摄像机在Pos点，那么Pos * MatView 必然等于0点，那么用0点乘以View矩阵的逆矩阵就可以得到Pos了
	*/
	D3DXMATRIX Inv;
	_ASSERTE(NULL != pOut && NULL != pMatView);
	if(NULL == D3DXMatrixInverse(&Inv, NULL, pMatView))
		return NULL;	
	
	return D3DXVec3TransformCoord(pOut, &D3DXVECTOR3(0,0,0), &Inv);
}
D3DXVECTOR3* D3DXMatrixGetDirOfView( const D3DXMATRIX* pMatView, D3DXVECTOR3* pOut )
{
	D3DXMATRIX Inv;
	_ASSERTE(NULL != pOut && NULL != pMatView);
	if(NULL == D3DXMatrixInverse(&Inv, NULL, pMatView))
		return NULL;
	return D3DXVec3TransformNormal(pOut, &D3DXVECTOR3(0,0,1), &Inv);
}
D3DXVECTOR3 D3DXMatrixGetRightOfView( const D3DXMATRIX& matView )
{
	return D3DXVECTOR3(matView._11, matView._21, matView._31);
}
D3DXVECTOR3 D3DXMatrixGetUpOfView( const D3DXMATRIX& matView )
{
	return D3DXVECTOR3(matView._12, matView._22, matView._32);
}
const D3DXVECTOR3& D3DXVec3GetNormalOfPlane( size_t i )
{
	static D3DXVECTOR3 PlaneNormals[] = 
	{
		D3DXVECTOR3(1,0,0),
		D3DXVECTOR3(0,1,0),
		D3DXVECTOR3(0,0,1)
	};
	_ASSERTE(i < _countof(PlaneNormals));
	return PlaneNormals[i];
}
const D3DXMATRIX& D3DXMatrixGetRotationMatrixOfAxis( size_t i )	//这些旋转矩阵是指从X轴正向转到其它向的矩阵
{
	static D3DXMATRIX	RotationMatrixArray[] = 
	{		
		D3DXMATRIX(
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
		),//YZ
		D3DXMATRIX(
		0,1,0,0,
		-1,0,0,0,
		0,0,1,0,
		0,0,0,1
		),//XZ
		D3DXMATRIX(
		0,0,1,0,
		0,1,0,0,
		-1,0,0,0,
		0,0,0,1
		),//XY		
	};
	_ASSERTE(i < _countof(RotationMatrixArray));
	return RotationMatrixArray[i];
}
D3DXVECTOR3 ProjectVectorToPlane( const D3DXVECTOR3& vVec, const D3DXVECTOR3& vPlaneNormal )
{
	return vVec - ProjectVectorToDirection(vVec, vPlaneNormal);
}

D3DXVECTOR3 ProjectPointToPlane( const D3DXVECTOR3& vP, const D3DXPLANE& plane)
{
	const D3DXVECTOR3& Normal = (const D3DXVECTOR3&)plane;
	return vP - (D3DXVec3Dot(&vP, &Normal) + plane.d) * Normal;
}

D3DXVECTOR3 ProjectPointToPlane( const D3DXVECTOR3& vP, const D3DXVECTOR3& vOnePointOnPlane, const D3DXVECTOR3& vNormalOfPlane )
{
	D3DXPLANE plane;
	D3DXPlaneFromPointNormal(&plane, &vOnePointOnPlane, &vNormalOfPlane);
	return ProjectPointToPlane(vP, plane); 
}
D3DXVECTOR3 ProjectPointToRay( const D3DXVECTOR3& vP, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vDir )
{
	FLOAT t = D3DXVec3Dot(&vDir, &(vP - vSrc));
	return vSrc + vDir * t;
}
FLOAT GetRectDiagonalLength(RECT rect)
{
	FLOAT fWidth = (FLOAT)(rect.right - rect.left);
	FLOAT fHeight = (FLOAT)(rect.bottom - rect.top);
	return sqrt(fWidth * fWidth + fHeight * fHeight);
}

HRESULT CalculatePickRay( const D3DXVECTOR2& vScreenPos, const D3DXMATRIX& matViewProjection ,  const D3DVIEWPORT9& viewport, D3DXVECTOR3& vSrcRet, D3DXVECTOR3& vNormalizedDirRet)
{
	D3DXMATRIX matViewport;
	D3DXMatrixViewPort(viewport, matViewport);

	return CalculatePickRay(vScreenPos, matViewProjection * matViewport, vSrcRet, vNormalizedDirRet);

	
}

HRESULT CalculatePickRay( const D3DXVECTOR2& vScreenPos, const D3DXMATRIX& matViewProjViewport , D3DXVECTOR3& vSrcRet, D3DXVECTOR3& vNormalizedDirRet )
{
	D3DXMATRIX matVPVInv;
	D3DXMatrixInverse(&matVPVInv, NULL, &matViewProjViewport);

	D3DXVECTOR3 vSrc;
	D3DXVec3TransformCoord(&vSrc, &D3DXVECTOR3(vScreenPos.x, vScreenPos.y, 0), &matVPVInv);	//PosWindow是屏幕坐标，应该在近平面上，Z是0

	D3DXVECTOR3 vDirDest(vScreenPos.x, vScreenPos.y, 1);

	D3DXVec3TransformCoord(&vDirDest, &vDirDest, &matVPVInv);
	D3DXVECTOR3 vDir;
	D3DXVec3Normalize(&vDir, &(vDirDest - vSrc));

	vSrcRet = vSrc;
	vNormalizedDirRet = vDir;
	return S_OK;
}
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KGMathFunctionsSpecial)
{
	{
		FLOAT f2 = 0.f;
		FLOAT fNum = 1.f / f2;	//根据编译选项,这里可能会引发除零异常，但是TDD架构能截获这个异常，不用担心
		_ASSERTE(! IS_FLOAT_NAN(fNum));
		_ASSERTE(IS_FLOAT_INFINITE(fNum));
		_ASSERTE(! IS_FLOAT_VALID(fNum));

		FLOAT fNum2 = -1.f / f2;
		_ASSERTE(! IS_FLOAT_NAN(fNum2));
		_ASSERTE(IS_FLOAT_INFINITE(fNum2));
		_ASSERTE(! IS_FLOAT_VALID(fNum2));


		_ASSERTE(! IS_FLOAT_NAN(1.f));
		_ASSERTE(! IS_FLOAT_INFINITE(1.f));
		_ASSERTE(IS_FLOAT_VALID(1.f));

		FLOAT fNum3 = sqrt(-1.f);
		_ASSERTE(IS_FLOAT_NAN(fNum3));
		_ASSERTE(IS_FLOAT_INFINITE(fNum3));
		_ASSERTE(! IS_FLOAT_VALID(fNum3));
	}
}
KG_TEST_END(KGMathFunctionsSpecial)
#endif
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KGMathFunctions)
{
#if 1
	//D3DCOLOR系列宏
	{
		D3DCOLOR color = D3DCOLOR_ARGB(255,111,123,233);
		_ASSERTE(D3DCOLOR_GETALPHA(color) == 255);
		_ASSERTE(D3DCOLOR_GETRED(color) == 111);
		_ASSERTE(D3DCOLOR_GETGREEN(color) == 123);
		_ASSERTE(D3DCOLOR_GETBLUE(color) == 233);

		D3DCOLOR_SETALPHA(color, 123);
		_ASSERTE(D3DCOLOR_GETALPHA(color) == 123);

	}
	//MT_FLOAT_EQUAL宏
	{
		BOOL bRet = MT_FLOAT_UNEQUAL(1.0f, 1.0f+FLT_EPSILON+FLT_EPSILON);
		_ASSERTE(bRet);
		bRet  = MT_FLOAT_UNEQUAL(1.0f, 1.0f);
		_ASSERTE(! bRet);

		_ASSERTE(MT_FLOAT_IS_A_LESS_THAN_B(0, FLT_EPSILON));
		_ASSERTE(MT_FLOAT_IS_A_LESS_OR_EQUAL_B(0, 0));
	}
	//MT_Round
	{
		struct Case
		{
			FLOAT In;
			INT Out;
		};
		Case CaseArray[] = {
			{0.1f, 0},
			{0,	0},
			{0.5f, 1},
			{1.2f, 1},
			{1.5f, 2},
			{-0.1f, 0},
			{-2.6f, -3},
			{-1.2f, -1},
		};
		for (size_t i = 0; i < _countof(CaseArray); ++i)
		{
			_ASSERTE(FourCullFiveAdd(CaseArray[i].In) == CaseArray[i].Out);
		}
	}
	{
		FLOAT fOnePlus = 1.f+FLT_EPSILON;

		_ASSERTE(0 == MT_FLOAT_TRIM_TAIL(0.1f));
		_ASSERTE(1 == MT_FLOAT_TRIM_TAIL(fOnePlus));
		_ASSERTE(0 == MT_FLOAT_TRIM_TAIL(-FLT_EPSILON));
		_ASSERTE(-1 == MT_FLOAT_TRIM_TAIL(-fOnePlus));
	}

	//D3DXMatrixGetTranslationPart
	{
		//Modify
		D3DXVECTOR3 Trans(1,2,3);

		D3DXMATRIX Mat;
		D3DXMatrixIdentity(&Mat);

		D3DXMatrixGetTranslationPart(Mat) = Trans;
		_ASSERTE(Mat._41 == Trans.x && Mat._42 == Trans.y && Mat._43 == Trans.z );

		//Read
		D3DXVECTOR3 Out;
		D3DXVec3Add(&Out, &D3DXMatrixGetTranslationPart(Mat), &(-Trans));
		_ASSERTE(Out.z == 0 && Out.y == 0 && Out.z == 0);

	}
	//PointDistanceToLine
	{
		struct TestCases 
		{
			D3DXVECTOR2	vP;
			D3DXVECTOR2 vSrc;
			D3DXVECTOR2	vDir;
			D3DXVECTOR2	vRet;
		};

		TestCases CaseArray[] = {
			{D3DXVECTOR2(0, -1), D3DXVECTOR2(0, 0), D3DXVECTOR2(1, 0), D3DXVECTOR2(0, 0)},
			{D3DXVECTOR2(-1, 1), D3DXVECTOR2(0, 0), D3DXVECTOR2(0, -1), D3DXVECTOR2(0, 1)},
			{D3DXVECTOR2(0, 0), D3DXVECTOR2(0, 2), D3DXVECTOR2(1, -1), D3DXVECTOR2(1, 1)},
			{D3DXVECTOR2(0, 1), D3DXVECTOR2(0, 0), D3DXVECTOR2(0, 1), D3DXVECTOR2(0, 1)},
			{D3DXVECTOR2(1, 0), D3DXVECTOR2(0, 0), D3DXVECTOR2(1, 0), D3DXVECTOR2(1, 0)},
		};

		for (size_t i = 0; i < _countof(CaseArray); i++)
		{
			D3DXVec2Normalize(&CaseArray[i].vDir, &CaseArray[i].vDir);
			D3DXVECTOR2 vRet = PointDistanceToLine(CaseArray[i].vP,CaseArray[i].vSrc,CaseArray[i].vDir);
			vRet -= CaseArray[i].vRet;
			_ASSERTE(MT_FLOAT_EQUAL_BIAS(vRet.x, 0, 0.001f) && MT_FLOAT_EQUAL_BIAS(vRet.y, 0, 0.001f));
		}
	}

	//TriangleArea
	{
		struct TestCase 
		{
			D3DXVECTOR3	p1;
			D3DXVECTOR3 p2;
			D3DXVECTOR3	p3;
			FLOAT		Area;
		};

		TestCase CaseArray[] = {
			{D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 1, 0), D3DXVECTOR3(1, 0, 0), 0.5f},
			{D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 0, 0), D3DXVECTOR3(0, 0, 4), 2.0f},
			{D3DXVECTOR3(-1, 0, 0), D3DXVECTOR3(1, 0, 0), D3DXVECTOR3(0, 2, 0), 2.0f},
		};

		for (size_t i = 0; i < _countof(CaseArray); i++)
		{
			FLOAT Area = TriangleArea(CaseArray[i].p1,CaseArray[i].p2,CaseArray[i].p3);
			_ASSERTE(MT_FLOAT_EQUAL_BIAS(Area, CaseArray[i].Area, 0.001f));
		}
	}

	//GetVertexStrideFromFVF
	{
		struct TestCase 
		{
			DWORD dwFVF;
			size_t	Size;
		};

		TestCase CaseArray[] = {
			{D3DFVF_XYZ, sizeof(FLOAT)*3},
			{D3DFVF_NORMAL, sizeof(FLOAT)*3},
			{D3DFVF_XYZ | D3DFVF_NORMAL, sizeof(FLOAT)*6},
			{D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, sizeof(FLOAT)*8},
			{D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2, sizeof(FLOAT)*10},
			{D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0), 0},	//不能分辨的标志
			{D3DFVF_XYZB2, 0},
		};

		for (size_t i = 0; i < _countof(CaseArray); ++i)
		{
			size_t Size = GetVertexStrideFromFVF(CaseArray[i].dwFVF);
			_ASSERTE(Size == CaseArray[i].Size);
		}
	}
	//D3DXMatrixGetPosOfView系列
	{
		D3DXMATRIX outMat;
		D3DXVECTOR3 Eye(100,-200,300);
		D3DXVECTOR3 Dir(1,-1,3);
		D3DXVec3Normalize(&Dir, &Dir);
		D3DXVECTOR3 At = Eye + Dir * 10; 
		D3DXVECTOR3 Up(0,1,0);
		D3DXMatrixLookAtLH(&outMat, &Eye, &At, &Up);

		D3DXVECTOR3 vOut;		
		D3DXMatrixGetPosOfView(&outMat, &vOut);
		_ASSERTE(MT_VECTOR_EQUAL(vOut, Eye,0.001f));
		
		D3DXMatrixGetDirOfView(&outMat, &vOut);
		_ASSERTE(MT_VECTOR_EQUAL(vOut, Dir, 0.001f));

		D3DXVECTOR3 vRight = D3DXMatrixGetRightOfView(outMat);
		{
			D3DXMATRIX matViewInv;
			D3DXMatrixInverse(&matViewInv, NULL, &outMat);
			D3DXVECTOR3 vRightGotInAnotherWay;
			D3DXVec3TransformNormal(&vRightGotInAnotherWay, &D3DXVECTOR3(1,0,0), &matViewInv);
			_ASSERTE(MT_VECTOR_EQUAL(vRight, vRightGotInAnotherWay, 0.001f));
		}

		D3DXVECTOR3 vUp = D3DXMatrixGetUpOfView(outMat);
		{
			D3DXMATRIX matViewInv;
			D3DXMatrixInverse(&matViewInv, NULL, &outMat);
			D3DXVECTOR3 vUpGotInAnotherWay;
			D3DXVec3TransformNormal(&vUpGotInAnotherWay, &D3DXVECTOR3(0,1,0), &matViewInv);
			_ASSERTE(MT_VECTOR_EQUAL(vUp, vUpGotInAnotherWay, 0.001f));
		}
	}

	//D3DXMatrixGetZNearFromViewMatrix系列函数
	{
		FLOAT Zn = 10.f;
		FLOAT Zf = 1000.f;
		D3DXMATRIX OutMat;
		FLOAT Temp = 0;
		D3DXMatrixPerspectiveFovLH(&OutMat, D3DX_PI /4, 800.f / 600.f, Zn, Zf);
		_ASSERTE(D3DXMatrixGetZNearFromProj(OutMat, Temp) && MT_FLOAT_EQUAL_BIAS(Temp, Zn, 0.01f));
		_ASSERTE(D3DXMatrixGetZFarFromProj(OutMat, Temp) && MT_FLOAT_EQUAL_BIAS(Temp, Zf, 0.01f));
		_ASSERTE(! D3DXMatrixProjIsOrtho(OutMat));

		D3DXMatrixOrthoLH(&OutMat, 800.f, 600.f, Zn, Zf);
		_ASSERTE(D3DXMatrixGetZNearFromProj(OutMat, Temp) && MT_FLOAT_EQUAL_BIAS(Temp, Zn, 0.01f));
		_ASSERTE(D3DXMatrixGetZFarFromProj(OutMat, Temp) && MT_FLOAT_EQUAL_BIAS(Temp, Zf, 0.01f));
		_ASSERTE(D3DXMatrixProjIsOrtho(OutMat));

		D3DXMatrixPerspectiveFovLH(&OutMat, D3DX_PI /4, 800.f / 600.f, Zn, Zf);
		OutMat._33 = 0;
		_ASSERTE(! D3DXMatrixGetZNearFromProj(OutMat, Temp));
		_ASSERTE(! D3DXMatrixGetZFarFromProj(OutMat, Temp));
		
		{
			D3DXMatrixPerspectiveFovLH(&OutMat, D3DX_PI /4, 800.f / 600.f, Zn, Zf);
			{
				D3DXMATRIX mat2 = OutMat;
				FLOAT newZFar = Zf + 100.f;
				D3DXMatrixProjResetZFar(mat2, newZFar);

				FLOAT ZFarRet = 0;
				D3DXMatrixGetZFarFromProj(mat2, ZFarRet);
				_ASSERTE(MT_FLOAT_EQUAL_BIAS(ZFarRet, newZFar, 0.01f));
			}
			D3DXMatrixPerspectiveFovLH(&OutMat, D3DX_PI /4, 800.f / 600.f, Zn, Zf);
			{
				FLOAT newZNear = 5;
				FLOAT newZFar = 10;
				D3DXMATRIX mat2 = OutMat;
				D3DXMatrixProjResetZNearFar(mat2, newZNear, newZFar);

				FLOAT ZNearRet = 0;
				FLOAT ZFarRet = 0;
				D3DXMatrixGetZFarFromProj(mat2, ZFarRet);
				D3DXMatrixGetZNearFromProj(mat2, ZNearRet);
				_ASSERTE(MT_FLOAT_EQUAL_BIAS(ZFarRet, newZFar, 0.01f));
				_ASSERTE(MT_FLOAT_EQUAL_BIAS(ZNearRet, newZNear, 0.01f));
			}
			D3DXMatrixOrthoLH(&OutMat, 800, 600, Zn, Zf);
			{
				FLOAT newZNear = 5;
				FLOAT newZFar = 10;
				D3DXMATRIX mat2 = OutMat;
				D3DXMatrixProjResetZNearFar(mat2, newZNear, newZFar);

				FLOAT ZNearRet = 0;
				FLOAT ZFarRet = 0;
				D3DXMatrixGetZFarFromProj(mat2, ZFarRet);
				D3DXMatrixGetZNearFromProj(mat2, ZNearRet);
				_ASSERTE(MT_FLOAT_EQUAL_BIAS(ZFarRet, newZFar, 0.01f));
				_ASSERTE(MT_FLOAT_EQUAL_BIAS(ZNearRet, newZNear, 0.01f));
			}
		}
	}
	//ProjectVectorToPlane
	{
		D3DXVECTOR3 vRet = ProjectVectorToPlane(D3DXVECTOR3(1,0,0), D3DXVECTOR3(0,1,0));
		D3DXVECTOR3 vCaseResult = D3DXVECTOR3(1,0,0);
		_ASSERTE(MT_VECTOR_EQUAL(vRet,vCaseResult, 0.01f));
		vRet = ProjectVectorToPlane(D3DXVECTOR3(0,1,0), D3DXVECTOR3(0,1,0));
		vCaseResult = D3DXVECTOR3(0,0,0);
		_ASSERTE(MT_VECTOR_EQUAL(vRet,vCaseResult, 0.01f));
		vRet = ProjectVectorToPlane(D3DXVECTOR3(1,1,1), D3DXVECTOR3(0,1,0));
		vCaseResult = D3DXVECTOR3(1,0,1);
		_ASSERTE(MT_VECTOR_EQUAL(vRet,vCaseResult, 0.01f));
	}

#endif
}


KG_TEST_END(KGMathFunctions)
#endif

