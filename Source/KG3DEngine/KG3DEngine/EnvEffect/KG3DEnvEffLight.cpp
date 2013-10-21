////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffLight.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-11-22 11:22:21
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DEnvEffLight.h"
#include "KG3DEnvEffCommonDef.h"
#include "KGINIFileHelpersEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace KG3D_ENVIRONMENT_EFFECT
{
	struct KG3DEnvEffLightTester
	{
		KG3DEnvEffLightTester(KG3DEnvEffLight& refEffect);
	};
	KG3DEnvEffLight::KG3DEnvEffLight()
		:KG3DEnvEffect(0, KG3DTYPE_ENVEFF_LIGHT)
		,m_fPercent(0)
		,m_State(0)
	{
		KG3DEnvEffLightDescStruct tempDesc;
		tempDesc.SetDefault();
		this->SetDescPrivate(tempDesc);
		//m_DescStruct.SetDefault();
		//m_DescStruct.ToKey(m_KeyResult);

#if defined(_DEBUG) | defined(DEBUG)
		static KG3DEnvEffLightTester LightTester(*this);
#endif
	}
	static KG3DLightParam s_lightParamSun;
	static KG3DLightParam s_lightParamPlayer;

	HRESULT KG3DEnvEffLight::LoadDefaultSunAndPlayerLight(IIniFile* pDefaultIni)
	{
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(NULL != pDefaultIni);
		{
			KG3DEnvEffLightDescStruct descStructSun;
			PCOMERROR_LOG(descStructSun.LoadMapSetting(pDefaultIni, _T("Light_0")));

			KG3DEnvEffLightDescStruct descStructPlayer;
			PCOMERROR_LOG(descStructPlayer.LoadMapSetting(pDefaultIni, _T("Light_1")));

			descStructSun.ToLightParam(s_lightParamSun);
			descStructPlayer.ToLightParam(s_lightParamPlayer);

			return S_OK;
		}
Exit0:
		return E_FAIL;
	}
	
	const KG3DLightParam& KG3DEnvEffLight::GetDefaultSunParam()
	{
		return s_lightParamSun;
	}
	const KG3DLightParam& KG3DEnvEffLight::GetDefaultPlayerParam()
	{
		return s_lightParamPlayer;
	}
	KG3DEnvEffLight::~KG3DEnvEffLight(void)
	{
	}

	HRESULT KG3DEnvEffLight::Render(const KG3DEnvEffRenderContext& renderContext)
	{
		return S_OK;
	}

	HRESULT KG3DEnvEffLight::FrameMove()
	{
		return S_OK;
	}

	HRESULT KG3DEnvEffLight::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		return m_DescStruct.SaveMapSetting(pIniFile, pSection);
	}

	HRESULT KG3DEnvEffLight::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		if (! pIniFile)
		{
			return this->RequestModification(NULL);
		}
		KG3DEnvEffLightDescStruct TempStruct;
		HRESULT hr = TempStruct.LoadMapSetting(pIniFile, pSection);
		KG_COM_PROCESS_ERROR(hr);
		TempStruct.m_dwModificationOption = KG3DENVEFFLIGHT_MDO_ALL;//包含了Enable
		return RequestModification(&TempStruct);
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffLight::RequestModification( const KG3DENVEFFLIGHT_DESCSTRUCT* pDescStruct )
	{
		if(! pDescStruct)
			return E_FAIL;
		KG3DEnvEffLightDescStruct TempStruct(*pDescStruct);
		return RequestModification(&TempStruct);
	}

	/*HRESULT KG3DEnvEffLight::RequestModification(const KG3DEnvEffLightDescStruct0* pDescOld )
	{
		if (! pDescOld)
		{
			return RequestModification(NULL);
		}
		KG3DEnvEffLightDescStruct TempStruct;
		TempStruct.SynFrom(*pDescOld);
		TempStruct.m_dwModificationOption = KG3DENVEFFLIGHT_MDO_ALL;
		
		return RequestModification(&TempStruct);
	}*/

	HRESULT KG3DEnvEffLight::RequestModification(const KG3DEnvEffLightDescStruct* pDesc )
	{
		if (! pDesc)
		{
			/*m_DescStruct.SetDefault();
			m_DescStruct.ToKey(m_KeyResult);*/

			KG3DEnvEffLightDescStruct tempDesc;
			tempDesc.SetDefault();
			this->SetDescPrivate(tempDesc);

			goto Exit1;
		}
		KG_PROCESS_ERROR(pDesc && pDesc != &m_DescStruct);
		{
			KG3DEnvEffLightDescStruct descTemp = m_DescStruct;

			//小心KG3DENVEFFLIGHT_MDO_ENABLELIGHT
			DWORD dwModification = pDesc->m_dwModificationOption;
			
			descTemp.SetLightEnable(pDesc->IsLightEnable());

			if (dwModification & KG3DENVEFFLIGHT_MDO_D3DLIGHT_DIFFUSE)
				descTemp.m_D3DLight.Diffuse = pDesc->m_D3DLight.Diffuse;
			if(dwModification & KG3DENVEFFLIGHT_MDO_D3DLIGHT_AMBIENT)
				descTemp.m_D3DLight.Ambient = pDesc->m_D3DLight.Ambient;
			if(dwModification & KG3DENVEFFLIGHT_MDO_D3DLIGHT_SPECULAR)
				descTemp.m_D3DLight.Specular = pDesc->m_D3DLight.Specular;
			if(dwModification & KG3DENVEFFLIGHT_MDO_D3DLIGHT_DIRECTION)
			{
				descTemp.m_D3DLight.Direction = pDesc->m_D3DLight.Direction;
				D3DXVECTOR3* pDir = static_cast<D3DXVECTOR3*>(&descTemp.m_D3DLight.Direction);
				D3DXVec3Normalize(pDir,pDir);
			}
			if (dwModification & KG3DENVEFFLIGHT_MDO_D3DLIGHT_POSITION)
			{
				descTemp.m_D3DLight.Position = pDesc->m_D3DLight.Position;
			}
			if(dwModification & KG3DENVEFFLIGHT_MDO_D3DLIGHT_OTHER)
			{
				//D3DLIGHTTYPE    Type;            /* Type of light source */
				//D3DCOLORVALUE   Diffuse;         /* Diffuse color of light */
				//D3DCOLORVALUE   Specular;        /* Specular color of light */
				//D3DCOLORVALUE   Ambient;         /* Ambient color of light */
				//D3DVECTOR       Position;         /* Position in world space */
				//D3DVECTOR       Direction;        /* Direction in world space */
				//float           Range;            /* Cutoff range */
				//float           Falloff;          /* Falloff */
				//float           Attenuation0;     /* Constant attenuation */
				//float           Attenuation1;     /* Linear attenuation */
				//float           Attenuation2;     /* Quadratic attenuation */
				//float           Theta;            /* Inner angle of spotlight cone */
				//float           Phi;              /* Outer angle of spotlight cone */
			
				//Type不改，永远是DIRECTIONAL型
				
				descTemp.m_D3DLight.Range = pDesc->m_D3DLight.Range;
				descTemp.m_D3DLight.Falloff = pDesc->m_D3DLight.Falloff;
				descTemp.m_D3DLight.Attenuation0 = pDesc->m_D3DLight.Attenuation0;
				descTemp.m_D3DLight.Attenuation1 = pDesc->m_D3DLight.Attenuation1;
				descTemp.m_D3DLight.Attenuation2 = pDesc->m_D3DLight.Attenuation2;
				descTemp.m_D3DLight.Theta = pDesc->m_D3DLight.Theta;
				descTemp.m_D3DLight.Phi = pDesc->m_D3DLight.Phi;
			
			}
			if (dwModification & KG3DENVEFFLIGHT_MDO_SKYLIGHT)
				descTemp.m_ColorArgs[0] = pDesc->m_ColorArgs[0];
			if (dwModification & KG3DENVEFFLIGHT_MDO_COMPLIGHT)
				descTemp.m_ColorArgs[1] = pDesc->m_ColorArgs[1];
			if (dwModification & KG3DENVEFFLIGHT_MDO_SCENEAMBIENT)
				descTemp.m_ColorArgs[2] = pDesc->m_ColorArgs[2];
			if (dwModification & KG3DENVEFFLIGHT_MDO_COLORARRAY_3)
				descTemp.m_ColorArgs[3] = pDesc->m_ColorArgs[3];

			//if (! this->IsSubStateEnable())//动态天气进行中
			//{
			//	m_DescStruct.ToKey(m_KeyResult);
			//}

			this->SetDescPrivate(descTemp);
		}
Exit1:		
		return S_OK;
Exit0:
		return E_FAIL;
	}
	HRESULT KG3DEnvEffLight::GetStruct( KG3DENVEFFLIGHT_DESCSTRUCT* pDescStruct )
	{
		if (pDescStruct)
		{
			m_DescStruct.ToBaseStruct(*pDescStruct);
			return S_OK;
		}
		return E_FAIL;
	}

	HRESULT KG3DEnvEffLight::ToSolidInterface( LPVOID* pEffectInterface )
	{
		if (! pEffectInterface)
			return E_FAIL;

		IEKG3DEnvEffLight* pIE = this;
		*pEffectInterface = pIE;
		return S_OK;
	}

//	HRESULT KG3DEnvEffLightDescStruct0::SaveToFile( LPCTSTR tczName )
//	{
//		//HRESULT hr = E_FAIL;
//		FILE* fpFile = NULL;
//		KG_PROCESS_ERROR(tczName);
//		{
//			fpFile = fopen(tczName, "wb");
//			KG_PROCESS_ERROR(fpFile);
//
//			
//			typedef KG3DEnvEffLightDescStruct0::Header TypeFileHeader;
//			TypeFileHeader FileHeader;
//			size_t Size = 0;
//			KG_PROCESS_ERROR(fpFile);
//
//			Size = fwrite(&FileHeader, sizeof(FileHeader), 1, fpFile);
//			KG_PROCESS_ERROR(Size == 1);
//
//			Size = fwrite(&m_D3DLight, sizeof(D3DLIGHT9), 1, fpFile);
//			Size = fwrite(&m_bVolumnLight, sizeof(BOOL), 1, fpFile);
//			Size = fwrite(&m_VolumnLightSzie, sizeof(D3DXVECTOR3), 1, fpFile);
//			Size = fwrite(&m_bEnabled, sizeof(BOOL), 1, fpFile);
//			Size = fwrite(&m_bCastShadow, sizeof(BOOL), 1, fpFile);
//
//			Size = fwrite(&m_dwSceneAmbient, sizeof(DWORD), 1, fpFile);
//			KG_PROCESS_ERROR(Size == 1);
//
//			return S_OK;
//		}
//Exit0:
//		if (fpFile)
//		{
//			fclose(fpFile);
//			fpFile = NULL;
//		}
//		return E_FAIL;
//	}

//	HRESULT KG3DEnvEffLightDescStruct0::LoadFromFile(LPCTSTR tczName)
//	{
//        // TODO: 性能很低，建议使用整块结构，一次读取
//
//		enum
//		{
//			version0_size = sizeof(D3DLIGHT9),
//			version1_size = sizeof(KG3DEnvEffLight),
//			version1_checked_size = sizeof(D3DLIGHT9) + sizeof(BOOL) * 3 + sizeof(D3DXVECTOR3),
//			version2_size = version1_size + sizeof(KG3DEnvEffLightDescStruct0::Header) + sizeof(DWORD),
//			version2_checked_size = version2_size - (version1_size- version1_checked_size),
//		};
//
//		typedef Header TypeFileHeader;
//
//        int nRetCode = false;
//        HRESULT hr = E_FAIL;
//		unsigned long lSize = 0;
//		unsigned long Size = 0;
//		TypeFileHeader FileHeader;
//		IFile* pFile = NULL;
//
//		KG_PROCESS_ERROR(tczName);
//
//		pFile = g_OpenFile(tczName);
//		KG_PROCESS_ERROR(pFile);
//
//        lSize = pFile->Size();
//		switch(lSize)
//		{
//		case version0_size:
//			{
//				//Old version
//				Size = pFile->Read(&m_D3DLight, sizeof(D3DLIGHT9));
//				KG_PROCESS_ERROR(Size == 1);
//				break;
//			}
//		case version1_size:
//			{
//                nRetCode = pFile->Seek(4, SEEK_CUR);
//                KGLOG_PROCESS_ERROR(nRetCode != -1);
//
//				Size = pFile->Read(&m_D3DLight, sizeof(D3DLIGHT9));
//                KGLOG_PROCESS_ERROR(Size == sizeof(D3DLIGHT9));
//
//                Size = pFile->Read(&m_bVolumnLight, sizeof(BOOL));
//                KGLOG_PROCESS_ERROR(Size == sizeof(BOOL));
//
//				Size = pFile->Read(&m_VolumnLightSzie, sizeof(D3DXVECTOR3));
//                KGLOG_PROCESS_ERROR(Size == sizeof(D3DXVECTOR3));
//
//				Size = pFile->Read(&m_bEnabled, sizeof(BOOL));
//                KGLOG_PROCESS_ERROR(Size == sizeof(BOOL));
//
//				Size = pFile->Read(&m_bCastShadow, sizeof(BOOL));
//                KGLOG_PROCESS_ERROR(Size == sizeof(BOOL));
//				break;
//			}
//		case version1_checked_size:
//			{
//				Size = pFile->Read(&m_D3DLight, sizeof(D3DLIGHT9));
//                KGLOG_PROCESS_ERROR(Size == sizeof(D3DLIGHT9));
//
//				Size = pFile->Read(&m_bVolumnLight, sizeof(BOOL));
//                KGLOG_PROCESS_ERROR(Size == sizeof(BOOL));
//
//				Size = pFile->Read(&m_VolumnLightSzie, sizeof(D3DXVECTOR3));
//                KGLOG_PROCESS_ERROR(Size == sizeof(D3DXVECTOR3));
//
//				Size = pFile->Read(&m_bEnabled, sizeof(BOOL));
//                KGLOG_PROCESS_ERROR(Size == sizeof(BOOL));
//
//				Size = pFile->Read(&m_bCastShadow, sizeof(BOOL));
//				KG_PROCESS_ERROR(Size == sizeof(BOOL));
//				break;
//			}
//		case version2_size:
//			{
//				Size = pFile->Read(&FileHeader, sizeof(FileHeader));	
//				KGLOG_PROCESS_ERROR(Size == sizeof(FileHeader));
//
//                nRetCode = pFile->Seek(4, SEEK_CUR);
//                KGLOG_PROCESS_ERROR(nRetCode != -1);
//
//				Size = pFile->Read(&m_D3DLight, sizeof(D3DLIGHT9));
//                KGLOG_PROCESS_ERROR(Size == sizeof(D3DLIGHT9));
//
//				Size = pFile->Read(&m_bVolumnLight, sizeof(BOOL));
//                KGLOG_PROCESS_ERROR(Size == sizeof(BOOL));
//
//				Size = pFile->Read(&m_VolumnLightSzie, sizeof(D3DXVECTOR3));
//                KGLOG_PROCESS_ERROR(Size == sizeof(D3DXVECTOR3));
//
//				Size = pFile->Read(&m_bEnabled, sizeof(BOOL));
//                KGLOG_PROCESS_ERROR(Size == sizeof(BOOL));
//
//				Size = pFile->Read(&m_bCastShadow, sizeof(BOOL));
//				KGLOG_PROCESS_ERROR(Size == sizeof(BOOL));
//
//				Size = pFile->Read(&m_dwSceneAmbient, sizeof(DWORD));
//				KGLOG_PROCESS_ERROR(Size == sizeof(DWORD));
//				break;
//			}
//		case version2_checked_size:
//			{
//				Size = pFile->Read(&FileHeader, sizeof(FileHeader));	
//				KGLOG_PROCESS_ERROR(Size == sizeof(FileHeader));
//
//				Size = pFile->Read(&m_D3DLight, sizeof(D3DLIGHT9));
//                KGLOG_PROCESS_ERROR(Size == sizeof(D3DLIGHT9));
//
//				Size = pFile->Read(&m_bVolumnLight, sizeof(BOOL));
//                KGLOG_PROCESS_ERROR(Size == sizeof(BOOL));
//
//				Size = pFile->Read(&m_VolumnLightSzie, sizeof(D3DXVECTOR3));
//                KGLOG_PROCESS_ERROR(Size == sizeof(D3DXVECTOR3));
//
//				Size = pFile->Read(&m_bEnabled, sizeof(BOOL));
//                KGLOG_PROCESS_ERROR(Size == sizeof(BOOL));
//
//				Size = pFile->Read(&m_bCastShadow, sizeof(BOOL));
//				KGLOG_PROCESS_ERROR(Size == sizeof(BOOL));
//
//				Size = pFile->Read(&m_dwSceneAmbient, sizeof(DWORD));
//				KGLOG_PROCESS_ERROR(Size == sizeof(DWORD));
//				break;
//			}
//		}
//
//        hr = S_OK;
//Exit0:
//        KG_COM_RELEASE(pFile);
//		return hr;
//	}

	//void KG3DEnvEffLightDescStruct0::SetDefault()
	//{
	//	//小心这些设置，在有的Scene中需要这种初始化
	//	{
	//		D3DCOLORVALUE SkyLightDiffuse = {0.5f, 0.5f, 0.5f, 0.5f};
	//		m_SkyLightInfo.Diffuse = SkyLightDiffuse;
	//		m_SkyLightInfo.fPower = 1.0f;
	//	}
	//	{
	//		D3DCOLORVALUE ComLightDiffuse = {0.5f, 0.5f, 0.5f, 0.5f};
	//		m_ComLightInfo.Diffuse = ComLightDiffuse;
	//		m_ComLightInfo.fPower = 1.0f;
	//	}

	//	ZeroMemory(&m_D3DLight.Ambient, sizeof(D3DCOLORVALUE));
	//	{
	//		D3DCOLORVALUE Specular = {1.0f,1.0f,1.0f,1.0f};
	//		m_D3DLight.Specular = Specular;
	//	}
	//	{
	//		D3DCOLORVALUE Diffuse = {0.5f, 0.5f, 0.5f, 1.0f};
	//		m_D3DLight.Diffuse = Diffuse;
	//	}
	//	
	//	m_D3DLight.Range = 1000.0f;
	//	m_D3DLight.Ambient.a = 1.0f;
	//	m_D3DLight.Position.x = 0.0f;
	//	m_D3DLight.Position.y = 400.0f;
	//	m_D3DLight.Position.z = 0.0f;

	//	D3DXVECTOR3 vec3Dir(-100.0f, -100.0f, -100.0f);
	//	D3DXVec3Normalize(&vec3Dir, &vec3Dir);
	//	m_D3DLight.Direction.x = vec3Dir.x;
	//	m_D3DLight.Direction.y = vec3Dir.y;
	//	m_D3DLight.Direction.z = vec3Dir.z;


	//	m_D3DLight.Attenuation1 = 0.001f;
	//	m_D3DLight.Type = D3DLIGHT_DIRECTIONAL;

	//	m_bVolumnLight = FALSE;//是否是体积光
	//	m_VolumnLightSzie = D3DXVECTOR3(100,100,100);//体积光源的尺寸
	//	m_bCastShadow = FALSE;//是否产生阴影

	//	m_bEnabled = TRUE;

	//	m_dwSceneAmbient = 0xff808080;

	//}

	HRESULT KG3DEnvEffLight::Apply(DWORD LightIndex)
	{
		KGLOG_PROCESS_ERROR(LightIndex >= 0 && LightIndex < 8);

		if (this->IsLightEnable())
		{
			g_pd3dDevice->LightEnable(LightIndex, TRUE);
			
			if (this->IsSubStateEnable())//动态天气进行中
			{
				KG3DEnvEffLightDescStruct TempStruct = m_DescStruct;
				TempStruct.SynWithKey(m_KeyResult);
				g_pd3dDevice->SetLight(LightIndex, &TempStruct.m_D3DLight);
			}
			else
			{
				g_pd3dDevice->SetLight(LightIndex, &m_DescStruct.m_D3DLight);
			}
		}
		else
		{
			g_pd3dDevice->LightEnable(LightIndex, FALSE);
		}
		return S_OK;
Exit0:
		return E_FAIL;
	}
	HRESULT KG3DEnvEffLight::Copy( IEKG3DEnvEffLight* pOtherLight )
	{
		KG3DEnvEffLight* pLight = dynamic_cast<KG3DEnvEffLight*>(pOtherLight);
		KG_PROCESS_ERROR(pLight);
		KG_PROCESS_ERROR(! IsSubStateEnable());
		KG_PROCESS_ERROR(this->CopyData(*pLight));
		return S_OK;
Exit0:
		return E_FAIL;
	}

	BOOL KG3DEnvEffLight::CopyData( const KG3DEnvEffLight& Other )
	{
		/*this->m_DescStruct = Other.m_DescStruct;
		m_DescStruct.ToKey(m_KeyResult);*/
		this->SetDescPrivate(Other.m_DescStruct);
		return TRUE;
	}

	HRESULT KG3DEnvEffLight::DWGCRequestPopFrontState(float fCurBlendPercent/* = 0.0f*/)
	{
		if (fCurBlendPercent > 0 && fCurBlendPercent < 1)
		{
			KG3DDWGC_Key_Light MidKey, BeforeKey;
			m_DescStruct.ToKey(BeforeKey);
			MidKey.CalculateInterp(BeforeKey, m_SubKey, fCurBlendPercent);
			//m_DescStruct.SynWithKey(MidKey);

			KG3DEnvEffLightDescStruct tempStruct = m_DescStruct;
			tempStruct.SynWithKey(MidKey);

			//m_DescStruct.ToKey(m_KeyResult);

			this->SetDescPrivate(tempStruct);
		}
		else
		{
			/*m_DescStruct.SynWithKey(m_SubKey);
			m_DescStruct.ToKey(m_KeyResult);*/

			KG3DEnvEffLightDescStruct tempStruct = m_DescStruct;
			tempStruct.SynWithKey(m_SubKey);

			this->SetDescPrivate(tempStruct);
		}
		return S_OK;
	}

	HRESULT KG3DEnvEffLight::DWGCRequestSubStateChange( const KG3DDWGC_KeyBase* pRetKey)
	{
		const KG3DDWGC_Key_Light* pLightKey = dynamic_cast<const KG3DDWGC_Key_Light*>(pRetKey);
		KG_PROCESS_ERROR(pLightKey);
		m_SubKey = *pLightKey;
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffLight::DWGCGetCurStateToKey( KG3DDWGC_KeyBase* pRetKey )
	{
		KG3DDWGC_Key_Light* pLightKey = dynamic_cast<KG3DDWGC_Key_Light*>(pRetKey);
		KG_PROCESS_ERROR(pLightKey);
		m_DescStruct.ToKey(*pLightKey);
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffLight::DWGCEnableSubState( BOOL bEnable )
	{
		this->EnableSubState(bEnable);
		if(bEnable)
			m_DescStruct.ToKey(m_KeyResult);
		return  S_OK;
	}

	HRESULT KG3DEnvEffLight::DWGCRequestModification( const KG3DDWGC_KeyBase* pKey)
	{
		const KG3DDWGC_Key_Light* pLightKey = dynamic_cast<const KG3DDWGC_Key_Light*>(pKey);
		KG_PROCESS_ERROR(pLightKey);
		{
			KG3DEnvEffLightDescStruct TempStruct;
			TempStruct.SynWithKey(*pLightKey);
			TempStruct.m_dwModificationOption = KG3DENVEFFLIGHT_MDO_ALL & (~KG3DENVEFFLIGHT_MDO_D3DLIGHT_OTHER) ;
			TempStruct.SetLightEnable(this->IsLightEnable());
			return RequestModification(&TempStruct);
		}
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffLight::DWGCSetPercent( float fPercent )
	{
		m_fPercent = fPercent;
		MT_LIMIT_TO_RANGE(m_fPercent, (1+FLT_EPSILON), 0-FLT_EPSILON);
		KG3DDWGC_Key_Light TempKey;
		m_DescStruct.ToKey(TempKey);

		KG3DDWGC_Key_Light keyResult;
		HRESULT hr = keyResult.CalculateInterp(TempKey, m_SubKey, fPercent) ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			//下面这个转换的倒腾有点粗糙，不过等整个帧变换架构完善之后，代码会好很多的，所以不用管它
			KG3DEnvEffLightDescStruct tempLight;
			tempLight.SynWithKey(keyResult);
			tempLight.ToLightParam(m_lightParam);
		}

		//HRESULT hr = m_KeyResult.CalculateInterp(TempKey, m_SubKey, fPercent);
		return hr;
	}

	BOOL KG3DEnvEffLight::SetDiffuse( D3DCOLORVALUE Diffuse )
	{
		KG3DEnvEffLightDescStruct TempStruct;
		TempStruct.m_D3DLight.Diffuse = Diffuse;
		TempStruct.m_dwModificationOption = KG3DENVEFFLIGHT_MDO_D3DLIGHT_DIFFUSE;
		HRESULT hr = RequestModification(&TempStruct);
		return SUCCEEDED(hr);
	}

	const KG3DLightParam& KG3DEnvEffLight::GetParam()const
	{
		/*KG3DLightParam paramLight;
		paramLight.dir = GetDirection();
		paramLight.diffuse = Color2Value(GetDiffuse());
		paramLight.specular = Color2Value(GetSpecular());
		paramLight.sceneAmbient = Color2Value(GetSceneAmbient());
		paramLight.sky_light = Color2Value(GetSkyLightDiffuse());
		paramLight.com_light = Color2Value(GetComLightDiffuse());
		
		return paramLight;*/
		return m_lightParam;
	}

	HRESULT KG3DEnvEffLight::SetParam( const KG3DLightParam& lightParam )
	{
		KG3DENVEFFLIGHT_DESCSTRUCT desc;
		desc.m_D3DLight.Diffuse = lightParam.diffuse;
		desc.m_D3DLight.Specular = lightParam.specular;
		desc.m_D3DLight.Direction = lightParam.dir;

		desc.m_ColorArgs[KG3DENVEFFLIGHT_DESCSTRUCT::color_arg_scene_ambient] 
				= Value2Color(lightParam.sceneAmbient);

		desc.m_ColorArgs[KG3DENVEFFLIGHT_DESCSTRUCT::color_arg_sky_light] 
				= Value2Color(lightParam.sky_light);

		desc.m_ColorArgs[KG3DENVEFFLIGHT_DESCSTRUCT::color_arg_comp_light] 
				= Value2Color(lightParam.com_light);


		desc.m_dwModificationOption = KG3DENVEFFLIGHT_MDO_D3DLIGHT_DIFFUSE
			| KG3DENVEFFLIGHT_MDO_D3DLIGHT_AMBIENT
			| KG3DENVEFFLIGHT_MDO_D3DLIGHT_SPECULAR
			| KG3DENVEFFLIGHT_MDO_D3DLIGHT_DIRECTION
			| KG3DENVEFFLIGHT_MDO_SCENEAMBIENT
			| KG3DENVEFFLIGHT_MDO_COMPLIGHT
			| KG3DENVEFFLIGHT_MDO_SKYLIGHT;

		desc.SetLightEnable(TRUE);

		return RequestModification(&desc);
	}

	KG3DLightParam KG3DEnvEffLight::GetDefaultLightParam()
	{
		static bool s_IsLightParamInited = false;
		static KG3DLightParam s_LightParam;
		if (! s_IsLightParamInited)
		{
			s_IsLightParamInited = true;

			typedef KG3DENVEFFLIGHT_DESCSTRUCT TypeDesc;
			KG3DEnvEffLightDescStruct descDefault;

			s_LightParam.dir = descDefault.m_D3DLight.Direction;
			s_LightParam.diffuse = descDefault.m_D3DLight.Diffuse;
			s_LightParam.specular = descDefault.m_D3DLight.Specular;
			s_LightParam.sceneAmbient = Color2Value(descDefault.m_ColorArgs[TypeDesc::color_arg_scene_ambient]);
			s_LightParam.sky_light = Color2Value(descDefault.m_ColorArgs[TypeDesc::color_arg_sky_light]);
			s_LightParam.com_light = Color2Value(descDefault.m_ColorArgs[TypeDesc::color_arg_comp_light]);
		}
		return s_LightParam;
	}

	VOID KG3DEnvEffLight::SetDescPrivate( const KG3DEnvEffLightDescStruct& descIn )
	{
		const_cast<KG3DEnvEffLightDescStruct&>(m_DescStruct) = descIn;
		if (! this->IsSubStateEnable())//动态天气进行中
		{
			m_DescStruct.ToKey(m_KeyResult);
			m_DescStruct.ToLightParam(m_lightParam);
		}
	}

	
	void KG3DEnvEffLightDescStruct::SetDefault()
	{
		struct TypeEnvEffLightDescStructInitializer
		{
			KG3DENVEFFLIGHT_DESCSTRUCT m_DescStruct;
			TypeEnvEffLightDescStructInitializer()
			{
				//小心这些设置，在有的Scene中需要这种初始化
				ZeroMemory(this, sizeof(KG3DEnvEffLightDescStruct));
				D3DCOLOR HalfColor = D3DCOLOR_ARGB(127,127,127,127);
				{
					m_DescStruct.m_ColorArgs[0] = HalfColor;//SkyLight
					m_DescStruct.m_ColorArgs[1] = HalfColor;//ComLight
					m_DescStruct.m_ColorArgs[2] = HalfColor;//Ambient
					m_DescStruct.m_ColorArgs[3] = HalfColor;
				}
				{
					D3DCOLORVALUE Specular = {1.0f,1.0f,1.0f,1.0f};
					m_DescStruct.m_D3DLight.Specular = Specular;
				}
				{
					D3DCOLORVALUE Diffuse = {0.5f, 0.5f, 0.5f, 1.0f};
					m_DescStruct.m_D3DLight.Diffuse = Diffuse;
				}

				m_DescStruct.m_D3DLight.Range = 1000.0f;
				m_DescStruct.m_D3DLight.Ambient.a = 1.0f;
				m_DescStruct.m_D3DLight.Position.x = 0.0f;
				m_DescStruct.m_D3DLight.Position.y = 400.0f;
				m_DescStruct.m_D3DLight.Position.z = 0.0f;

				D3DXVECTOR3 vec3Dir(-100.0f, -100.0f, -100.0f);
				D3DXVec3Normalize(&vec3Dir, &vec3Dir);
				m_DescStruct.m_D3DLight.Direction.x = vec3Dir.x;
				m_DescStruct.m_D3DLight.Direction.y = vec3Dir.y;
				m_DescStruct.m_D3DLight.Direction.z = vec3Dir.z;

				m_DescStruct.m_D3DLight.Attenuation1 = 0.001f;
				m_DescStruct.m_D3DLight.Type = D3DLIGHT_DIRECTIONAL;
			}
		};
		//用了static形式的Initializer之后，这个函数调用多少次都不会执行上面那么多代码，只是个位拷贝而已
		static TypeEnvEffLightDescStructInitializer st_LightDescInitializer;//用于初始化
		KG_CUSTOM_HELPERS::TTypeCopy<KG3DENVEFFLIGHT_DESCSTRUCT>(this, &st_LightDescInitializer.m_DescStruct);
		this->SetLightEnable(TRUE);
	}

	

	KG3DEnvEffLightDescStruct::KG3DEnvEffLightDescStruct()
	{
		SetDefault();
	}

	HRESULT KG3DEnvEffLightDescStruct::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		bool bRet = false;
		KG_PROCESS_ERROR(pIniFile && pSection);
		using namespace KG_CUSTOM_HELPERS;
		bRet = LoadINIFileBigData<D3DLIGHT9>(pIniFile, pSection, _T("D3DLIGHT9"), &this->m_D3DLight);
		KG_PROCESS_ERROR(bRet);

		/*BOOL bEnable = TRUE;
		bRet = LoadINIFileData<BOOL>(pIniFile, pSection, _T("Enable"), TRUE, &bEnable);
		KG_PROCESS_ERROR(bRet);
		this->SetLightEnable(bEnable);*/

		this->SetLightEnable(TRUE);

		bRet = LoadINIFileData<D3DCOLOR>(pIniFile, pSection, _T("Color0"), 0, &this->m_ColorArgs[0]);
		KG_PROCESS_ERROR(bRet);
		bRet = LoadINIFileData<D3DCOLOR>(pIniFile, pSection, _T("Color1"), 0, &this->m_ColorArgs[1]);
		KG_PROCESS_ERROR(bRet);
		bRet = LoadINIFileData<D3DCOLOR>(pIniFile, pSection, _T("Color2"), 0, &this->m_ColorArgs[2]);
		KG_PROCESS_ERROR(bRet);
		bRet = LoadINIFileData<D3DCOLOR>(pIniFile, pSection, _T("Color3"), 0, &this->m_ColorArgs[3]);
		KG_PROCESS_ERROR(bRet);

		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffLightDescStruct::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )const
	{
		KG_PROCESS_ERROR(pIniFile && pSection);
		using namespace KG_CUSTOM_HELPERS;
		SaveINIFileBigData<D3DLIGHT9>(pIniFile, pSection, _T("D3DLIGHT9"), this->m_D3DLight);
		//SaveINIFileData<BOOL>(pIniFile, pSection, _T("Enable"), this->IsLightEnable());
		SaveINIFileData<D3DCOLOR>(pIniFile, pSection, _T("Color0"), this->m_ColorArgs[0]);
		SaveINIFileData<D3DCOLOR>(pIniFile, pSection, _T("Color1"), this->m_ColorArgs[1]);
		SaveINIFileData<D3DCOLOR>(pIniFile, pSection, _T("Color2"), this->m_ColorArgs[2]);
		SaveINIFileData<D3DCOLOR>(pIniFile, pSection, _T("Color3"), this->m_ColorArgs[3]);
		return S_OK;
Exit0:
		return E_FAIL;
	}

	//void KG3DEnvEffLightDescStruct::SynFrom( const KG3DEnvEffLightDescStruct0& Other )
	//{
	//	m_D3DLight = Other.m_D3DLight;
	//	this->SetLightEnable(Other.m_bEnabled);
	//	m_ColorArgs[0] = Value2Color(Other.m_SkyLightInfo.Diffuse);
	//	m_ColorArgs[1] = Value2Color(Other.m_ComLightInfo.Diffuse);
	//	m_ColorArgs[2] = Other.m_dwSceneAmbient;	//原来的SceneAmbient放到Args槽的第三个位置
	//}

	void KG3DEnvEffLightDescStruct::SynWithKey( const KG3DDWGC_Key_Light& Other)
	{
		KG3DDWGC_Key_Light& OtherKey = const_cast<KG3DDWGC_Key_Light&>(Other);
		m_D3DLight.Diffuse = Color2Value(OtherKey.GetDiffuse());
		m_D3DLight.Ambient = Color2Value(OtherKey.GetAmbient());
		m_D3DLight.Specular = Color2Value(OtherKey.GetSpecular());

		for(size_t i = 0; i < OtherKey.GetColorArgCount(); i++)
			m_ColorArgs[i] = OtherKey.GetColorArg(i);

		m_D3DLight.Direction = OtherKey.m_Direction;
	}

	void KG3DEnvEffLightDescStruct::ToKey( KG3DDWGC_Key_Light& Other )const
	{
		Other.GetDiffuse() = Value2Color(m_D3DLight.Diffuse);
		Other.GetAmbient() = Value2Color(m_D3DLight.Ambient);
		Other.GetSpecular() = Value2Color(m_D3DLight.Specular);

		for (size_t i = 0; i < color_arg_num; i++)
			Other.GetColorArg(i) = m_ColorArgs[i];
		
		Other.m_Direction = m_D3DLight.Direction;
	}

	void KG3DEnvEffLightDescStruct::ToLightParam( KG3DLightParam& lightParam ) const
	{
		lightParam.dir = m_D3DLight.Direction;
		lightParam.diffuse = m_D3DLight.Diffuse;
		lightParam.specular = m_D3DLight.Specular;
		lightParam.sceneAmbient = Color2Value(m_ColorArgs[color_arg_scene_ambient]);
		lightParam.sky_light = Color2Value(m_ColorArgs[color_arg_sky_light]);
		lightParam.com_light = Color2Value(m_ColorArgs[color_arg_comp_light]);
	}

	void KG3DEnvEffLightDescStruct::ToBaseStruct( KG3DENVEFFLIGHT_DESCSTRUCT& Other )const
	{
		KG_CUSTOM_HELPERS::TTypeCopy<KG3DENVEFFLIGHT_DESCSTRUCT>(&Other, this);
	}

	void KG3DEnvEffLightDescStruct::SynWithLightParam( const KG3DLightParam& lightParam )
	{
		m_D3DLight.Direction = lightParam.dir;
		m_D3DLight.Diffuse = lightParam.diffuse;
		m_D3DLight.Specular = lightParam.specular;
		m_ColorArgs[color_arg_scene_ambient] = Value2Color(lightParam.sceneAmbient);
		m_ColorArgs[color_arg_sky_light] = Value2Color(lightParam.sky_light);
		m_ColorArgs[color_arg_comp_light] = Value2Color(lightParam.com_light);
	}

	bool operator==(const KG3DEnvEffLightDescStruct& a, const KG3DEnvEffLightDescStruct& b)
	{
		_ASSERTE(0 == offsetof(KG3DEnvEffLightDescStruct, m_dwModificationOption));
		
		size_t uOp = sizeof(a.m_dwModificationOption);

		//不检查第一个成员ModificationOption，其它按字节比较
		return 0 == memcmp(((byte*)&a) + uOp, ((byte*)&b) + uOp, sizeof(KG3DEnvEffLightDescStruct) - uOp);
	}
	bool operator!=(const KG3DEnvEffLightDescStruct& a, const KG3DEnvEffLightDescStruct& b)
	{
		return !(a == b);
	}
	KG3DDWGC_Key_Light::KG3DDWGC_Key_Light()
	{
		m_dwModificationOption = 0;
		ZeroMemory(&m_Colors, sizeof(m_Colors));
		ZeroMemory(&m_Direction, sizeof(m_Direction));
	}

	bool KG3DDWGC_Key_Light::CalculateInterp( const KG3DDWGC_KeyBase& BaseKeyBefore, const KG3DDWGC_KeyBase& BaseKeyAfter, float fPercent)
	{
		//留意Key结构，下面的这种强转不一定可以的
		KG3DDWGC_Key_Light& KeyBefore = (KG3DDWGC_Key_Light&)BaseKeyBefore;
		KG3DDWGC_Key_Light& KeyAfter = (KG3DDWGC_Key_Light&)BaseKeyAfter;

		for (size_t i = 0; i < _countof(m_Colors); i++)
		{
			TwoColorBlend(KeyBefore.m_Colors[i], KeyAfter.m_Colors[i], m_Colors[i], fPercent, true);
		}

		MT_FLOAT_BLEND(KeyBefore.m_Direction.x, KeyAfter.m_Direction.x, m_Direction.x, fPercent);
		MT_FLOAT_BLEND(KeyBefore.m_Direction.y, KeyAfter.m_Direction.y, m_Direction.y, fPercent);
		MT_FLOAT_BLEND(KeyBefore.m_Direction.z, KeyAfter.m_Direction.z, m_Direction.z, fPercent);

		return true;
	}

	static LPCTSTR	stColorSaveKeyName[] = 
	{
		_T("Diffuse"),_T("Ambient"),_T("Specular"),_T("C0"),_T("C1"),_T("C2"),_T("C3"),
	};


	bool KG3DDWGC_Key_Light::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		KG_PROCESS_ERROR(pIniFile&&pSection);
		using namespace KG_CUSTOM_HELPERS;
		_ASSERTE(_countof(m_Colors) == _countof(stColorSaveKeyName));
		for (size_t i =0; i < _countof(m_Colors); i++)
		{
			SaveINIFileData<D3DCOLOR>(pIniFile, pSection, stColorSaveKeyName[i], m_Colors[i]);
		}
		SaveINIFileBigData<D3DVECTOR>(pIniFile, pSection, _T("Direction"), m_Direction);
		return true;
Exit0:
		return false;
	}

	bool KG3DDWGC_Key_Light::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		KG_PROCESS_ERROR(pIniFile&&pSection);
		using namespace KG_CUSTOM_HELPERS;
		_ASSERTE(_countof(m_Colors) == _countof(stColorSaveKeyName));
		bool bRet = false;
		for (size_t i =0; i < _countof(m_Colors); i++)
		{
			bRet = LoadINIFileData<D3DCOLOR>(pIniFile, pSection, stColorSaveKeyName[i], 0, &(m_Colors[i]));
			KG_PROCESS_ERROR(bRet);
		}
		bRet = LoadINIFileBigData<D3DVECTOR>(pIniFile, pSection, _T("Direction"), &m_Direction);
		return true;
Exit0:
		return false;
	}

#if defined(_DEBUG) | defined(DEBUG)
	void KG3DDWGC_Key_Light::DebugPrint( std::ostringstream& OSS )
	{
		OSS << "C1-3 :" << std::hex << m_Colors[0] << " " <<  m_Colors[1] << " " << m_Colors[2] 
		<< std::oct << "Dir: " << m_Direction.x << "," << m_Direction.y << "," << m_Direction.z;
	}
#endif
	
	KG3DEnvEffLightTester::KG3DEnvEffLightTester(KG3DEnvEffLight& Light)
	{

		KG3DEnvEffLightDescStruct StanderStruct;
		StanderStruct.SetDefault();
		KG3DDWGC_Key_Light StanderKey;
		//Case 1 KG3DEnvEffLightDescStruct::ToKey
		{
			bool IsConst = false;
			KG3DEnvEffLightDescStruct TempStruct = StanderStruct;
			StanderStruct.ToKey(StanderKey);
			if(StanderStruct == TempStruct)
				IsConst = true;
			_ASSERTE(IsConst);//ToKey不会改变内部

			_ASSERTE(StanderKey.GetDiffuse() == Value2Color(StanderStruct.m_D3DLight.Diffuse));
			_ASSERTE(StanderKey.GetAmbient() == Value2Color(StanderStruct.m_D3DLight.Ambient));
			_ASSERTE(StanderKey.GetSpecular() == Value2Color(StanderStruct.m_D3DLight.Specular));
			_ASSERTE(StanderKey.GetColorArgCount() == KG3DEnvEffLightDescStruct::color_arg_num);
			for (size_t i = 0; i < StanderKey.GetColorArgCount(); i++)
			{
				_ASSERTE(StanderKey.GetColorArg(i) == StanderStruct.m_ColorArgs[i]);
			}
			_ASSERTE(0 == memcmp(&StanderKey.m_Direction, &StanderStruct.m_D3DLight.Direction, sizeof(D3DVECTOR)));
		}
		//Case 2 KG3DEnvEffLightDescStruct::SynWithKey
		{
			//这时候Key和StanderStruct应该有相同的内容，尽管结构是不一样的
			KG3DEnvEffLightDescStruct TempStruct;
			TempStruct.SetDefault();

			KG3DDWGC_Key_Light TempKey;
			TempKey = StanderKey;

			TempStruct.SynWithKey(StanderKey);
			bool bIsConst = false;
			if(StanderKey == TempKey)
				bIsConst = true;
			_ASSERTE(bIsConst);

			bool bIsSynWithKeySucceeded = false;
			{
				_ASSERTE(MT_FLOAT_EQUAL_BIAS(TempStruct.m_D3DLight.Position.x, StanderStruct.m_D3DLight.Position.x, FLT_EPSILON));
				_ASSERTE(MT_FLOAT_EQUAL_BIAS(TempStruct.m_D3DLight.Position.y, StanderStruct.m_D3DLight.Position.y, FLT_EPSILON));
				_ASSERTE(MT_FLOAT_EQUAL_BIAS(TempStruct.m_D3DLight.Position.z, StanderStruct.m_D3DLight.Position.z, FLT_EPSILON));
				_ASSERTE(TempStruct.m_D3DLight.Type == StanderStruct.m_D3DLight.Type);
				KG3DDWGC_Key_Light TempTestKey;
				TempStruct.ToKey(TempTestKey);
				if (TempTestKey == StanderKey)//不能直接比较KG3DEnvEffLightDescStruct，浮点数有精度差，一个个用偏移比较太麻烦
				{
					bIsSynWithKeySucceeded = true;
				}
			}
			_ASSERTE(bIsSynWithKeySucceeded);
		}
		//Case 3 KG3DDWGC_Key_Light::CalculateInterp
		{
			KG3DDWGC_Key_Light PrevKey;
			_ASSERTE(PrevKey.GetDiffuse() == PrevKey.GetColorArg(0) && PrevKey.GetColorArg(0)== 0);//默认构造函数是ZeroMemory的
			KG3DDWGC_Key_Light NextKey;
			for (size_t i = 0; i < KG3DDWGC_Key_Light::color_num; i++)
			{
				NextKey.m_Colors[i] = 0xFFFFFFFF;
			}
			NextKey.m_Direction.x = NextKey.m_Direction.y = NextKey.m_Direction.z = 1.0f;

			KG3DDWGC_Key_Light ResultKey;
			bool bRet = ResultKey.CalculateInterp(PrevKey, NextKey, 0.5);
			_ASSERTE(bRet);
			bRet;

			INT BlendedResult = (255 + 0)/2;
			D3DCOLOR StanderResultColor = D3DCOLOR_ARGB(BlendedResult, BlendedResult, BlendedResult, BlendedResult);
			StanderResultColor;
			for (size_t i = 0; i < KG3DDWGC_Key_Light::color_num; i++)
			{
				_ASSERTE(ResultKey.m_Colors[i] == StanderResultColor);
			}
			_ASSERTE(MT_FLOAT_EQUAL_BIAS(ResultKey.m_Direction.x, 0.5, FLT_EPSILON));
			_ASSERTE(MT_FLOAT_EQUAL_BIAS(ResultKey.m_Direction.y, 0.5, FLT_EPSILON));
			_ASSERTE(MT_FLOAT_EQUAL_BIAS(ResultKey.m_Direction.y, 0.5, FLT_EPSILON));
		}
		//_KG3D_DEBUG_REPORT("KG3DEnvEffLight Runtime Test Passed\n");
	}
#if defined(KG_ENABLE_TDD)
	KG_TEST_CLASS_TEST_BEGIN(KG3DEnvEffLight)
	{
		KG3DEnvEffLight lightTemp;

		//D3DLIGHTTYPE    Type;            /* Type of light source */
		//D3DCOLORVALUE   Diffuse;         /* Diffuse color of light */
		//D3DCOLORVALUE   Specular;        /* Specular color of light */
		//D3DCOLORVALUE   Ambient;         /* Ambient color of light */
		//D3DVECTOR       Position;         /* Position in world space */
		//D3DVECTOR       Direction;        /* Direction in world space */
		//float           Range;            /* Cutoff range */
		//float           Falloff;          /* Falloff */
		//float           Attenuation0;     /* Constant attenuation */
		//float           Attenuation1;     /* Linear attenuation */
		//float           Attenuation2;     /* Quadratic attenuation */
		//float           Theta;            /* Inner angle of spotlight cone */
		//float           Phi;              /* Outer angle of spotlight cone */

		KG3DEnvEffLightDescStruct descStandard;
		{
			D3DCOLORVALUE cvSpecial = {1,2,3,4};
			D3DVECTOR vPos = {1,2,3};
			D3DXVECTOR3 vDir(-1,0,0);
			D3DXVec3Normalize(&vDir, &vDir);

			descStandard.m_D3DLight.Type = D3DLIGHT_DIRECTIONAL;
			descStandard.m_D3DLight.Diffuse = cvSpecial;
			descStandard.m_D3DLight.Specular = cvSpecial;
			descStandard.m_D3DLight.Ambient = cvSpecial;
			descStandard.m_D3DLight.Position = vPos;
			descStandard.m_D3DLight.Direction = vDir;
			descStandard.m_D3DLight.Range = 123;
			descStandard.m_D3DLight.Falloff = 123;
			descStandard.m_D3DLight.Attenuation0 = 123;
			descStandard.m_D3DLight.Attenuation1 = 123;
			descStandard.m_D3DLight.Attenuation2 = 123;
			descStandard.m_D3DLight.Theta = 123;
			descStandard.m_D3DLight.Phi = 123;

			descStandard.m_ColorArgs[KG3DENVEFFLIGHT_DESCSTRUCT::color_arg_sky_light] = D3DCOLOR_ARGB(1,2,3,4);
			descStandard.m_ColorArgs[KG3DENVEFFLIGHT_DESCSTRUCT::color_arg_comp_light] = D3DCOLOR_ARGB(1,2,3,4);
			descStandard.m_ColorArgs[KG3DENVEFFLIGHT_DESCSTRUCT::color_arg_scene_ambient] = D3DCOLOR_ARGB(1,2,3,4);
			descStandard.m_ColorArgs[KG3DENVEFFLIGHT_DESCSTRUCT::color_arg_not_use_current0] = D3DCOLOR_ARGB(1,2,3,4);
		}

		HRESULT hr = E_FAIL;

		{
			KG3DEnvEffLightDescStruct descTemp = descStandard;
			descTemp.m_dwModificationOption = KG3DENVEFFLIGHT_MDO_ALL;
			hr = lightTemp.RequestModification(&descTemp);
			_ASSERTE(SUCCEEDED(hr));
			_ASSERTE(lightTemp.m_DescStruct == descStandard);
		}
	}
	KG_TEST_CLASS_TEST_END(KG3DEnvEffLight)
#endif	
};
