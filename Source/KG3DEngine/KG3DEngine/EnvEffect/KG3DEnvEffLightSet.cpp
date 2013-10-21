////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffLightSet.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-11-22 11:09:15
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DEnvEffLightSet.h"
#include "KG3DEnvEffCommonDef.h"
#include "KGHelperClasses.h"
#include "KG3DEnvEffLight.h"
#include "KGINIFileHelpersEx.h"
#include "KG3DGraphicsTool.h"
#include "KG3DScene.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
namespace KG3D_ENVIRONMENT_EFFECT
{
	KG3DEnvEffLightSet::KG3DEnvEffLightSet()
		:KG3DEnvEffect(0, KG3DTYPE_ENVEFF_LIGHTSET)
		,m_fPercent(0)
	{
		//由于这个类在Environment中是用CommonBuffer的方式静态持有的，所以可能在加载场景的时候会被创建两次
	}

	KG3DEnvEffLightSet::~KG3DEnvEffLightSet()
	{
		Clear();
	}

	HRESULT KG3DEnvEffLightSet::Render(const KG3DEnvEffRenderContext& renderContext)
	{
		for (TypeLightContainer::iterator it = m_vecLight.begin(); it != m_vecLight.end(); ++it)
		{
			KG3DEnvEffLight* p = *it;
			p->Render(renderContext);
		}
		return S_OK;
	}

	HRESULT KG3DEnvEffLightSet::FrameMove()
	{
		KG_CUSTOM_HELPERS::ForAll(m_vecLight, mem_fun(&KG3DEnvEffLight::FrameMove));
		return S_OK;
	}

/*
	HRESULT KG3DEnvEffLightSet::EnsureSunLightAndPlayerLight()
	{
		HRESULT hr = E_FAIL;
		if (m_vecLight.size() < 2)//如果读取不能成功的读出两个Light，那么无论如何也创建两个Light
		{
			for (size_t u = m_vecLight.size(); u < 2; u++)
			{
				hr = this->Add(NULL, NULL);
				KG_COM_PROCESS_ERROR(hr);
			}
		}
		return S_OK;
Exit0:
		return E_FAIL;
	}*/

	HRESULT KG3DEnvEffLightSet::LoadMapSetting( IIniFile* pIniFile, LPCTSTR)
	{
		//默认状态
		HRESULT hr = E_FAIL;
		if (! pIniFile)
		{
			AddTwoDefaultLights();

			this->RequestModification(NULL);
			return S_OK;
		}
	
		this->Clear();

		//读取
		this->RequestModification(NULL);

		//旧的加载
		//if (! KG_CUSTOM_HELPERS::IsSectionExist(pIniFile, _T("LightSet")))
		//{
		//	AddTwoDefaultLights();

		//	KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
		//	_ASSERTE(pScene);
		//	const std::tstring tczLightFileName = this->GetLTFileFileNameFromSceneLoadingPath(pScene->m_scName.c_str());

		//	KG3DEnvEffLightSetDescStruct0 TempStruct; 
		//	hr = TempStruct.LoadFromFile(tczLightFileName.c_str(), pIniFile);
		//	KG_COM_PROCESS_ERROR(hr);
		//	
		//	

		//	if(TempStruct.m_bSunLightReadSucceed)
		//		GetSunLight().RequestModification(KGCommonBufferGetPtr(TempStruct.m_pSunLightDesc));
		//	if (TempStruct.m_bPlayerLightReadSucceed)
		//	{
		//		if (TempStruct.m_bSunLightReadSucceed)
		//		{
		//			//PlayerLight没有保存Direction，由SunLight的Direction同步
		//			TempStruct.m_pPlayerLightDesc->m_D3DLight.Direction = TempStruct.m_pSunLightDesc->m_D3DLight.Direction;
		//		}
		//		GetPlayerLight().RequestModification(KGCommonBufferGetPtr(TempStruct.m_pPlayerLightDesc));
		//	}

		//	//SceneAmibient由SunLight的读取中来
		//	if(TempStruct.m_bSunLightReadSucceed)
		//		this->SetSceneAmbient(TempStruct.m_pSunLightDesc->m_dwSceneAmbient);

		//	//原来就是这样的，SceneEditor在LoadMapSetting的时候又读了这个东东一次
		//	D3DCOLOR dwTemp;
		//	if (KG_CUSTOM_HELPERS::LoadINIFileData<D3DCOLOR>(pIniFile, _T("SceneAmbient"), _T("dwAmbient"), 0, &dwTemp))
		//	{
		//		this->SetSceneAmbient(dwTemp);
		//	}
		//}
		//else
		{
			KG3DEnvEffLightSetLoader TempLoader;
			hr = TempLoader.LoadMapSetting(pIniFile, reinterpret_cast<LPCTSTR>((KG3DDWGC_Key_LightSet*)this));
			KG_COM_PROCESS_ERROR(hr);
			_ASSERTE(m_vecLight.size() >= 2);
		}
//Exit1:
			return S_OK;
Exit0:
		return E_FAIL;
	}
	const std::tstring KG3DEnvEffLightSet::GetLTFileFileNameFromSceneLoadingPath( LPCTSTR ScenePath)
	{
		TCHAR tczLightFileName[MAX_PATH]  = {'\0'};

		{
			TCHAR tczDriver[MAX_PATH];
			TCHAR tczDir[MAX_PATH];
			TCHAR tczName[MAX_PATH];
			TCHAR tczExt[MAX_PATH];
			
			_tsplitpath_s(ScenePath, tczDriver, tczDir, tczName, tczExt);
			_stprintf_s(tczLightFileName, _T("%s%s%s.lt"), tczDriver, tczDir, tczName);
		}
		return tczLightFileName;
	}
	HRESULT KG3DEnvEffLightSet::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		return m_DescStruct.SaveMapSetting(pIniFile, reinterpret_cast<LPCTSTR>((KG3DDWGC_Key_LightSet*)this));
	}	

	HRESULT KG3DEnvEffLightSet::ToSolidInterface( LPVOID* pEffectInterface )
	{
		if (! pEffectInterface)
			return E_FAIL;
		IEKG3DEnvEffLightSet* pIE = this;
		*pEffectInterface = pIE;
		return S_OK;

	}

	HRESULT KG3DEnvEffLightSet::Add( IEKG3DEnvEffLight** pRetLight, INT* pIndex )
	{
		KG3DEnvEffLight* pLight = this->Add(pIndex);
		KG_PROCESS_ERROR(pLight);
		if (pRetLight)
			*pRetLight = pLight;
		return S_OK;
Exit0:
		return E_FAIL;
	}

	KG3DEnvEffLight* KG3DEnvEffLightSet::Add(INT* pIndex)
	{
		KG3DEnvEffLight* pLight = NULL;
		KG_PROCESS_ERROR(m_vecLight.size() < max_light);
		//如果是创建第一和第二个Light的话，就使用Buffer，而不是New，保证创建成功
		switch(m_vecLight.size())
		{
		case 0:
			{
				pLight = KGCommonBufferGetPtr(m_pSunLightBuffer);
				_ASSERTE(pLight);
				m_vecLight.push_back(pLight);
			}
			break;
		case 1:
			{
				pLight = KGCommonBufferGetPtr(m_pPlayerLightBuffer);
				_ASSERTE(pLight);
				m_vecLight.push_back(pLight);
			}
			break;
		default:
			{
				pLight = new KG3DEnvEffLight;
				KG_PROCESS_ERROR(pLight);
				m_vecLight.push_back(pLight);
			}
			break;
		}
		_ASSERTE(pLight);
		if(pIndex)
			*pIndex= static_cast<INT>(m_vecLight.size() - 1);
		return pLight;
Exit0:
		return NULL;
	}
	HRESULT KG3DEnvEffLightSet::Del( INT nIndex )
	{
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(nIndex >= 0 && nIndex < static_cast<INT>(m_vecLight.size()) );
		{
			if (nIndex == 0)
			{
				_ASSERTE(m_vecLight[0] == KGCommonBufferGetPtr(m_pSunLightBuffer));
				KG3DEnvEffLight* pRecreatedLight = KGCommonBufferRecreate(m_pSunLightBuffer);//在Buffer中Displacement Delete然后再次Displacement New，指针不变
				_ASSERTE(pRecreatedLight == m_vecLight[0]);
				hr = pRecreatedLight->RequestModification(NULL);
				_ASSERTE(SUCCEEDED(hr));
			}
			else if (nIndex == 1)
			{
				_ASSERTE(m_vecLight[1] == KGCommonBufferGetPtr(m_pPlayerLightBuffer));
				KG3DEnvEffLight* pRecreatedLight = KGCommonBufferRecreate(m_pPlayerLightBuffer);//在Buffer中Displacement Delete然后再次Displacement New，指针不变
				_ASSERTE(pRecreatedLight == m_vecLight[1]);
				hr = pRecreatedLight->RequestModification(NULL);
				_ASSERTE(SUCCEEDED(hr));
			}
			else
			{
				KG3DEnvEffLight* pLight = m_vecLight[nIndex];
				_ASSERTE(pLight);
				TypeLightContainer::iterator it = m_vecLight.begin();
				std::advance(it, nIndex);
				m_vecLight.erase(it);
				delete pLight;
				
			}
			_ASSERTE(m_vecLight.size() >= 2);//保证一个SunLight一个PlayerLight
			
			return S_OK;
		}
Exit0:
		return E_FAIL;
	}	

	HRESULT KG3DEnvEffLightSet::Clear()
	{
		size_t SizeTemp = m_vecLight.size();
		KG_PROCESS_SUCCESS(SizeTemp == 0);
		if (SizeTemp > 2)
		{
			TypeLightContainer::iterator it = m_vecLight.begin();
			it+=2;
			for(; it != m_vecLight.end(); ++it)
			{
				delete *it;
			}
			SizeTemp = 2;
		}
		if (SizeTemp > 1)
		{
			_ASSERTE(m_vecLight[1] == KGCommonBufferGetPtr(m_pPlayerLightBuffer));
			KGCommonBufferRecreate(m_pPlayerLightBuffer);
			SizeTemp = 1;
		}
		if (SizeTemp > 0)
		{
			_ASSERTE(m_vecLight[0] == KGCommonBufferGetPtr(m_pSunLightBuffer));
			KGCommonBufferRecreate(m_pSunLightBuffer);
		}
		m_vecLight.clear();		
Exit1:
		return S_OK;
	}

	HRESULT KG3DEnvEffLightSet::Size(INT* pSize)
	{
		if(!pSize)
			return E_FAIL;
		*pSize = static_cast<INT>(m_vecLight.size());
		return S_OK;
	}

	INT KG3DEnvEffLightSet::Size()
	{
		return (INT)m_vecLight.size();
	}
	HRESULT KG3DEnvEffLightSet::Get( INT nIndex, IEKG3DEnvEffLight** pRetLight )
	{
		KG_PROCESS_ERROR(pRetLight);
		KG_PROCESS_ERROR(nIndex >= 0 && nIndex < static_cast<INT>(m_vecLight.size()));
		KG3DEnvEffLight* pLight = m_vecLight[nIndex];
		*pRetLight = pLight;
		return S_OK;
Exit0:
		return E_FAIL;
	}

	KG3DEnvEffLight* KG3DEnvEffLightSet::Get( INT nIndex )
	{
		if(nIndex >= 0 && nIndex < (INT)m_vecLight.size())
			return m_vecLight[nIndex];
		return NULL;
	}
	KG3DEnvEffLight& KG3DEnvEffLightSet::GetSunLight()
	{
#if defined(_DEBUG) | defined(DEBUG)
		if (m_vecLight.size() > 0)
			_ASSERTE(m_vecLight[0] == KGCommonBufferGetPtr(m_pSunLightBuffer));
#endif
		return *KGCommonBufferGetPtr(m_pSunLightBuffer);
	}

	KG3DEnvEffLight& KG3DEnvEffLightSet::GetPlayerLight()
	{
#if defined(_DEBUG) | defined(DEBUG)
		if (m_vecLight.size() > 1)
			_ASSERTE(m_vecLight[1] == KGCommonBufferGetPtr(m_pPlayerLightBuffer));
#endif
		return *KGCommonBufferGetPtr(m_pPlayerLightBuffer);
	}

	HRESULT KG3DEnvEffLightSet::Apply()
	{
		GetSunLight().Apply(0);
		//关掉其它的Light
		for (int i = 1; i < 8; ++i)
		{
			g_pd3dDevice->LightEnable(i, FALSE);
		}
		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);//如果一个Light都没有的话是要关掉的，但是SunLight是恒定的，所以可以直接设True
		g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, m_DescStruct.m_dwSceneAmbient);
		return S_OK;
	}

	KG3DEnvEffect* KG3DEnvEffLightSet::GetSubEffect( DWORD dwType, int nFinded )
	{
		if(dwType == DWGCSUBKEY_TYPE)
			return this->Get(nFinded);
		return NULL;
	}

	KG3DEnvEffect* KG3DEnvEffLightSet::GetSubEffect( INT nIndex, KG3DENVEFF_HANDLE* pHandle /*= NULL*/ )
	{
		KG3DEnvEffect* pEffect = this->Get(nIndex);
		if (pEffect && pHandle)
		{
			pHandle->m_dwType = DWGCSUBKEY_TYPE;
			pHandle->m_nIndex = nIndex;
		}
		return pEffect;
	}

	INT KG3DEnvEffLightSet::GetSubEffectCount()
	{
		return Size();
	}

	INT KG3DEnvEffLightSet::GetSubEffectCount( DWORD dwSubType )
	{
		if(dwSubType == KG3DTYPE_ENVEFF_LIGHT)
			return GetSubEffectCount();
		return 0;
	}
	HRESULT KG3DEnvEffLightSet::RequestModification( KG3DENVEFFLIGHTSET_DESCSTRUCT* pStruct )
	{
		if (! pStruct)
		{
			m_DescStruct.SetDefault();
			m_DescStruct.ToKey(m_KeyResult);
		}
		else
		{
			if (pStruct->m_dwModificationOption & KG3DENVEFFLIGHTSET_MDO_AMBIENT)
			{
				m_DescStruct.m_dwSceneAmbient = pStruct->m_dwSceneAmbient;
			}
			if (! this->IsSubStateEnable())
			{
				m_DescStruct.ToKey(m_KeyResult);
			}
		}
		return S_OK;
	}

	HRESULT KG3DEnvEffLightSet::GetStruct( KG3DENVEFFLIGHTSET_DESCSTRUCT* pStruct )
	{
		if (pStruct)
		{
			m_DescStruct.ToBaseStruct(*pStruct);
			return S_OK;
		}
		return E_FAIL;
	}

	D3DCOLOR KG3DEnvEffLightSet::GetSceneAmbient()
	{
		return m_KeyResult.m_Ambient;
	}

	bool KG3DEnvEffLightSet::SetSceneAmbient( D3DCOLOR Color )
	{
		if (this->IsSubStateEnable())
		{
			_KG3D_DEBUG_REPORT("在动态天气渐变过程中改变Ambient，无效");			
			return false;
		}
		KG3DENVEFFLIGHTSET_DESCSTRUCT stStruct;
		stStruct.m_dwSceneAmbient = Color;
		stStruct.m_dwModificationOption = KG3DENVEFFLIGHTSET_MDO_AMBIENT;
		return (SUCCEEDED(this->RequestModification(&stStruct)));
	}

	void KG3DEnvEffLightSet::AddTwoDefaultLights()
	{
		if (m_vecLight.size() < 2)//不管能不能的读出两个Light，那么无论如何也创建两个Light
		{
			for (size_t u = m_vecLight.size(); u < 2; u++)
			{
				KG3DEnvEffLight* pLight = this->Add(NULL);
				_ASSERTE(pLight);
#if defined(_DEBUG) | defined(DEBUG)
				if(u == 0)
					pLight = KGCommonBufferGetPtr(m_pSunLightBuffer);
				else if(u == 1)
					pLight = KGCommonBufferGetPtr(m_pPlayerLightBuffer);
#endif
				HRESULT hr = pLight->LoadMapSetting(NULL, NULL);
				_ASSERTE(SUCCEEDED(hr));(VOID)hr;
			}
		}
	}

	HRESULT KG3DEnvEffLightSet::DWGCRequestPopFrontState(float fCurBlendPercent /*= 0.0f*/)
	{
		if (fCurBlendPercent > 0 && fCurBlendPercent < 1)
		{
			KG3DDWGC_Key_LightSet MidKey, BeforeKey;
			m_DescStruct.ToKey(BeforeKey);
			MidKey.CalculateInterp(BeforeKey, m_SubKey, fCurBlendPercent);
			m_DescStruct.SynWithKey(MidKey);
			m_DescStruct.ToKey(m_KeyResult);
		}
		else
		{
			m_DescStruct.SynWithKey(m_SubKey);
			m_DescStruct.ToKey(m_KeyResult);
		}
		
		return S_OK;
	}

	HRESULT KG3DEnvEffLightSet::DWGCRequestSubStateChange( const KG3DDWGC_KeyBase* pRetKey)
	{
		const KG3DDWGC_Key_LightSet* pLightSet = dynamic_cast<const KG3DDWGC_Key_LightSet*>(pRetKey);
		if(! pLightSet)
			return E_FAIL;

		m_SubKey = *pLightSet;
		return S_OK;
	}

	HRESULT KG3DEnvEffLightSet::DWGCGetCurStateToKey( KG3DDWGC_KeyBase* pRetKey)
	{
		KG3DDWGC_Key_LightSet* pLightSet = dynamic_cast<KG3DDWGC_Key_LightSet*>(pRetKey);
		if(! pLightSet)
			return E_FAIL;

		m_DescStruct.ToKey(*pLightSet);
		return S_OK;
	}

	HRESULT KG3DEnvEffLightSet::DWGCEnableSubState( BOOL bEnable )
	{
		this->SetSubStateEnable(TRUE);
		return S_OK;				
	}

	HRESULT KG3DEnvEffLightSet::DWGCSetPercent( float fPercent )
	{
		m_fPercent = fPercent;

		KG3DDWGC_Key_LightSet TempKey;
		m_DescStruct.ToKey(TempKey);

		m_KeyResult.CalculateInterp(TempKey, m_SubKey, m_fPercent);
		return S_OK;
	}

	HRESULT KG3DEnvEffLightSet::DWGCRequestModification( const KG3DDWGC_KeyBase* pKey )
	{
		const KG3DDWGC_Key_LightSet* pLightSet = dynamic_cast<const KG3DDWGC_Key_LightSet*>(pKey);
		if(! pLightSet)
			return E_FAIL;

		KG3DEnvEffLightSetDescStruct TempStruct;
		TempStruct.SynWithKey(*pLightSet);
		TempStruct.m_dwModificationOption = KG3DENVEFFLIGHTSET_MDO_AMBIENT;
		return RequestModification(&TempStruct);
	}

	VOID KG3DEnvEffLightSet::GetExistSubEffectTypeList( std::vector<DWORD>& Types )
	{
		if (m_vecLight.size() > 0)
		{
			Types.push_back(KG3DTYPE_ENVEFF_LIGHT);
		}
	}
	
	
	
	//HRESULT KG3DEnvEffLightSetDescStruct0::LoadFromFile( LPCTSTR tcsFileName, IIniFile* pIniFile)
	//{
	//	//原来的读取是一个从lt文件，一个从ini文件读取的。
	//	_ASSERTE(m_pSunLightDesc);
	//	m_bSunLightReadSucceed = false;
	//	m_bPlayerLightReadSucceed = false;
	//	m_pSunLightDesc->SetDefault();
	//	HRESULT hr = m_pSunLightDesc->LoadFromFile(tcsFileName);
	//	if (SUCCEEDED(hr))
	//		m_bSunLightReadSucceed = true;

	//	{
	//		_ASSERTE(m_pPlayerLightDesc);
	//		m_pPlayerLightDesc->SetDefault();
	//		/*
	//		pIniFile->GetInteger("Player Light", "Diffuse", 0xffffffff, (int*)&dwDiffuse);
	//					m_PlayerLight.SetLightDiffuse(Color2Value(dwDiffuse));
	//					pIniFile->GetInteger("Player Light", "Ambient", 0xffffffff, (int*)m_PlayerLight.GetSceneAmbientPtr());
	//					pIniFile->GetInteger("Player Light", "SkyLight", 0xffffffff, (int*)&dwSkyLight);
	//					m_PlayerLight.GetSkyLightDiffuse() = Color2Value(dwSkyLight);
	//					pIniFile->GetInteger("Player Light", "ComLight", 0xffffffff, (int*)&dwComLight);
	//					m_PlayerLight.GetComLightDiffuse() = Color2Value(dwComLight);*/

	//		if (pIniFile)
	//		{
	//			m_bPlayerLightReadSucceed = true;
	//			using namespace KG_CUSTOM_HELPERS;
	//			INT nTemp = 0;
	//			if(! LoadINIFileData<INT>(pIniFile, _T("Player Light"), _T("Diffuse"), 0, &nTemp))
	//				m_bPlayerLightReadSucceed = false;
	//			else
	//				m_pPlayerLightDesc->m_D3DLight.Diffuse = Color2Value(_kg_to_smaller_size_cast<D3DCOLOR>(nTemp));

	//			if(! LoadINIFileData<INT>(pIniFile, _T("Player Light"), _T("Ambient"), 0, &nTemp))
	//				m_bPlayerLightReadSucceed = false;
	//			else
	//			{
	//				//m_pPlayerLightDesc->m_D3DLight.Ambient = Color2Value(_kg_to_smaller_size_cast<D3DCOLOR>(nTemp));
	//				m_pPlayerLightDesc->m_dwSceneAmbient =_kg_to_smaller_size_cast<D3DCOLOR>(nTemp); 
	//			}
	//			
	//			if(! LoadINIFileData<INT>(pIniFile, _T("Player Light"), _T("SkyLight"), 0, &nTemp))
	//				m_bPlayerLightReadSucceed = false;
	//			else
	//				m_pPlayerLightDesc->m_SkyLightInfo.Diffuse = Color2Value(_kg_to_smaller_size_cast<D3DCOLOR>(nTemp));

	//			if(! LoadINIFileData<INT>(pIniFile, _T("Player Light"), _T("ComLight"), 0, &nTemp))
	//				m_bPlayerLightReadSucceed = false;
	//			else
	//				m_pPlayerLightDesc->m_ComLightInfo.Diffuse = Color2Value(_kg_to_smaller_size_cast<D3DCOLOR>(nTemp));
	//		}
	//	}

	//	return (m_bSunLightReadSucceed || m_bPlayerLightReadSucceed) ? S_OK : E_FAIL;//随便一个成功
	//}

	/*HRESULT KG3DEnvEffLightSetDescStruct0::SaveToFile( LPCTSTR tcsFileName )
	{
		_ASSERTE(m_pSunLightDesc);
		return m_pSunLightDesc->SaveToFile(tcsFileName);
	}*/

	HRESULT KG3DEnvEffLightSetDescStruct1::LoadMapSetting( IIniFile* pFile, LPCTSTR p )
	{
		HRESULT hr = E_FAIL;
		KG3DEnvEffLightSet* pLightSet = reinterpret_cast<KG3DEnvEffLightSet*>(const_cast<LPTSTR>(p));
		KG_PROCESS_ERROR(pFile && pLightSet);

		{
			TCHAR tczKeyTemp[MAX_PATH] = _T("");
			TCHAR tczSectionString[MAX_PATH] = _T("");

			TCHAR tczSection[] = _T("LightSet");

			using namespace KG_CUSTOM_HELPERS;
			bool bRet = false;
			{
				D3DCOLOR dwTemp = 0;
				bRet = LoadINIFileData<D3DCOLOR>(pFile, tczSection, _T("SceneAmbient"), 0, &dwTemp);
				KG_PROCESS_ERROR(bRet);
				m_dwSceneAmbient = dwTemp;

				m_dwModificationOption = KG3DENVEFFLIGHTSET_MDO_ALL;
				hr = pLightSet->RequestModification(this);
				KG_COM_PROCESS_ERROR(hr);
			}
			
			_tcscpy_s(tczKeyTemp, _T("SceneAmbient"));
			while (GetNextKey(pFile, tczSection, tczKeyTemp, tczKeyTemp))
			{
				if (pLightSet->Size() >= KG3DEnvEffLightSet::max_light)
				{
					break;	//忽略多余的灯光
				}

				bRet = LoadINIFileData(pFile, tczSection, tczKeyTemp, _T(""), tczSectionString);
				if (! bRet)
					continue;

				_ASSERTE(*tczSectionString != '\0');

				INT nIndex = -1;
				KG3DEnvEffLight* pLight = pLightSet->Add(&nIndex);
				KG_PROCESS_ERROR(pLight);
				_ASSERTE(nIndex >= 0);

				hr = pLight->LoadMapSetting(pFile, tczSectionString);
				if(FAILED(hr))
				{
					hr = pLightSet->Del(nIndex);
					_ASSERTE(SUCCEEDED(hr));
					pLight = NULL;
					goto Exit0;
				}
			}
		}

		//Exit1:
		return S_OK;
Exit0:
		_KG3D_DEBUG_REPORT("LightSet Loader读取错误");
		return E_FAIL;	
	}

	HRESULT KG3DEnvEffLightSetDescStruct1::SaveMapSetting( IIniFile* pFile, LPCTSTR p )
	{
		KG3DEnvEffLightSet* pLightSet = reinterpret_cast<KG3DEnvEffLightSet*>(const_cast<LPTSTR>(p));
		KG_PROCESS_ERROR(pFile && pLightSet);
		{
			using namespace KG_CUSTOM_HELPERS;

			TCHAR tczNum[MAX_PATH] = _T("");
			TCHAR tczKeyTemp[MAX_PATH] = _T("");

			TCHAR tczSection[] = _T("LightSet");

			EraseSection(pFile, _T("SceneAmbient"));//这个Section是以前遗留的，为了避免麻烦，顺便在新的保存中搞掉它
			EraseSection(pFile, tczSection);

			INT nSize = pLightSet->Size();
			_ASSERTE(nSize >= 2);
			bool bRet = false;
			HRESULT hr = E_FAIL;

			bRet = SaveINIFileData<D3DCOLOR>(pFile, tczSection, _T("SceneAmbient"), m_dwSceneAmbient);
			KG_PROCESS_ERROR(bRet);
			for (INT i = 0; i < nSize; i++)
			{
				KG3DEnvEffLight* pLight = pLightSet->Get(i);
				_ASSERTE(pLight);
				
				KG_PROCESS_ERROR(TValueToString(i, tczNum));
				KG_PROCESS_ERROR(_stprintf_s(tczKeyTemp, _T("Light_%d"), i) > 0);
							
				bRet = SaveINIFileData(pFile, tczSection, tczNum, tczKeyTemp);	//形成 0 = Light_0
				KG_PROCESS_ERROR(bRet);
				
				hr = pLight->SaveMapSetting(pFile, tczKeyTemp);	//在Light_0中写入数据
				KG_COM_PROCESS_ERROR(hr);
			}
		}

		return S_OK;
Exit0:
		return E_FAIL;
	}

	KG3DEnvEffLightSetDescStruct1::KG3DEnvEffLightSetDescStruct1()
	{
		ZeroMemory(this,sizeof(KG3DEnvEffLightSetDescStruct1));
		m_dwSceneAmbient = 0xff808080;
	}

	HRESULT KG3DEnvEffLightSetDescStruct1::SynWithKey( const KG3DDWGC_Key_LightSet& Key )
	{
		m_dwSceneAmbient = Key.m_Ambient;
		return S_OK;
	}

	HRESULT KG3DEnvEffLightSetDescStruct1::ToKey( KG3DDWGC_Key_LightSet& Key )
	{
		Key.m_Ambient = m_dwSceneAmbient;
		return S_OK;
	}
	bool KG3DDWGC_Key_LightSet::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		///如果是Invalid的话，当作没有关键帧处理，但不是非法
		_ASSERTE(pIniFile&&pSection);///<DW_SUB_Frame>

		KG_CUSTOM_HELPERS::EraseSection(pIniFile, pSection);

		KG_CUSTOM_HELPERS::SaveINIFileData<D3DCOLOR>(pIniFile, pSection, "Ambient", m_Ambient);
		
		return true;
	}

	bool KG3DDWGC_Key_LightSet::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		///如果是Invalid的话，当作没有关键帧处理，但不是非法
		///<DW_SUB_Frame>
		_ASSERTE(pIniFile&&pSection);
		
		KG_PROCESS_ERROR(KG_CUSTOM_HELPERS::LoadINIFileData<D3DCOLOR>(pIniFile
			, pSection, "Ambient", 0, &m_Ambient));
		
		return true;
Exit0:
		return false;
	}

	bool KG3DDWGC_Key_LightSet::CalculateInterp( const KG3DDWGC_KeyBase& BaseKeyBefore , const KG3DDWGC_KeyBase& BaseKeyAfter , float fPercent)
	{
		const KG3DDWGC_Key_LightSet& KeyBefore = (const KG3DDWGC_Key_LightSet&)(BaseKeyBefore);
		const KG3DDWGC_Key_LightSet& KeyAfter = (const KG3DDWGC_Key_LightSet&)(BaseKeyAfter);

		if(fPercent < FLT_MIN)
			*this = KeyBefore;
		else if (fPercent >= 1.0f)
			*this = KeyAfter;
		else
		{
			TwoColorBlend(KeyBefore.m_Ambient, KeyAfter.m_Ambient, this->m_Ambient, fPercent);			
		}
		return true;
	}
#if defined(DEBUG) | defined(_DEBUG)
	void KG3DDWGC_Key_LightSet::DebugPrint(std::ostringstream& OSS)
	{
		/*
			size_t Size = m_IndexArray.size();
					OSS << "LNum: " << Size; 
					if(Size > 0)
						OSS << "; Sun: "<< m_IndexArray[0];
					if(Size > 1)
						OSS << "; Pl: " << m_IndexArray[1]; */
		OSS << _T("; Amibient :") << m_Ambient;

	}
#endif
};