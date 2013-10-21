#include "stdafx.h"

///编译宏选择
#include "KG3DEnvEffCommonDef.h"

#include "KG3DSkySystem.h"
#include "./KG3DEngineManager.h"
#include "./KG3DGraphicsTool.h"
#include "./KG3DTextureTable.h"
#include "KG3DModelTable.h"
#include "KG3DStructs.h"
#include "KG3DMesh.h"
#include "KGINIFileHelpers.h"
#include "KG3DGraphicsStructEx.h"
#include "KG3DMeshLensFlare.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace KG3D_ENVIRONMENT_EFFECT
{
	/************************************************************************/
	/*SkySystem 
	*/
	/************************************************************************/
	KG3DSkySystem::KG3DSkySystem()
		:KG3DEnvEffect(0, KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM)
		,m_lpSkyBox(NULL)
		,m_d3dc_FogColor(0xffffffff)
		,m_bEnableSynWithFogColor(false)
	{
		 
	}

	KG3DSkySystem::~KG3DSkySystem( void )
	{
		//KG3DDWGC_Key_SkySystem::UnLinkTables();
		ClearSkyBox();
		ClearCloudVec();
		ClearFarMountVec();
	}
	HRESULT KG3DSkySystem::FrameMove()
	{
		KG_PROCESS_ERROR(FALSE != KG3DEnvEffect::IsEnable());
		KG3DMeshSkyBox* pSkyBox = GetSkyBox();
		if ( NULL != pSkyBox && pSkyBox->IsEnable())
		{
			pSkyBox->FrameMove();
		}
		{
			bool bNeedReOrderRender = false;
			for (KG3DCloudPtrVector::iterator i = m_vecClouds.begin(); i!= m_vecClouds.end(); i++)//<DW_SUB_Frame>///记得加上IsEnable
			{
				KG3DCloud* p = *i;
				if(p->IsEnable())
				{
					p->FrameMove();
				}
				if (p->IsHeightChanged())
				{
					bNeedReOrderRender = true;
				}
			}
			if (bNeedReOrderRender)
			{
				SortCloudRenderOrder();
				for (KG3DCloudPtrVector::iterator i = m_vecClouds.begin(); i!= m_vecClouds.end(); i++)//<DW_SUB_Frame>///记得加上IsEnable
				{
					KG3DCloud* p = *i;
					if (p->IsHeightChanged())
					{
						p->OnHeightChangeHandled();
					}
				}
			}
		}
		KG3DMeshFarMountain* pFarmount = NULL;
		for (KG3DMeshFarMountainPtrVector::iterator it2 = m_vecFarMounts.begin(); it2 != m_vecFarMounts.end(); it2++ )
		{
			pFarmount = *it2;
			assert(pFarmount);
			if (pFarmount->IsEnable())
			{
				pFarmount->FrameMove();
			}
			
		}
		//Exit1:
		return S_OK;
Exit0:
		return E_FAIL;
	}

    HRESULT KG3DSkySystem::OnLostDevice()
    {
        HRESULT hr = E_FAIL;
        HRESULT hrResult = E_FAIL;

        {
            KG3DMeshFarMountainPtrVector::iterator it = m_vecFarMounts.begin();
            KG3DMeshFarMountainPtrVector::iterator ie = m_vecFarMounts.end();
            for (; it != ie; ++it)
            {
                KG3DMeshFarMountain* p = *it;

                hr = p->OnLostDevice();
                KGLOG_COM_PROCESS_ERROR(hr);
            }
        }

        {
            KG3DCloudPtrVector::iterator it = m_vecClouds.begin();
            KG3DCloudPtrVector::iterator ie = m_vecClouds.end();
            for (; it != ie; ++it)
            {
                KG3DCloud* p = *it;

                hr = p->OnLostDevice();
                KGLOG_COM_PROCESS_ERROR(hr);
            }
        }

        hrResult = S_OK;
Exit0:
        return hrResult;
    }

    HRESULT KG3DSkySystem::OnResetDevice()
    {
        HRESULT hr = E_FAIL;
        HRESULT hrResult = E_FAIL;

        {
            KG3DCloudPtrVector::iterator it = m_vecClouds.begin();
            KG3DCloudPtrVector::iterator ie = m_vecClouds.end();
            for (; it != ie; ++it)
            {
                KG3DCloud* p = *it;

                hr = p->OnResetDevice();
                KGLOG_COM_PROCESS_ERROR(hr);
            }
        }

        {
            KG3DMeshFarMountainPtrVector::iterator it = m_vecFarMounts.begin();
            KG3DMeshFarMountainPtrVector::iterator ie = m_vecFarMounts.end();
            for (; it != ie; ++it)
            {
                KG3DMeshFarMountain* p = *it;

                hr = p->OnResetDevice();
                KGLOG_COM_PROCESS_ERROR(hr);
            }
        }

        hrResult = S_OK;
Exit0:
        return hrResult;
    }

	HRESULT KG3DSkySystem::Render(const KG3DEnvEffRenderContext& renderContext)
	{
		return S_OK;
	}	

	HRESULT KG3DSkySystem::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{

		_ASSERTE( pIniFile );

		KG3DMeshSkyBox* pSkyBox = GetSkyBox();
		if ( NULL != pSkyBox )
		{
			pSkyBox->SaveMapSetting( pIniFile, NULL);
		}
		char szSection[64] = "";
		char szKeyName[64] = "";
		char szKeyValue[512] = "";
		///Clouds
		KG3DCloudPtrVector::iterator it = m_vecClouds.begin();
		int k = 1;
		_tcscpy_s(szSection, "Clouds");
		KG_CUSTOM_HELPERS::EraseSection(pIniFile, szSection);

		for ( ;it != m_vecClouds.end(); k++, it++ )
		{
			KG3DCloud* p = (KG3DCloud*)*it;
			sprintf_s(szKeyName, "%d", k);
			sprintf_s(szKeyValue, "Cloud%d", k);
			pIniFile->WriteString(szSection, szKeyName, szKeyValue);
			p->SaveMapSetting(pIniFile, szKeyValue);
		}
		///FarMounts
		_tcscpy_s(szSection, "FarMounts");
		KG_CUSTOM_HELPERS::EraseSection(pIniFile, szSection);

		KG3DMeshFarMountain* pFarMount = NULL;
		KG3DMeshFarMountainPtrVector::iterator it2;
		int nCount = 0;
		for ( it2 = m_vecFarMounts.begin(); it2 != m_vecFarMounts.end(); it2++ )
		{
			pFarMount = (KG3DMeshFarMountain*)(*it2);
			_ASSERTE(pFarMount);
			sprintf_s( szKeyName, sizeof(szKeyName), "%d", nCount );
			sprintf_s( szKeyValue, sizeof(szKeyValue), "FarMount%d", nCount );
			pIniFile->WriteString( "FarMounts", szKeyName, szKeyValue );
			pFarMount->SaveMapSetting( pIniFile, szKeyValue );
			nCount++;
		}

		///保存参数
		KG_CUSTOM_HELPERS::SaveINIFileData<BOOL>(pIniFile, "SkySystem", _STRINGER(m_bEnableSynWithFogColor), m_bEnableSynWithFogColor);
		//Exit1:
		return S_OK;
		//Exit0:
		//	return hr;
	}

	HRESULT KG3DSkySystem::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		if (! pIniFile)
		{
			return S_OK;
		}
		KG3DMeshSkyBox* pSkyBox = NULL;
		if (pIniFile->IsSectionExist("SkySphere"))
		{
			pSkyBox = AddSkyBox(pIniFile, _T("SkySphere"));
		}
		

		TCHAR szKeyValue[512];
		char szKeyName[64];
		///Clouds
		*szKeyValue = '\0';
		*szKeyName  = '\0';

		while (pIniFile->GetNextKey("Clouds", szKeyName, szKeyName))
		{
			pIniFile->GetString("Clouds", szKeyName, "", szKeyValue, sizeof(szKeyValue));
			if (*szKeyValue != '\0')
			{
				KG3DCloud* pCloud = KG3DCloud::Build(NULL, reinterpret_cast<DWORD_PTR>(pIniFile), reinterpret_cast<DWORD_PTR>(szKeyValue));
				if(NULL != pCloud)
					m_vecClouds.push_back(pCloud);
			}
		}

		this->SortCloudRenderOrder();
		///FarMountains
		*szKeyValue = '\0';
		*szKeyName  = '\0';
		while (pIniFile->GetNextKey("FarMounts", szKeyName, szKeyName))
		{
			pIniFile->GetString("FarMounts", szKeyName, "", szKeyValue, sizeof(szKeyValue));
			if (*szKeyValue != '\0')
			{
				KG3DMeshFarMountain* pMount = new KG3DMeshFarMountain;
				if (pMount)
				{
					_ASSERTE(NULL != pMount);
					if ( SUCCEEDED( pMount->LoadMapSetting( pIniFile, szKeyValue ) ) )
					{
						if (FAILED(AddFarMountToVec(pMount)))
						{
							SAFE_DELETE(pMount);
						}
					}
					else
					{
						SAFE_DELETE(pMount);
					}
				}
			}
		}///End of While
		/*
		///取回参数
		bool bSuccessLoad = KG_CUSTOM_HELPERS::LoadINIFileData<BOOL>(pIniFile
												, _T("SkySystem")
												, _STRINGER(m_bEnableSynWithFogColor)
												, FALSE
												, &m_bEnableSynWithFogColor);
		
		if (!bSuccessLoad)
				{
					CheckIsEnableSynWithFog(&m_bEnableSynWithFogColor);
				}*/
		
		//Exit1:
		return S_OK;
		//Exit0:
		//	return hr;
	}

	/************************************************************************/
	//云管理部分
	/************************************************************************/
	HRESULT KG3DSkySystem::AddCloud()
	{
		KG3DCloud* pCloud = KG3DCloud::Build(NULL, NULL, NULL);
		KG_PROCESS_ERROR(NULL != pCloud);
		m_vecClouds.push_back(pCloud);
		SortCloudRenderOrder();
		return S_OK;
Exit0:
		SAFE_RELEASE(pCloud);
		return E_FAIL;
	}
	HRESULT KG3DSkySystem::DelCloud( KG3DCloud* pCound )
	{
		KG3DCloudPtrVector::iterator it;
		KG_PROCESS_ERROR(pCound);

		it =std::find(m_vecClouds.begin(), m_vecClouds.end(), pCound);
		if (it != m_vecClouds.end())
		{
			KG3DCloud* pCloud = *it;
				SAFE_RELEASE(pCloud);
			m_vecClouds.erase(it);
		}

		SortCloudRenderOrder();

		return S_OK;
Exit0:
		return E_FAIL;
	}
	HRESULT KG3DSkySystem::DelCloud(int nIndex)
	{
		KG3DCloudPtrVector::iterator it = m_vecClouds.begin();
		KGLOG_PROCESS_ERROR( nIndex >= 0 && nIndex < (int)m_vecClouds.size() );

		it+=nIndex;
		KG3DCloud* pCloud = *(it);
		SAFE_RELEASE(pCloud);
		m_vecClouds.erase(it);

		SortCloudRenderOrder();

		return S_OK;
Exit0:
		return E_FAIL;
	}
	

	KG3DCloud* KG3DSkySystem::GetCloud(int nIndex)
	{
		KG_PROCESS_ERROR( nIndex >= 0 && nIndex < (int)m_vecClouds.size() );
		_ASSERTE( (KG3DCloud*)m_vecClouds[nIndex] );
		return (KG3DCloud*)m_vecClouds[nIndex];
Exit0:
		return NULL;
	}


	int	KG3DSkySystem::GetCloudVecSize()
	{
		return (int)m_vecClouds.size();
	}
	HRESULT	KG3DSkySystem::ClearCloudVec()
	{
		KG3DCloudPtrVector::iterator it;
		KG3DCloud* pCloud = NULL;
		for (it = m_vecClouds.begin(); it != m_vecClouds.end(); it++)
		{
			pCloud = *it;
			SAFE_RELEASE(pCloud);
		}
		m_vecClouds.clear();
		if (0 == m_vecClouds.size())
		{
			return S_OK;
		}
		return E_FAIL;
	}
	BOOL KG3DSkySystem::IsCloudPointerValid(KG3DCloud* pCloud)
	{
		KG_PROCESS_ERROR(pCloud);
		if (m_vecClouds.end() != std::find(m_vecClouds.begin(), m_vecClouds.end(), pCloud))
		{
			return TRUE;
		}
Exit0:
		return FALSE;
	}
	int	KG3DSkySystem::GetCloudIndexByPointer(KG3DCloud* pCloud)
	{
		if(!pCloud)
			return -1;
		KG3DCloudPtrVector::iterator itResult  = std::find(m_vecClouds.begin(), m_vecClouds.end(), pCloud);
		if (itResult != m_vecClouds.end())
		{
			int nResult = (int)(itResult-m_vecClouds.begin());
			_ASSERTE(nResult >= 0 && nResult < (int)m_vecClouds.size());
			return nResult;
		}
		return -1;
	}
	/************************************************************************/
	//云接口定义开始
	/************************************************************************/
	///HRESULT KG3DSkySystem::DelCloud( int nIndex ){}///同管理部分
	HRESULT KG3DSkySystem::DelCloud( IEKG3DCloud* pCloud )
	{
		return DelCloud((KG3DCloud*)pCloud);
	}
	///HRESULT KG3DSkySystem::ClearCloudVec(){}///同管理部分
	HRESULT KG3DSkySystem::GetCloud( int nIndex, IEKG3DCloud** ppCloud )
	{
		KG_PROCESS_ERROR(ppCloud);
		*ppCloud = (IEKG3DCloud*)GetCloud(nIndex);
		if (NULL != *ppCloud)
		{
			return S_OK;
		}
Exit0:
		return E_FAIL;
	}
	HRESULT KG3DSkySystem::GetCloudIndexByPointer( IEKG3DCloud* pCloud, int* pIndex )
	{
		KG_PROCESS_ERROR(pIndex);
		KG_PROCESS_ERROR(pCloud);
		*pIndex = GetCloudIndexByPointer((KG3DCloud*)pCloud);
		if (*pIndex >=0 && *pIndex < (int)m_vecClouds.size() )
		{
			return S_OK;
		}
Exit0:
		return E_FAIL;
	}
	HRESULT KG3DSkySystem::IsCloudPointerValid( IEKG3DCloud* pCloud, BOOL* pBool )
	{
		KG_PROCESS_ERROR(pBool);
		KG_PROCESS_ERROR(pCloud);
		*pBool = IsCloudPointerValid((KG3DCloud*)pCloud);
		if (TRUE == *pBool)
		{
			return S_OK;
		}
Exit0:
		return E_FAIL;
	}
	HRESULT KG3DSkySystem::GetCloudVecSize( int* pSize )
	{
		if(!pSize){return E_FAIL;};
		*pSize = GetCloudVecSize();
		_ASSERTE (*pSize >= 0  );
		return S_OK;
	}
	/*
	HRESULT KG3DSkySystem::SortCloudVec()
		{
			std::sort(m_vecClouds.begin(), m_vecClouds.end(), SortComp);
			return S_OK;
		}*/
	/*
	bool KG3DSkySystem::SortComp( KG3DCloud* pElem1, KG3DCloud* pElem2 )
		{
			_ASSERTE(pElem1);_ASSERTE(pElem2);
			return (pElem1->GetStruct()->m_fHeight) > (pElem2->GetStruct()->m_fHeight);
			}*/
	
	VOID KG3DSkySystem::SortCloudRenderOrder()
	{
		//利用Map的自动排序特性，把高处的云排在渲染顺序前面
		m_vecCloudRenderOrder.clear();
		typedef std::multimap<float, size_t, std::greater<float> > TypeSortMap;
		TypeSortMap	mapSortByHeight;
		KG_PROCESS_SUCCESS(m_vecClouds.empty()); 
		//mapSortByHeight.reserve(m_vecClouds.size());
		m_vecCloudRenderOrder.reserve(m_vecClouds.size());
		for (size_t i = 0; i < m_vecClouds.size(); ++i)
		{
			KG3DCloud* p = m_vecClouds[i];
			mapSortByHeight.insert(std::pair<float, size_t>(p->GetStruct()->m_fHeight, i));
		}

		for (TypeSortMap::iterator it = mapSortByHeight.begin(); it != mapSortByHeight.end(); ++it)
		{
			m_vecCloudRenderOrder.push_back(it->second);
		}

#if defined(DEBUG) | defined(_DEBUG)
		for (unsigned i = 0, j = 1; j < m_vecCloudRenderOrder.size(); ++i, ++j)
		{
			_ASSERTE(m_vecCloudRenderOrder[i] < m_vecClouds.size());
			_ASSERTE(m_vecCloudRenderOrder[j] < m_vecClouds.size());
			float f1 = m_vecClouds[m_vecCloudRenderOrder[i]]->GetStruct()->m_fHeight;
			float f2 = m_vecClouds[m_vecCloudRenderOrder[j]]->GetStruct()->m_fHeight;
			_ASSERTE(f1 > f2 || MT_FLOAT_EQUAL(f1, f2));
		}
#endif

Exit1:
		return;
	}
	
	/************************************************************************/
	//远山部分
	/************************************************************************/
	HRESULT KG3DSkySystem::AddFarMount( const KG3DMeshFarMountDescStruct* pDescStruct, KG3DMeshFarMountain** ppAddedFarmount )
	{
		KG3DMeshFarMountain* pMount = NULL;
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR (MAX_FARMOUNT > (int)m_vecFarMounts.size());///不能过度增加
		{
			pMount = new KG3DMeshFarMountain;
			KG_PROCESS_ERROR(pMount);

			_ASSERTE(NULL != pMount);
			
			hr = pMount->RequestModification(pDescStruct);///pDescStruct如果是NULL就新建
			KG_COM_PROCESS_ERROR( hr );
			///改成最后一个的名字
			KG_COM_PROCESS_ERROR(AddFarMountToVec(pMount));
			if(NULL != ppAddedFarmount)
				*ppAddedFarmount = pMount;
		}
		return S_OK;
Exit0:
		SAFE_DELETE(pMount);
		return E_FAIL;
	}


	HRESULT KG3DSkySystem::AddFarMountToVec(KG3DMeshFarMountain* pFarmount)///这是最后的动作
	{
		KG_PROCESS_ERROR(pFarmount);
		{
			int nDigit = 0;
			int nDigitToAdd = -1;
			KG3DMeshFarMountainPtrVector::iterator it;
			for ( it = m_vecFarMounts.begin(); it != m_vecFarMounts.end(); it++ )///得到最大的FarMount的名字编号
			{
				LPTSTR pTemp = (*it)->Name();
				pTemp += (int)sizeof("FarMount")-1;///小心字符后面有个\n
				nDigit = _ttoi( pTemp );///得到名字究竟是FarMount几
				nDigitToAdd = max( nDigitToAdd, nDigit );
			}
			_stprintf_s( pFarmount->Name(), 32, "FarMount%d", nDigitToAdd+1 );
			m_vecFarMounts.push_back( pFarmount );
		}
		return S_OK;
Exit0:
		return E_FAIL;
	}
	HRESULT KG3DSkySystem::DelFarMount( int nIndex )
	{
		KG3DMeshFarMountainPtrVector::iterator it = m_vecFarMounts.begin();
		KGLOG_PROCESS_ERROR( nIndex >= 0 && nIndex < (int)m_vecFarMounts.size() );
		KG3DMeshFarMountain* pFarMount = *(it+nIndex);
		delete pFarMount;
		m_vecFarMounts.erase( it+nIndex );

		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DSkySystem::DelFarMount( KG3DMeshFarMountain* pFarMount )
	{
		KG_PROCESS_ERROR(pFarMount);
		int nIndex = GetFarMountIndexByPointer( pFarMount );
		if ( nIndex >= 0 && nIndex < (int)(m_vecFarMounts.size()) )
		{
			return DelFarMount( nIndex );
		}
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DSkySystem::UpperFarMount( int nIndex )
	{
		KG3DMeshFarMountainPtrVector::iterator it = m_vecFarMounts.begin();
		KGLOG_PROCESS_ERROR( nIndex >= 0 && nIndex < (int)m_vecFarMounts.size() );
		KGLOG_PROCESS_SUCCESS( 0 == nIndex );	
		it += nIndex;
		KG3DMeshFarMountain* pMount = (KG3DMeshFarMountain*)(*(it-1));
		*(it-1) = *it;
		*it = pMount;

Exit1:
		return S_OK;
Exit0:
		return E_FAIL;
	}
	HRESULT KG3DSkySystem::LowerFarMount( int nIndex )
	{
		if ( nIndex == (int)(m_vecFarMounts.size())-1 )
		{
			return S_OK;
		}
		return UpperFarMount( nIndex + 1 );
	}
	KG3DMeshFarMountain* KG3DSkySystem::GetFarMount( int nIndex )
	{
		KG_PROCESS_ERROR( nIndex >= 0 && nIndex < (int)m_vecFarMounts.size() );
		_ASSERTE( (KG3DMeshFarMountain*)m_vecFarMounts[nIndex] );
		return (KG3DMeshFarMountain*)m_vecFarMounts[nIndex];
Exit0:
		return NULL;
	}


	HRESULT KG3DSkySystem::ClearFarMountVec()
	{
		KG3DMeshFarMountainPtrVector::iterator it2;
		KG3DMeshFarMountain* pMount = NULL; 
		for( it2 = m_vecFarMounts.begin(); it2 != m_vecFarMounts.end(); it2++ )
		{
			pMount = (KG3DMeshFarMountain*)(*it2);
			_ASSERTE(pMount);
			SAFE_DELETE(pMount);
		}
		m_vecFarMounts.clear();
		return S_OK;
	}
	int KG3DSkySystem::GetFarMountIndexByPointer( KG3DMeshFarMountain* pFarMout )
	{
		KG3DMeshFarMountainPtrVector::iterator itResult;
		KG_PROCESS_ERROR(pFarMout);
		itResult = std::find( m_vecFarMounts.begin(), m_vecFarMounts.end(), pFarMout );
		if ( itResult != m_vecFarMounts.end() )
		{
			int nIndexFound = (int)(itResult-m_vecFarMounts.begin());
			_ASSERTE( nIndexFound >=0 && nIndexFound < (int)m_vecFarMounts.size() );
			return nIndexFound;
		}
Exit0:
		return -1;
	}
	int KG3DSkySystem::GetFarMountVecSize()
	{
		return (int)m_vecFarMounts.size();
	}

	BOOL KG3DSkySystem::IsFarMountPointerValid( KG3DMeshFarMountain* pFarMout )
	{
		if ( NULL == pFarMout )
		{
			return FALSE;
		}
		if ( m_vecFarMounts.end() != std::find( m_vecFarMounts.begin(), m_vecFarMounts.end(), pFarMout ) )
		{
			return TRUE;
		}
		return FALSE;
	}


	/************************************************************************/
	//远山接口定义开始                                                                      
	/************************************************************************/
	HRESULT KG3DSkySystem::AddFarMount( const KG3DMESHFARMOUNTAIN_DESCSTRUCT* pDescStruct, IEKG3DMeshFarMountain** ppAddedFarmount )///可以用NULL
	{
		KG3DMeshFarMountain* p = NULL;
		HRESULT hr = AddFarMount( (KG3DMeshFarMountDescStruct*)pDescStruct, &p);
		if (SUCCEEDED(hr) && NULL != ppAddedFarmount)
		{
			*ppAddedFarmount = p;
		}
		return hr;
	}

	HRESULT KG3DSkySystem::DelFarMount( IEKG3DMeshFarMountain* pFarMount )
	{
		if(NULL == pFarMount)
			return E_FAIL;
		return DelFarMount( (KG3DMeshFarMountain*)pFarMount );
	}
	HRESULT KG3DSkySystem::GetFarMount( int nIndex, IEKG3DMeshFarMountain** ppFarMount )
	{
		KG3DMeshFarMountain* pFarMount = GetFarMount( nIndex );
		if ( ppFarMount != NULL&& pFarMount != NULL )
		{
			*ppFarMount = (IEKG3DMeshFarMountain*)pFarMount;
			_ASSERTE(*ppFarMount);
			return S_OK;
		}
		return E_FAIL;
	}
	HRESULT KG3DSkySystem::GetFarMountIndexByPointer( IEKG3DMeshFarMountain* pFarMout, int* pIndex )
	{
		KG_PROCESS_ERROR(pIndex);
		KG_PROCESS_ERROR(pFarMout);
		int nIndex = GetFarMountIndexByPointer( (KG3DMeshFarMountain*)pFarMout );	
		if ( -1 != nIndex )
		{
			_ASSERTE( nIndex >=0 && nIndex < (int)m_vecFarMounts.size() );
			*pIndex = nIndex;
			return S_OK;
		}
		*pIndex = -1;
Exit0:
		return E_FAIL;
	}
	HRESULT KG3DSkySystem::IsFarMountPointerValid( IEKG3DMeshFarMountain* pFarMout, BOOL* pBool )
	{
		KG_PROCESS_ERROR( NULL != pFarMout && NULL != pBool );
		BOOL bResult = IsFarMountPointerValid( (KG3DMeshFarMountain*)pFarMout );
		if ( TRUE == bResult)
		{
			*pBool = bResult;
			return S_OK;
		}
		*pBool = FALSE;
Exit0:
		return E_FAIL;
	}
	HRESULT KG3DSkySystem::GetFarMountVecSize( int* pSize )
	{
		if(NULL == pSize)
			return E_FAIL;
		*pSize = GetFarMountVecSize();
		return S_OK;
	}
	HRESULT KG3DSkySystem::AddSkyBox( IEKG3DMeshSkyBox** ppiMeshSkyBox )
	{
		KG3DMeshSkyBox* pSkyBox = AddSkyBox(NULL, NULL);
		if (ppiMeshSkyBox)
		{
			*ppiMeshSkyBox = pSkyBox;
		}
		return pSkyBox != NULL ? S_OK : E_FAIL;
	}
	HRESULT KG3DSkySystem::DelSkyBox()
	{
		SAFE_DELETE(m_lpSkyBox);
		return S_OK;
	}
	KG3DMeshSkyBox* KG3DSkySystem::AddSkyBox(IIniFile* pFile, LPCTSTR strSection)
	{
		if (m_lpSkyBox)///重复添加是不允许的
		{
			return NULL;
		}

		HRESULT hr = KG3DMeshSkyBox::CreateInstance(pFile, strSection, (LPVOID*)&m_lpSkyBox);
		KGLOG_COM_PROCESS_ERROR(hr);

		return m_lpSkyBox;
Exit0:
		//SAFE_DELETE(pTempSkyBox);
		return NULL;
	}
	HRESULT KG3DSkySystem::GetSkyBox(IEKG3DMeshSkyBox** ppiMeshSkyBox)
	{
		HRESULT hResult = E_FAIL;
		KG3DMeshSkyBox *pMeshBox = NULL;

		KGLOG_PROCESS_ERROR(ppiMeshSkyBox);

		pMeshBox = GetSkyBox();

		*ppiMeshSkyBox = dynamic_cast<IEKG3DMeshSkyBox *>(pMeshBox);
		KGLOG_PROCESS_ERROR(*ppiMeshSkyBox);

		hResult = S_OK;
Exit0:
		return hResult;
	}

	HRESULT KG3DSkySystem::ToSolidInterface( LPVOID* pEffectInterface )
	{
		if (!pEffectInterface)
		{
			return E_FAIL;
		}
		IEKG3DSkySystem* piSkySystem = this;
		*pEffectInterface = (LPVOID)piSkySystem;
		return S_OK;
	}

	

	KG3DEnvEffect* KG3DSkySystem::GetSubEffect( DWORD dwType, INT nIndex )
	{
		///<DW_SUBFrame>
		switch(dwType)
		{
		case KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX:///<DW_SUB_Frame>
			return GetSkyBox();
			break;
		case KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN:
			return GetFarMount(nIndex);
		    break;
		case KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD:
			return GetCloud(nIndex);
		default:
			_KG3D_DEBUG_SELF_REPORT(_STRINGER(KG3DSkySystem)":"_STRINGER(GetSubEffect)":错误的类型请求");
		    break;
		}
		return NULL;
	}

	KG3DEnvEffect* KG3DSkySystem::GetSubEffect( INT nIndex, KG3DENVEFF_HANDLE* pHandle /*NULL*/)
	{
		///<DW_SUBFrame>
		INT nRest = nIndex;
		KG3DEnvEffect* pEnvEffect = NULL, *pTemp = NULL;
		KG_PROCESS_ERROR(nIndex>=0);
		pTemp = GetSkyBox();
		INT nSkyBoxCount = pTemp ? 1 : 0;
		if (nRest < nSkyBoxCount)
		{	
			pEnvEffect = pTemp;
			goto Exit1;
		}
		else
		{
			nRest -=nSkyBoxCount;
		}
		INT nFarmountCount = GetFarMountVecSize();
		if (nRest < nFarmountCount)
		{
			pEnvEffect = GetFarMount(nRest);
			goto Exit1;
		}
		else
		{
			nRest -= nFarmountCount;
		}
		INT nCloudCount = GetCloudVecSize();
		if(nRest < nCloudCount)
		{
			pEnvEffect = GetCloud(nRest);
			goto Exit1;
		}
		
Exit1:
		if (pEnvEffect && pHandle)
		{
			pHandle->m_dwType = pEnvEffect->GetType();
			pHandle->m_nIndex = nRest;
			_ASSERTE(pHandle->m_dwType != KG3DTYPE_ENVEFF_INVALID_EFFECT);
#if defined(DEBUG) | defined(_DEBUG)
			_ASSERTE(nRest >= 0);
			if (pHandle->m_dwType == KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN)
			{
				_ASSERTE(nRest < GetFarMountVecSize());
			}
			if(pHandle->m_dwType == KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD)
			{
				_ASSERTE(nRest < GetCloudVecSize());
			}
#endif	
		}
Exit0:
		return pEnvEffect;
	}
	INT KG3DSkySystem::GetSubEffectCount()
	{
		///<DW_SUBFrame>
		int nCount = GetSkyBox() ? 1 : 0;
		return nCount + GetFarMountVecSize() + GetCloudVecSize();
	}

	INT KG3DSkySystem::GetSubEffectCount( DWORD dwSubType )
	{
		INT nRet = 0;
		///<DW_SUBFrame>
		switch(dwSubType)
		{
		case KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX:
			nRet = GetSkyBox() ? 1:0;
			break;
		case KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN:
			nRet = GetFarMountVecSize();
		    break;
		case KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD:
			nRet = GetCloudVecSize();
		default:
		    break;
		}
		return nRet;
	}
	VOID KG3DSkySystem::GetSubEffectTypeList( std::vector<DWORD>& Types )
	{
		///<DW_SUBFrame>
		Types.push_back(KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX);
		Types.push_back(KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN);
		Types.push_back(KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD);
	}
	VOID KG3DSkySystem::GetExistSubEffectTypeList( std::vector<DWORD>& Types )
	{
		///<DW_SUBFrame>
		if(GetSkyBox())
			Types.push_back(KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX);
		if (GetFarMountVecSize() > 0)
			Types.push_back(KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN);
		if(GetCloudVecSize() > 0)
			Types.push_back(KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD);
	}
	HRESULT KG3DSkySystem::AddEnvEffect(DWORD dwType, KG3DEnvEffect** ppEnvEffect)
	{
		KG3DEnvEffect* pEnvEffect = NULL;
		switch(dwType)
		{
		case KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX:///<DW_SUB_Frame>
			{
				pEnvEffect = AddSkyBox(NULL, NULL);
				KG_PROCESS_ERROR(pEnvEffect);
			}
			break;
		case KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN:
			{
#if defined(_DEBUG) | defined(DEBUG)
				int nCout = GetFarMountVecSize();
#endif	
				KG_COM_PROCESS_ERROR(AddFarMount((KG3DMeshFarMountDescStruct*)NULL, NULL));

				int nCoutNew = GetFarMountVecSize();
				_ASSERTE(nCoutNew == nCout+1);
				
				pEnvEffect = GetFarMount(nCoutNew -1 );
			}
			break;
		case KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD:
			{
				HRESULT hr = this->AddCloud();
				KG_COM_PROCESS_ERROR(hr);
			}
		default:
			_KG3D_DEBUG_SELF_REPORT(_STRINGER(KG3DSkySystem)":"_STRINGER(GetSubEffect)":错误的类型请求");
			goto Exit0;
			break;
		}
		KG_PROCESS_ERROR(pEnvEffect);
		if (ppEnvEffect)
		{
			*ppEnvEffect = pEnvEffect;
		}
		return S_OK;
Exit0:
		return E_FAIL;
	}
	
	
	/************************************************************************/
	/*其它的辅助类 
	*/
	/************************************************************************/


//	bool KG3DDWGC_Key_SkySystem::CalculateInterp(const KG3DDWGC_KeyBase& BaseKeyBefore
//		, const KG3DDWGC_KeyBase& BaseKeyAfter
//		//, const KG3DEnvEff_DWGC_Key_Struct_Base& BaseKeyResult
//		, float fPercent
//		, KG3DDWGC_KeyTableManagerBase* pTableManager /* = NULL*/)
//	{
//		///<DW_SUB_Frame>
//		KG3DDWGC_Key_SkySystem& KeyBefore = (KG3DDWGC_Key_SkySystem&)BaseKeyBefore;
//		KG3DDWGC_Key_SkySystem& KeyAfter = (KG3DDWGC_Key_SkySystem&)BaseKeyAfter;
//		
//		_ASSERTE(pTableManager);
//		///如果是一头一尾的话，就不用创建新的插值号了
//		if (fPercent < FLT_MIN)
//		{
//			*this = KeyBefore;
//			return true;
//		}
//		else if (fPercent >= 1.0f)
//		{
//			*this = KeyAfter;
//			return true;
//		}
//		///SkyBox的部分/////////////////////////////////////////////////////////////////////////////////////////
//		CalculateSkyBox(*pTableManager, KeyBefore, KeyAfter, fPercent);
//
//		///FarMout的部分/////////////////////////////////////////////////////////////////////////////////////////
//		CalculateFarmount(*pTableManager, KeyBefore, KeyAfter, fPercent);
//		
//		return true;
///*
//Exit0:
//		return true;*/
//
//
//	}
//	bool KG3DDWGC_Key_SkySystem::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
//	{
//		///如果是Invalid的话，当作没有关键帧处理，但不是非法
//		_ASSERTE(pIniFile&&pSection);//<DW_SUB_Frame>
//
//		KG_CUSTOM_HELPERS::EraseSection(pIniFile, pSection);
//
//		pIniFile->WriteInteger(pSection, _T("DWGC_KEY_INDEX_SKYBOX"), m_nSkyBoxIndex);
//		char szFarmountIndexKey[MAX_KEYNAME_SIZE];
//		int k = 0;
//		for (FARMOUNT_INDEX_ARRAY::iterator it = m_FarMountIndexArray.begin(); it != m_FarMountIndexArray.end(); it++, k++)
//		{
//			_stprintf_s(szFarmountIndexKey, MAX_KEYNAME_SIZE, "%s_%d", _T("DWGC_KEY_INDEX_FARMOUNT"), k );
//			pIniFile->WriteInteger(pSection, szFarmountIndexKey, (*it) );
//		}
//		
//		return true;
//	}
//
//	bool KG3DDWGC_Key_SkySystem::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
//	{
//		///如果是Invalid的话，当作没有关键帧处理，但不是非法
//		///<DW_SUB_Frame>
//		_ASSERTE(pIniFile&&pSection);
//		int nTempSkyBoxIndex = -1;
//		pIniFile->GetInteger(pSection, _T("DWGC_KEY_INDEX_SKYBOX"), -1, &nTempSkyBoxIndex);
//		KG_PROCESS_ERROR(nTempSkyBoxIndex != -1);
//		m_nSkyBoxIndex = nTempSkyBoxIndex;
//		
//		m_FarMountIndexArray.clear();
//		char szFarMountIndexKey[MAX_KEYNAME_SIZE];
//		for (int k = 0 ; k < MAX_FARMOUNT; k++)
//		{
//			int nIndex = -1;
//			_stprintf_s(szFarMountIndexKey, MAX_KEYNAME_SIZE, "%s_%d", _T("DWGC_KEY_INDEX_FARMOUNT"), k );
//			if (!(pIniFile->GetInteger(pSection, szFarMountIndexKey, -1, &nIndex)))
//			{
//				break;///找不到下一个了，弹出
//			}
//			KG_PROCESS_ERROR(nIndex != -1);				
//			m_FarMountIndexArray.push_back(nIndex);
//		}
//		
//		return true;
//Exit0:
//		return false;
//	}
//#if defined(DEBUG) | defined(_DEBUG)
//	void KG3DDWGC_Key_SkySystem::DebugPrint(std::ostringstream& OSS)
//	{
//		///<DW_SUB_Frame>
//		/*
//			_ASSERTE(pDebugStream);
//					char czTemp[32];
//					sprintf_s(czTemp, 32, "SkyBox:%d; ", m_nSkyBoxIndex);
//					_tcscat_s(pDebugStream, nSize, czTemp);
//					nSize-=(int)strlen(czTemp);
//					if (!m_FarMountIndexArray.empty())
//					{
//						sprintf_s(czTemp, 32, "Farmount1:%d; ", m_FarMountIndexArray[0]);
//						_tcscat_s(pDebugStream, nSize, czTemp);
//						nSize-=(int)strlen(czTemp);
//			
//		}*/
//		OSS << "SB: " << m_nSkyBoxIndex << "FNum: " << m_FarMountIndexArray.size();
//		if (! m_FarMountIndexArray.empty())
//			OSS << "; Fs1: " << m_FarMountIndexArray[0];
//	}
//#endif
//	///<DW_SUB_Frame>
//	KG3DDWGC_Key_SkySystem::KG3DDWGC_Key_SkySystem()
//		:m_nSkyBoxIndex(DW_KEYINDEX_INVALID)
//	{
//		
//	}
//	KG3DDWGC_Key_SkySystem::KG3DDWGC_Key_SkySystem(const KG3DDWGC_Key_SkySystem& OtherStruct)
//		:m_nSkyBoxIndex(OtherStruct.m_nSkyBoxIndex)///<DW_SUB_Frame>
//		,m_FarMountIndexArray(OtherStruct.m_FarMountIndexArray)
//	{
//
//	}
//	KG3DDWGC_Key_SkySystem& KG3DDWGC_Key_SkySystem::operator=(const KG3DDWGC_Key_SkySystem& OtherStruct)
//	{
//		///不知道这样行不行///<DW_SUB_Frame>
//		//memcpy_s(this, sizeof(DWGC_KG3DSkySystem_Key_Struct), &OtherStruct, sizeof(DWGC_KG3DSkySystem_Key_Struct));
//		m_nSkyBoxIndex = OtherStruct.m_nSkyBoxIndex;
//		m_FarMountIndexArray = OtherStruct.m_FarMountIndexArray;
//		return *this;
//	}
	
	/*void KG3DDWGC_Key_SkySystem::ReleaseContainedKey(KG3DDWGC_KeyTableManagerBase* pTableManager, bool OnlyInterp)///<DW_SUB_Frame>
	{
		_ASSERTE(pTableManager);
		
		if (OnlyInterp && !CheckIsIndexInterp(m_nSkyBoxIndex))
		{

		}
		else
		{
			pTableManager->GetTableByType(KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX).ReleaseKeyAndContainedKey(m_nSkyBoxIndex, OnlyInterp);
		}

		for (FARMOUNT_INDEX_ARRAY::iterator it = m_FarMountIndexArray.begin()
			; it != m_FarMountIndexArray.end()
			; it++)
		{
			INT nIndex = *it;
			if (OnlyInterp && !CheckIsIndexInterp(nIndex))
				continue;
			pTableManager->GetTableByType(KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN).ReleaseKeyAndContainedKey(nIndex, OnlyInterp);
		}
		
	}*/

	/*KG3DDWGC_KeyBase* KG3DDWGC_Key_SkySystem::GetContainedKey(KG3DDWGC_KeyTableManagerBase& RefTableMgr
																, DWORD dwType
																, int nFinded) const 
	{
		switch(dwType)
		{
		case KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX:///<DW_SUB_Frame>
			{
				return RefTableMgr.GetTableByType(KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX).GetKey(m_nSkyBoxIndex);
			}
			break;
		case KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN:
			{
				if ((int)m_FarMountIndexArray.size() > nFinded && nFinded >= 0)
				{
					return RefTableMgr.GetTableByType(KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN).GetKey(m_FarMountIndexArray[nFinded]);
				}
			}
			break;
		}
		return NULL;
	}*/
	//INT KG3DDWGC_Key_SkySystem::GetContainedKeyIndex( KG3DDWGC_KeyTableManagerBase& RefTableMgr, DWORD dwType, int nFinded) const
	//{
	//	///<DW_Sub_Frame>
	//	switch(dwType)
	//	{
	//	case KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX:
	//		{
	//			return m_nSkyBoxIndex;
	//		}
	//		break;
	//	case KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN:
	//		{
	//			if (nFinded >=0 && nFinded < (int)m_FarMountIndexArray.size())
	//			{
	//				return m_FarMountIndexArray[nFinded];
	//			}		
	//		}
	//	    break;
	//	default:
	//	    break;
	//	}
	//	return DW_KEYINDEX_INVALID;
	//}

	//int KG3DDWGC_Key_SkySystem::GetContainedKeyCount() const
	//{
	//	///<DW_SUB_Frame>
	//	return 1 + (int)m_FarMountIndexArray.size();//是SkyBox
	//}
	/*bool KG3DDWGC_Key_SkySystem::SetContainedKeyIndex( KG3DDWGC_KeyTableManagerBase& RefTableMgr, DWORD dwType, int nFinded, INT nNewIndex )
	{
		///<DW_Sub_Frame>
		switch(dwType)
		{
		case KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX:
			
			if (nNewIndex== DW_KEYINDEX_INVALID 
				|| RefTableMgr.GetTableByType(KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX).Find(nNewIndex))
			{
				m_nSkyBoxIndex = nNewIndex;
				return true;
			}		
			
			break;
		case KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN:
			if(nNewIndex== DW_KEYINDEX_INVALID 
				|| RefTableMgr.GetTableByType(KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN).Find(nNewIndex))
			{
				size_t uCurFarmountIndexArraySize = m_FarMountIndexArray.size();
				if (nFinded >= 0 && nFinded < (INT)uCurFarmountIndexArraySize)
				{
					m_FarMountIndexArray[nFinded] = nNewIndex;
					return true;
				}
				else if(nFinded >= 0 && nFinded < MAX_FARMOUNT)
				{
					for (int i = (int)uCurFarmountIndexArraySize; i < nFinded; i++)
					{
						m_FarMountIndexArray.push_back(DW_KEYINDEX_INVALID);
					}
					m_FarMountIndexArray.push_back(nNewIndex);
					_ASSERTE(m_FarMountIndexArray.size() <= MAX_FARMOUNT);
					return true;
				}
			}
						
			
			break;
		default:
			break;
		}
		return false;
	}*/
	/*void KG3DDWGC_Key_SkySystem::CalculateSkyBox(KG3DDWGC_KeyTableManagerBase& RefTableMgr
												, KG3DDWGC_Key_SkySystem &KeyBefore
												, KG3DDWGC_Key_SkySystem &KeyAfter
												, float fPercent )
	{
		int nSkyBoxBeforeIndex = KeyBefore.m_nSkyBoxIndex;
		int nSkyBoxAfterIndex = KeyAfter.m_nSkyBoxIndex;
		if (nSkyBoxBeforeIndex == nSkyBoxAfterIndex)
		{
			this->m_nSkyBoxIndex = nSkyBoxBeforeIndex;
		}
		else
		{
			int nInterpIndex = DW_KEYINDEX_INVALID;
			KG3DDWGC_KeyTableBase0& SkyBoxTable = RefTableMgr.GetTableByType(KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX);
			KG3DDWGC_KeyBase& SkyBoxKeyResult = SkyBoxTable.GetInterpKey(nInterpIndex);///可以请求更多的Interp
			if(true != CheckIsIndexValid(nInterpIndex))
			{
				_ASSERTE(NULL&&"取插值Key失败");
				goto Exit0;
			}
			KG3DDWGC_KeyBase* pSkyBoxBefore = SkyBoxTable.GetKey(nSkyBoxBeforeIndex);
			KG3DDWGC_KeyBase* pSkyBoxAfter = SkyBoxTable.GetKey(nSkyBoxAfterIndex);
			KG_PROCESS_ERROR(pSkyBoxBefore && pSkyBoxAfter);
			this->m_nSkyBoxIndex = nInterpIndex;
			SkyBoxKeyResult.CalculateInterp(*pSkyBoxBefore, *pSkyBoxAfter, fPercent);
		}
		return;
Exit0:
		m_nSkyBoxIndex = DW_KEYINDEX_INVALID;
	}

	void KG3DDWGC_Key_SkySystem::CalculateFarmount(KG3DDWGC_KeyTableManagerBase& RefTableMgr
											, KG3DDWGC_Key_SkySystem& KeyBefore
											, KG3DDWGC_Key_SkySystem& KeyAfter
											, float fPercent )
	{
		FARMOUNT_INDEX_ARRAY& FM_ArrayBefore = KeyBefore.m_FarMountIndexArray;
		FARMOUNT_INDEX_ARRAY& FM_ArrayAfter = KeyAfter.m_FarMountIndexArray;
		FARMOUNT_INDEX_ARRAY& FM_ArrayResult = this->m_FarMountIndexArray;

		if (FM_ArrayBefore.empty())
		{
			FM_ArrayResult = FM_ArrayAfter;
			return;
		}
		else if (FM_ArrayAfter.empty())
		{
			FM_ArrayResult = FM_ArrayBefore;
			return;
		}
		else
		{
			BalanceTwoVectors(FM_ArrayBefore, FM_ArrayAfter, MAX_FARMOUNT);
			FARMOUNT_INDEX_ARRAY::iterator itBefore = FM_ArrayBefore.begin();
			FARMOUNT_INDEX_ARRAY::iterator itAfter = FM_ArrayAfter.begin();
			KG3DDWGC_KeyTableBase0& FarMountTable = RefTableMgr.GetTableByType(KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN);
			m_FarMountIndexArray.clear();
			while (itBefore != FM_ArrayBefore.end())
			{
				int nBeforeIndex = *itBefore;
				int nAfterIndex = *itAfter;
				int nInterpIndex = DW_KEYINDEX_INVALID;
				KG3DDWGC_KeyBase& FarmountResult = FarMountTable.GetInterpKey(nInterpIndex);
				if(true != CheckIsIndexValid(nInterpIndex))
				{
					_ASSERTE(NULL&&"取插值Key失败");
					goto Exit0;
				}
				KG3DDWGC_KeyBase* pFarmountBefore = FarMountTable.GetKey(nBeforeIndex);
				KG3DDWGC_KeyBase* pFarmountAfter = FarMountTable.GetKey(nAfterIndex);
				KG_PROCESS_ERROR(pFarmountBefore && pFarmountAfter);
				m_FarMountIndexArray.push_back(nInterpIndex);
				FarmountResult.CalculateInterp(*pFarmountBefore, *pFarmountAfter, fPercent);
				itBefore++;
				itAfter++;
			}
		}
		_ASSERTE(FM_ArrayBefore.size() == FM_ArrayAfter.size());
		
		return;
Exit0:
		m_FarMountIndexArray.clear();
	}*/

	/*bool KG3DDWGC_Key_SkySystem::IsLegal(KG3DDWGC_KeyTableManagerBase* pTableManager)const
	{
		///<DW_SUB_Frame>
		KG_PROCESS_ERROR(pTableManager->GetTableByType(KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX).Find(m_nSkyBoxIndex));
		for (int j = 0; j < static_cast<int>(m_FarMountIndexArray.size()); j++)
		{
			KG_PROCESS_ERROR(pTableManager->GetTableByType(KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN).Find(m_FarMountIndexArray[j]));
		}
		
		return true;
Exit0:
		return false;
	}*/

	
	/*std::pair<DWORD, int> KG3DDWGC_Key_SkySystem::GetContainedKeyIndexByOrder( int nOrder ) const
	{
		///<DW_SUB_Frame>
		size_t uRest = (size_t)nOrder;
		KG_PROCESS_ERROR(nOrder >=0);
		if (uRest == 0)
		{
			return std::pair<DWORD, int>(KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX, m_nSkyBoxIndex);
		}
		else
		{
			uRest -= 1;
		}
		if (uRest < m_FarMountIndexArray.size())
		{
			return std::pair<DWORD, int>(KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN, m_FarMountIndexArray[uRest]);
		}
		else
		{
			uRest -= m_FarMountIndexArray.size();
		}
		///<云>
Exit0:
		return std::pair<DWORD, int>(KG3DTYPE_ENVEFF_INVALID_EFFECT, -1);
	}*/
	/*bool KG3DDWGC_Key_SkySystem::SetContainedKeyIndexByOrder( KG3DDWGC_KeyTableManagerBase& RefTableMgr, int nOrder, INT nNewIndex )
	{
		///<DW_SUB_Frame>
		size_t uRest = (size_t)nOrder;
		INT* pTargetIndex = NULL;
		DWORD dwIndexType = KG3DTYPE_ENVEFF_INVALID_EFFECT;
		KG_PROCESS_ERROR(nOrder >=0);
		if (uRest == 0)
		{
			pTargetIndex = &m_nSkyBoxIndex;
			dwIndexType = KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX;
			goto Exit1;
		}
		else
		{
			uRest -= 1;
		}
		if (uRest < m_FarMountIndexArray.size())
		{
			pTargetIndex = &m_FarMountIndexArray[uRest];
			dwIndexType = KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN;
			goto Exit1;
		}
		else
		{
			uRest -= m_FarMountIndexArray.size();
		}
Exit1:
		KG_PROCESS_ERROR(pTargetIndex);
		_ASSERTE(dwIndexType == KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX || dwIndexType == KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN);
		KG_PROCESS_ERROR(nNewIndex == DW_KEYINDEX_INVALID 
			 || RefTableMgr.GetTableByType(dwIndexType).Find(nNewIndex));
		*pTargetIndex = nNewIndex;
		return true;
Exit0:
		return false;
	}*/

	/*void KG3DDWGC_Key_SkySystem::CopyLegalPartsOfAnotherKey(const KG3DDWGC_KeyBase& OtherKeyBase, KG3DDWGC_KeyTableManagerBase& TableManager )
	{
		const KG3DDWGC_Key_SkySystem& OtherKey = dynamic_cast<const KG3DDWGC_Key_SkySystem&>(OtherKeyBase);
		_ASSERTE(&OtherKey);
		_ASSERTE(this != (&OtherKey));
		_ASSERTE(OtherKey.IsLegal(&TableManager));
		if (! TableManager.GetTableByType(KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX).Find(m_nSkyBoxIndex))
		{
			m_nSkyBoxIndex = OtherKey.m_nSkyBoxIndex;
		}
		///因为FarmountVec是变长的，要提防OtherKey的Vec长度不匹配的情况
		KG3DDWGC_KeyTableBase0& FarMountTable = TableManager.GetTableByType(KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN);
		size_t uOtherFMVecSize = OtherKey.m_FarMountIndexArray.size();
		if (uOtherFMVecSize == 0)
		{
			FARMOUNT_INDEX_ARRAY ValidArray;
			ValidArray.reserve(m_FarMountIndexArray.size());
			for (size_t i = 0; i < m_FarMountIndexArray.size(); i++)
			{
				if (FarMountTable.Find(m_FarMountIndexArray[i]))
				{
					ValidArray.push_back(m_FarMountIndexArray[i]);
				}
			}
			m_FarMountIndexArray = ValidArray;
		}
		else
		{
			for (size_t i = 0; i < m_FarMountIndexArray.size(); i++)
			{
				if (! FarMountTable.Find(m_FarMountIndexArray[i]))
				{
					if(i < uOtherFMVecSize)
						m_FarMountIndexArray[i] = OtherKey.m_FarMountIndexArray[i];
					else
						m_FarMountIndexArray[i] = OtherKey.m_FarMountIndexArray[uOtherFMVecSize - 1];
				}
			}
			///ValidKey的多出来的部分填入这里
			if (uOtherFMVecSize > m_FarMountIndexArray.size())
			{
				for (size_t i = m_FarMountIndexArray.size(); i < uOtherFMVecSize; i++)
				{
					m_FarMountIndexArray.push_back(OtherKey.m_FarMountIndexArray[i]);
				} 
			}
		}
	}*/
	
};