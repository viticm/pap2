////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFrameDrawer.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-29 14:58:23
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DFRAMEDRAWER_H_
#define _INCLUDE_KG3DFRAMEDRAWER_H_

////////////////////////////////////////////////////////////////////////////////
struct KG3DFrameDrawerBasic	//可以用D3D的绘制函数
{
	enum{em_fvf = D3DFVF_XYZ,};
	/*
	两个矩阵如果不给的话，会自动从Device里面取一次，所以如果Begin之前要设Transform的话，应该在这里设，不然会被覆盖。
	默认使用默认的渲染状态渲染，如果设为FALSE的话，需要自己管理渲染状态
	*/
	virtual HRESULT Begin(const D3DXMATRIX* pWorld = NULL, const D3DXMATRIX* pView = NULL
						, const D3DXMATRIX* pProj = NULL, BOOL UseDefaultStates = TRUE) = 0;	
	
	virtual HRESULT End() = 0;
	virtual HRESULT SetTransform(const D3DXMATRIX* pWorld, const D3DXMATRIX* pView, const D3DXMATRIX* pPerspective) = 0;
	virtual HRESULT SetColor(const D3DCOLORVALUE& color) = 0;

	virtual ~KG3DFrameDrawerBasic() = 0{}
};
struct KG3DFrameDrawer : public KG3DFrameDrawerBasic
{
	virtual VOID	SetLineColor(D3DCOLOR color) = 0;
	virtual VOID	SetFrameColor(D3DCOLOR color) = 0;
	virtual VOID	SetParameters(BOOL bEnableFace, BOOL bEnableLine, D3DCOLOR FaceColor, D3DCOLOR LineColor) = 0;
	virtual HRESULT SetColor(const D3DCOLORVALUE& color) = 0;	//同时设Line和Frame的颜色
	virtual HRESULT Draw(const D3DXVECTOR3& UpperLeft, const D3DXVECTOR3& UpperRight, const D3DXVECTOR3& LowerRight) = 0;
};

struct KG3DFrameDrawerCone : public KG3DFrameDrawer 
{
	virtual HRESULT DrawCone(const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, FLOAT fBottomRadius, FLOAT fHeight) = 0;
};

struct KG3DFrameDrawerFan : public KG3DFrameDrawerCone
{
	//半径由Begin到Center的位置决定,Begin到End永远走顺时针(Normal的左手法则)。Angle会Clamp到正负2Pi
	virtual HRESULT DrawFan(const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, const D3DXVECTOR3& Begin, FLOAT fAngle) = 0;
	virtual HRESULT DrawCircle(const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, FLOAT Radius) = 0;	//画圆
};

struct KG3DFrameDrawerBox : public KG3DFrameDrawer 
{
	virtual HRESULT SetAxis(const D3DXVECTOR3& vX, const D3DXVECTOR3& vZ) = 0;
	virtual HRESULT SetRange(const D3DXVECTOR3& vRangeXYZ) = 0;
	virtual HRESULT DrawBox(const D3DXVECTOR3& Center) = 0;
};

//所有的Drawer，在一个Begin之后，另外一个都不能Begin，直到End，这和Shader本身是一致的，一个Effect调用Begin，就不能Begin另外一个Effect
extern KG3DFrameDrawerBasic& g_GetFrameDrawerBasic();	//所有的用颜色画的东西都可以用这个Drawer，这个Drawer如果Begin了之后，可以直接调用d3d的绘制函数
extern KG3DFrameDrawer& g_GetFrameDrawerRect();
extern KG3DFrameDrawer& g_GetFrameDrawerTri();
extern KG3DFrameDrawerFan& g_GetFrameDrawerFan();
extern KG3DFrameDrawerCone& g_GetFrameDrawerCone();
extern KG3DFrameDrawerBox& g_GetFrameDrawerBox();

extern VOID g_CheckFrameDrawers(const D3DXVECTOR3& vPos, FLOAT fRange);	//用于检查绘制正确与否


struct KG3DTexFrameDrawer : public KG3DFrameDrawer
{
	virtual HRESULT SetTexture(LPDIRECT3DTEXTURE9 pTexture) = 0;
};

#endif //_INCLUDE_KG3DFRAMEDRAWER_H_
