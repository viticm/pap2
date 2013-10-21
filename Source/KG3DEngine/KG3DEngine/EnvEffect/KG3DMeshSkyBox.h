////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMeshSkyBox.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-12-19 12:14:39
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _INCLUDE_KG3DMESHSKYBOX_H_
#define _INCLUDE_KG3DMESHSKYBOX_H_

////////////////////////////////////////////////////////////////////////////////

#include "IEKG3DMeshSkyBox.h"
#include "KG3DMessenger.h"

//#include "KG3DEnvEffDW_HelperClasses.h"
//#include "KG3DEnvEffDW_BaseClasses.h"
#include "KG3DEnvEffect.h"
#include "KG3DGraphicsStructEx.h"
//#include "TemplateLibs/LOKI_APP/KG3DLokiStructInterface.h"
//using namespace std;

class KG3DTexture;
namespace KG3D_ENVIRONMENT_EFFECT
{
	struct KG3DDWGC_Key_SkyBox : public KG3DMESHSKYBOX_DESCSTRUCT, public KG3DDWGC_KeyBase
	{
		enum
		{
			DWGCKEY_TYPE = KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX,
		};
		virtual DWORD GetType()const{return DWGCKEY_TYPE;}
		
		virtual bool CalculateInterp(const KG3DDWGC_KeyBase& BaseKeyBefore
			, const KG3DDWGC_KeyBase& BaseKeyAfter
			, float fPercent);
		bool SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection );
		bool LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection );
		//bool IsLegal(KG3DDWGC_KeyTableManagerBase* pTableManager)const;
		KG3DDWGC_Key_SkyBox()
		{
			//ZeroMemory(this, sizeof(KG3DDWGC_Key_SkyBox));(错误，不能这样的)
		}
#if defined(DEBUG) | defined(_DEBUG)
		void DebugPrint(std::ostringstream& OSS);
#endif		
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct	KG3DMeshSkyBoxDescStruct: public KG3DMESHSKYBOX_DESCSTRUCT 	//存储渐变方式
	{
		//前面的部分在KG3DMESHSKYBOX_DESCSTRUCT
		KG3DMeshSkyBoxDescStruct();
		KG3DMeshSkyBoxDescStruct(const KG3DMeshSkyBoxDescStruct& OtherStruct);
		KG3DMeshSkyBoxDescStruct(const KG3DMESHSKYBOX_DESCSTRUCT& OtherStruct);
		KG3DMeshSkyBoxDescStruct(const KG3DDWGC_Key_SkyBox& OtherStruct);///<DW_Frame>
		inline bool operator==( const KG3DMeshSkyBoxDescStruct &cStruct );
		inline bool operator!=( const KG3DMeshSkyBoxDescStruct &cStruct );
		void ToKey(KG3DDWGC_Key_SkyBox& OtherStruct)const;///<DW_Frame>
		void ToBaseStruct(KG3DMESHSKYBOX_DESCSTRUCT* pOtherStruct);
		bool SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		bool LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		bool SetDefaultValue();
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class KG3DMeshSkyBox;
	
	class KG3DMeshSkyBox : public KG3DEnvEffect,/*public KG3DMesh,*/public KG3DMessageSingleReceiverSender, public IEKG3DMeshSkyBox
	{
				
	public:
		///<接口>
		virtual HRESULT	RequestModification( const KG3DMESHSKYBOX_DESCSTRUCT* pSKStruct );
		virtual HRESULT GetDescStruct( KG3DMESHSKYBOX_DESCSTRUCT* pSKStruct );
		///</接口>
		///<KG3DEnvEffect>
		virtual HRESULT Render(const KG3DEnvEffRenderContext& renderContext);
		virtual HRESULT FrameMove();
		virtual HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		virtual HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection){return E_NOTIMPL;}
		virtual HRESULT ToSolidInterface(LPVOID* pEffectInterface);
		///</KG3DEnvEffect>
		HRESULT	RequestModification( const KG3DMeshSkyBoxDescStruct* pSKStruct );//更高贴图的描述结构，要求贴图重绘，外部改变结构的入口
		//HRESULT RequestSubTextureRedraw(const KG3DMeshSkyBoxDescStruct* pSKStruct);	
		const KG3DMeshSkyBoxDescStruct& GetDescStruct( void ){return m_DescStruct;}

		static HRESULT CreateInstance(IIniFile* pFile, LPCTSTR strSection, LPVOID* ppInstance);	//约定，当文件名为NULL的时候，创建默认的

		///<动态天气>
	
		///<DW_Controller>
	public:
		typedef KG3DDWGC_Key_SkyBox			 TypeKey;		
	public:
		HRESULT DWGCRequestPopFrontState(float fCurBlendPercent = 0.0f);
		HRESULT DWGCRequestSubStateChange(const KG3DDWGC_KeyBase* pRetKey);
		HRESULT DWGCGetCurStateToKey(KG3DDWGC_KeyBase* pRetKey);		
		HRESULT DWGCEnableSubState(BOOL bEnable);
		HRESULT DWGCSetPercent(float fPercent);
		HRESULT DWGCRequestModification(const KG3DDWGC_KeyBase* pKey);
		///</DW_Controller>
	private:
		KG3DDWGC_Key_SkyBox	m_SubKey;
		LPDIRECT3DTEXTURE9	m_pSkyColorSubMap;
		bool			m_bIsSubTextureCreated;	
		D3DCOLOR		m_d3dc_TextureBlendFactor;

		BOOL			m_bUseSubTexture;
		float			m_fBlendPercent;

		///</动态天气>
	private:
		struct SkyColorPosStruct;
		//static LPD3DXMESH ProcessMesh(LPD3DXMESH pMeshIn, D3DXVECTOR3 vecCenter, FLOAT fRadius);
		//HRESULT CustomInit();
		//HRESULT SetSkyTexture(char pszName[] = NULL );
		HRESULT	ModifyTextureByInterp(const KG3DMeshSkyBoxDescStruct& DescStruct, LPDIRECT3DTEXTURE9 pTex);
		//HRESULT	CreateSubTextureByInterp(const KG3DMeshSkyBoxDescStruct& SKSubStruct);
		VOID	TranslateStructToPos(const KG3DMeshSkyBoxDescStruct& cStruct, SkyColorPosStruct& cPosStruct, int nTextureWidth, int nTextureHeight);
	protected:///封闭拷贝，形成Singleton
		KG3DMeshSkyBox(KG3DMeshSkyBox& OtherSkyBox );
		KG3DMeshSkyBox& operator=( KG3DMeshSkyBox& OtherSkyBox );
	public:	
		KG3DMeshSkyBox(void);
		~KG3DMeshSkyBox(void);

	private:		
		enum
		{
			EM_DEFAULT_RADIUS = 5000000,
		};
		struct SkyColorPosStruct	//用于转换成边界值
		{
			int	nTotalHeight;
			int n0TopPos;
			int n1TopMidBlendBoundPos;
			int n2MidTopPos;
			int n3MidBotmPos;
			int n4MidBotmBlendBoundPos;
			int n5BotmPos;
			SkyColorPosStruct()
			{
				ZeroMemory(this, sizeof(SkyColorPosStruct));
			}
		};
	public:
		struct RenderState
		{
			GraphicsStruct::SamplerStateAddressVClamp	m_AddressVCamp;
			GraphicsStruct::MaterialWhite	m_MaterialWhite;
			RenderState();
		};
	private:
		LPD3DXMESH	m_pD3DXMesh;
		LPDIRECT3DTEXTURE9	m_pSkyColorMap;
		//unsigned	m_uTextureHeight;
		//unsigned	m_uTextureWidth;
		//bool		m_bIsTextureCreated; //贴图只能创建一次
		//bool		m_bIsInitialized;
		KG3DMeshSkyBoxDescStruct	m_DescStruct;
	};
};
#endif //_INCLUDE_KG3DMESHSKYBOX_H_
