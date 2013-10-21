////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DGraphicsStructEx.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-22 16:58:29
//  Comment     : 因为SceneEditor中间接的间接使用了GraphicStruct.h
// 只要向里面添加和引擎有关的东西就会编译不过，只能额外做这个文件
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DGRAPHICSTRUCTEX_H_
#define _INCLUDE_KG3DGRAPHICSTRUCTEX_H_
#include "GraphicsStruct.h"
////////////////////////////////////////////////////////////////////////////////

/*
这些类都是为了改变渲染状态之后能自动还原(利用析构函数)
使用方式都是
ClassABCD abcd(XX,XX);
要注意生命域
*/
namespace GraphicsStruct
{
	//////////////////////////////////////////////////////////////////////////
	struct RenderStateAlphaWithoutSettingAlphaTest///设定基本的AlphaBlend参数，并没有AlphaTest
	{

		DWORD m_dwBackAlphaBlendEnable;
		DWORD m_dwBackSrcBlend;
		DWORD m_dwBackDestlend;
		RenderStateAlphaWithoutSettingAlphaTest(DWORD AlphaBlendEnable = TRUE,DWORD SrcBlend = D3DBLEND_SRCALPHA, DWORD DestBlend = D3DBLEND_INVSRCALPHA);
		~RenderStateAlphaWithoutSettingAlphaTest();
	};
	struct RenderStateGlobleCameraZFar///用于设定全局的Camera的ZFar最大
	{
		D3DXMATRIX m_matProj;
		RenderStateGlobleCameraZFar(FLOAT fZFar = -FLT_EPSILON);	///默认的情况下最快
		~RenderStateGlobleCameraZFar();		
	};
	struct RenderStateWorldMatrixMoveWithCamera///令世界矩阵随着Camera运动
	{
		D3DXMATRIX m_matWorldSave;

		RenderStateWorldMatrixMoveWithCamera();
		~RenderStateWorldMatrixMoveWithCamera();
	};
	struct SamplerStateAddressVClamp///使用V纹理夹贴
	{
		SamplerState	m_SamplerState;
		SamplerStateAddressVClamp(DWORD dwSampler = 0);
		~SamplerStateAddressVClamp();
	};
	struct MaterialWhite	//白色材质
	{
		D3DMATERIAL9	m_Material;
		MaterialWhite();
		~MaterialWhite();
	};
	struct LightDisable		//禁止灯光
	{
		DWORD m_nIndex;
		BOOL m_Value;
		LightDisable(DWORD nWhichLight);
		~LightDisable();
	};
	struct RenderTargetSave		//保存RenderTarget，不能连续在同一个域里面使用
	{
		LPDIRECT3DSURFACE9	m_pTarget;
		D3DVIEWPORT9		m_ViewportSave;//改RenderTarget要顺便保存ViewPort，不然死定了
		DWORD m_uIndex;
		RenderTargetSave(DWORD uIndex);
		~RenderTargetSave();
	};
	struct DelpthStencilSave	//保存DepthStencil
	{
		LPDIRECT3DSURFACE9	m_pDelpthStencil;
		DelpthStencilSave();
		~DelpthStencilSave();
	};
	struct RenderTargetAndDepthStencilSave : public RenderTargetSave, public DelpthStencilSave	//一般SetRenderTarget要同时改RT，Depth，ViewPort，这里一次保存了
	{
		RenderTargetAndDepthStencilSave(DWORD uIndex):RenderTargetSave(uIndex){}
	};
	struct TextureDisable		//禁止某Index的Stage的贴图
	{
		LPDIRECT3DBASETEXTURE9	m_pTexSave;
		DWORD m_uIndex;
		TextureDisable(DWORD Index);
		~TextureDisable();
	};
	struct StencilEnable		//打开Stencil，打开之后一定要关闭，所以要用这个类 
	{
		StencilEnable();
		~StencilEnable();
	};
	struct StencilTwoSidedEnable	//打开双向Stencil，很容易出错，所以打开之后也要自动关掉
	{
		StencilTwoSidedEnable();
		~StencilTwoSidedEnable();
	};
	struct ColorWriteDisable		//禁止颜色输出
	{
		ColorWriteDisable();
		~ColorWriteDisable();
	};
	struct SaveMatrix		//保存某个D3DPT的矩阵
	{
		D3DXMATRIX m_Mat;
		D3DTRANSFORMSTATETYPE m_Type;
		SaveMatrix(D3DTRANSFORMSTATETYPE dwType);//D3DTS_WORLD
		~SaveMatrix();
	};
	struct FVFSetter		//保存FVF
	{
		DWORD dwFVF;
		FVFSetter(DWORD dwFVFNew);
		~FVFSetter();
	};
};

class KG3DCameraSphere
{
public :
	void Update(D3DXVECTOR3& vCenter, float fRaduis);

	bool CheckModelCross(KG3DModel* pModel);
	bool CheckFacesCross(D3DXVECTOR3* pFaceVertex, DWORD dwFaceNum);

	inline ObstructdInfo* GetObstructdInfo() {return &m_ObsInfo;}
	inline D3DXVECTOR3*   GetCenter() {return &m_vCenter;}
	inline float          GetRadius() {return  m_fRadius;}

	KG3DCameraSphere();
	virtual ~KG3DCameraSphere();

protected :
	bool _ChkModelCross(KG3DModel* pModel);

protected :
	D3DXVECTOR3 m_vCenter;
	float       m_fRadius;

	ObstructdInfo m_ObsInfo;
};


#endif //_INCLUDE_KG3DGRAPHICSTRUCTEX_H_
