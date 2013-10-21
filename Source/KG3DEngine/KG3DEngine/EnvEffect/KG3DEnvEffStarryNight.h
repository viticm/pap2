////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffStarryNight.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-2-26 10:10:01
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DENVEFFSTARRYNIGHT_H_
#define _INCLUDE_KG3DENVEFFSTARRYNIGHT_H_
#include "KG3DEnvEffect.h"
#include "IEKG3DEnvEffStarryNight.h"
class KG3DTexture;
////////////////////////////////////////////////////////////////////////////////
namespace KG3D_ENVIRONMENT_EFFECT
{
	struct KG3DDWGC_Key_StarryNight : public KG3DDWGC_KeyBase 
	{
		enum{DWGCKEY_TYPE = KG3DTYPE_ENVEFF_STARRY_NIGHT,};
		virtual DWORD GetType()const{return DWGCKEY_TYPE;}

		virtual bool CalculateInterp(
			const KG3DDWGC_KeyBase& BaseKeyBefore, 
			const KG3DDWGC_KeyBase& BaseKeyAfter,
			float fPercent);

		bool SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		bool LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		void DebugPrint(std::ostringstream& OSS);
	};

	class KG3DEnvEffStarryNight : public KG3DEnvEffect, public IEKG3DEnvEffStarryNight
	{
		struct DescHandler : public IEKG3DEnvEffDescHandler
		{
			enum{DWGC_TYPE = KG3DTYPE_ENVEFF_STARRY_NIGHT};
			typedef IEKG3DEnvEffStarryNightDesc TypeDesc;

			virtual HRESULT GetDefDesc(IEKG3DEnvEffDescBase* pDesc, size_t uSizeOfDesc);			
			virtual HRESULT GetDescParamTable(TypeParamDescEnumer** pEnumer, size_t* puKeyParamCount);
		};
	public:
		virtual HRESULT Render(const KG3DEnvEffRenderContext& renderContext);
		virtual HRESULT	FrameMove();
		virtual HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		virtual HRESULT ToSolidInterface(LPVOID* pEffectInterface);

		virtual HRESULT GetDescHandler(IEKG3DEnvEffDescHandler** pDescHandler);

		static VOID SetDefaultDesc(IEKG3DEnvEffStarryNightDesc& desc);
		KG3DEnvEffect* Build(LPCTSTR lpFile, DWORD_PTR wParam, DWORD_PTR lParam);
		KG3DEnvEffStarryNight();
	private:
		virtual HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection);	

		LPDIRECT3DVERTEXBUFFER9	m_pVB;
		KG3DTexture* m_pTexture;

		DescHandler m_descHandler;
	};
};

#endif //_INCLUDE_KG3DENVEFFSTARRYNIGHT_H_
