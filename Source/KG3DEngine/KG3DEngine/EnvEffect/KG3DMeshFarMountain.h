////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMeshFarMountain.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-10-19 17:46:21
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////
# pragma once
#ifndef _INCLUDE_KG3DMESHFARMOUNTAIN_H_
#define _INCLUDE_KG3DMESHFARMOUNTAIN_H_

////////////////////////////////////////////////////////////////////////////////


//#include "KG3DMessenger.h"
#include "IEKG3DMeshFarMountain.h"
#include "KG3DEnvEffect.h"
//#include "KG3DGraphicsStructEx.h"

class KG3DTexture;
namespace GraphicsStruct
{
	class RenderStateAlphaWithoutSettingAlphaTest;
};

namespace KG3D_ENVIRONMENT_EFFECT
{
	//////////////////////////////////////////////////////////////////////////
	struct KG3DDWGC_Key_FarMount : public KG3DDWGC_KeyBase
	{
		enum { DWGCKEY_TYPE = KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN };

		virtual DWORD GetType() const { return DWGCKEY_TYPE; }

        DWORD		m_dwModificationOption;		//只能够改雾色，雾色同步必须打开
		//D3DCOLOR	m_d3dc_Color;				//与雾同步的主色
		FLOAT		m_fFogBlendFactor;			//雾颜色与贴图色的混合系数

		KG3DDWGC_Key_FarMount();

		virtual bool CalculateInterp(const KG3DDWGC_KeyBase& BaseKeyBefore, const KG3DDWGC_KeyBase& BaseKeyAfter, float fPercent);

		bool SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection );
		bool LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection );

#if defined(DEBUG) | defined(_DEBUG)
		void DebugPrint(std::ostringstream& oss);
#endif
	};

	//////////////////////////////////////////////////////////////////////////
	struct KG3DMeshFarMountDescStruct : public KG3DMESHFARMOUNTAIN_DESCSTRUCT
	{
		//不要试图派生任何有虚函数的类，否则所有的运算符重载就不能正常工作了
		KG3DMeshFarMountDescStruct();		
		KG3DMeshFarMountDescStruct( const KG3DMeshFarMountDescStruct &OtherStruct );
		KG3DMeshFarMountDescStruct( const KG3DMESHFARMOUNTAIN_DESCSTRUCT &OtherStruct );
		void SynWithKey(const KG3DDWGC_Key_FarMount& Key);///<DW_Frame>
		void operator=( const KG3DMeshFarMountDescStruct& OtherStruct );
		void operator=( const KG3DMESHFARMOUNTAIN_DESCSTRUCT& OtherStruct );
		void ToBaseStruct( KG3DMESHFARMOUNTAIN_DESCSTRUCT &OtherStruct );
		void ToKey(KG3DDWGC_Key_FarMount& OtherStruct);///<DW_Frame>
		bool SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection );
		bool LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection );
		bool SetDefaultValue();
	};


    class KG3DMeshFarMountain : 
        public KG3DEnvEffect,
        //public KG3DMessageSingleSenderReceiver, 
        public IEKG3DMeshFarMountain
	{
	private:
		//BBox		m_BBox;
		//LPD3DXMESH	m_pD3DXMesh;
		struct TypeVertex 
		{
			FLOAT x,y,z;
			FLOAT uPercent;
		};
		enum{
			em_slice = 32,
			em_fvf = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE1(0),
			
		};
		LPDIRECT3DVERTEXBUFFER9	m_pVB;
		KG3DMeshFarMountDescStruct	m_DescStruct;	
		KG3DTexture* m_pColorMap;
		//unsigned	m_uTextureHeight;
		//unsigned	m_uTextureWidth;
		//bool	m_bIsTextureLoaded;
		//TCHAR	m_tczTexturePathBuffer[MAX_PATH];
		
		bool	m_bIsInitialized;
		TCHAR	m_tczName[32];				//用于标志名字

		//D3DCOLOR m_d3dc_FogBlendedColor;	//DescStruct中的参数FogColor和FogBlendFactor相乘之后得到的结果
		//bool	m_bBlockReloadMessage;		//用于主动重载时，不接受重载的消息

	//public:
	/*	struct RenderState
		{
			GraphicsStruct::RenderStateAlphaWithoutSettingAlphaTest	m_AlphaState;
			GraphicsStruct::SamplerStateAddressVClamp m_AddressVClamp;
			RenderState();
			~RenderState();
		};*/

	public:
		///<接口>
		virtual HRESULT GetName( LPTSTR szName, UINT uBufferCount );
		virtual HRESULT SetName( LPTSTR szName );
		virtual HRESULT	RequestModification( const KG3DMESHFARMOUNTAIN_DESCSTRUCT* pStruct );///可以用pStruct中的ModificationOption设定改什么东西
		virtual HRESULT	GetStruct( KG3DMESHFARMOUNTAIN_DESCSTRUCT* pResultStruct );
		///</接口>

		///<KG3DEnvEffect>
        virtual HRESULT OnLostDevice();
        virtual HRESULT OnResetDevice();
		virtual HRESULT Render(const KG3DEnvEffRenderContext& renderContext);
		virtual HRESULT FrameMove();
		virtual HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection );
		virtual HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection );
		virtual HRESULT ToSolidInterface(LPVOID* pEffectInterface);
		///</KG3DEnvEffect>

		LPTSTR	Name() { return m_tczName; }
		const KG3DMeshFarMountDescStruct* GetStruct(){ return &m_DescStruct; };
		HRESULT RequestModification( const KG3DMeshFarMountDescStruct* pStruct );
		
		///<动态天气>
		///<DW_Controller>
	private:
		KG3DDWGC_Key_FarMount m_SubKey;
		bool	m_bEnableBlending;
		//bool	m_bIsSubStateInitialized;
		float	m_fBlendPercent;

	public:
		typedef	KG3DDWGC_Key_FarMount	TypeKey;		

	public:
		HRESULT DWGCRequestPopFrontState(float fCurBlendPercent = 0.0f);
		HRESULT DWGCRequestSubStateChange(const KG3DDWGC_KeyBase* pRetKey);
		HRESULT DWGCGetCurStateToKey(KG3DDWGC_KeyBase* pRetKey);		
		HRESULT DWGCEnableSubState(BOOL bEnable);
		HRESULT DWGCSetPercent(float fPercent);
		HRESULT DWGCRequestModification(const KG3DDWGC_KeyBase* pKey);
		///</DW_Controller>

		///<动态天气>
	private:
		HRESULT CustomInit();
		HRESULT SetTexture( LPCTSTR tczTexturePath );///输入的是相对路径，相对于g_Def_WorkDirectory
		//HRESULT ProcessMesh(LPD3DXMESH	pMeshIn);///新建的时候，载入用的
		//HRESULT ModifyMesh(const float* pfRadius = NULL, const float* pnHeight = NULL, const float* pBotmZPos = NULL, const float *pfZRotation = NULL, const float *pfTextureCordUTiles = NULL );///改变形状，会比较参数是否和内部的Struct一致，如果一致就不理会，除非指定bForceModify
		//HRESULT ModifyTexture(const D3DCOLOR* pColor = NULL, const float* pBotmBlendHeight = NULL, const D3DCOLOR* pFogColor = NULL, const float* pfFogBlendFactor = NULL );///产生贴图下方的渐变,会比较参数是否和内部的Struct一致，如果一致就不理会，除非指定bForceRedraw
		//inline void ModifyFinalBlendedColor(D3DCOLOR SurfaceColor, D3DCOLOR FogColor, float fPercent);
		//virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender );///覆盖IKG3DMessageReceiver中的同名方法
	
		HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption ) { return S_OK; };///封闭这个函数

	public:
		KG3DMeshFarMountain(); 
		~KG3DMeshFarMountain();
	};

};
#endif //_INCLUDE_KG3DMESHFARMOUNTAIN_H_
