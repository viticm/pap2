
#pragma once

#include "KG3DTypes.h"

template <class TResource>
class TKG3DMultiTheadBuffer
{
private:
	TResource* m_cMultiTheadList[2];
protected:
public:
	HRESULT Init()
	{
		KGLOG_PROCESS_ERROR(!m_cMultiTheadList[0]);
		KGLOG_PROCESS_ERROR(!m_cMultiTheadList[1]);

		m_cMultiTheadList[0] = new TResource;
		KGLOG_PROCESS_ERROR(m_cMultiTheadList[0]);

		m_cMultiTheadList[1] = new TResource;
		KGLOG_PROCESS_ERROR(m_cMultiTheadList[1]);

		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT FindList_NoData(TResource*& pList,TResource*& pListNext)
	{

		if(m_cMultiTheadList[0]->m_dwState == BLOCK_LIST_STATE_NODATA)
		{
			pList = m_cMultiTheadList[0];
			pListNext = m_cMultiTheadList[1];
		}
		else if (m_cMultiTheadList[1]->m_dwState == BLOCK_LIST_STATE_NODATA)
		{
			pList = m_cMultiTheadList[1];
			pListNext = m_cMultiTheadList[0];
		}
		else
		{
			return E_FAIL;
		}
		return S_OK;
	}

	HRESULT FindList_DataOK(TResource*& pList,TResource*& pListNext)
	{

		if(m_cMultiTheadList[0]->m_dwState == BLOCK_LIST_STATE_DATAOK)
		{
			pList = m_cMultiTheadList[0];
			pListNext = m_cMultiTheadList[1];
		}
		else if (m_cMultiTheadList[1]->m_dwState == BLOCK_LIST_STATE_DATAOK)
		{
			pList = m_cMultiTheadList[1];
			pListNext = m_cMultiTheadList[0];
		}
		else
		{
			return E_FAIL;
		}
		return S_OK;
	}

	HRESULT Finished()
	{
		if (m_cMultiTheadList[0]->m_dwState == BLOCK_LIST_STATE_NODATA &&
			m_cMultiTheadList[1]->m_dwState == BLOCK_LIST_STATE_NODATA)
		{
			return S_OK;
		}
		return E_FAIL;
	}

	TKG3DMultiTheadBuffer()
	{
		m_cMultiTheadList[0] = NULL;
		m_cMultiTheadList[1] = NULL;
	}	

	~TKG3DMultiTheadBuffer()
	{
		SAFE_DELETE(m_cMultiTheadList[0]);
		SAFE_DELETE(m_cMultiTheadList[1]);
	}

	HRESULT ForceGetResource(TResource*& pList,TResource*& pListNext)
	{
		pList = m_cMultiTheadList[0];
		pListNext = m_cMultiTheadList[1];
		return S_OK;
	}
};;;

#define  MAX_EXCHANGE_RESOURCE  1024

template <class TResource>
class TKG3DMultiTheadExchanger
{
public:
	TResource* m_cResource[MAX_EXCHANGE_RESOURCE];
	TResource* m_lpCurrentResource;

	TKG3DMultiTheadExchanger()
	{
		ZeroMemory(m_cResource,sizeof(m_cResource));
		m_lpCurrentResource = NULL;
	}

	~TKG3DMultiTheadExchanger()
	{
		UnInit();
	}

	HRESULT Init()
	{
		return S_OK;
	}

	HRESULT UnInit()
	{
		for (int i=0;i<MAX_EXCHANGE_RESOURCE;i++)
		{
			SAFE_RELEASE(m_cResource[i]);
		}
		return S_OK;
	}

	HRESULT InsertResource(TResource* pResource)
	{
		for (int i=0;i<MAX_EXCHANGE_RESOURCE;i++)
		{
			if(m_cResource[i] == NULL)
			{
				pResource->AddRef();
				m_cResource[i] = pResource;
				return S_OK;
			}
		}

		KGLogPrintf(KGLOG_ERR, "MultiThread list full!\n");
		return E_MUTL_LOAD_ARRAY_FULL;
	}

	HRESULT RemoveResource(TResource* pResource)
	{

		for (int i=0;i<MAX_EXCHANGE_RESOURCE;i++)
		{
			if(m_cResource[i] == pResource)
			{
				SAFE_RELEASE(m_cResource[i]);
				return S_OK;
			}
		}

		KGLogPrintf(KGLOG_ERR, "MultiThread list Remove Failed!\n");
		return E_FAIL;
	}

	HRESULT TryMultiThreadLoad(TResource* pResource,BOOL& bPause)
	{
		HRESULT hr = E_FAIL;
		KGLOG_PROCESS_ERROR(pResource);
		KGLOG_PROCESS_ERROR(!pResource->m_bInMultiThreadLoadList);

		if(bPause)
		{
			hr = pResource->LoadFromFileMultiThread();
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		else
		{
			pResource->m_bInMultiThreadLoadList = TRUE;
			hr = InsertResource(pResource);
			KGLOG_COM_PROCESS_ERROR(hr);
		}

		hr = S_OK;
Exit0:
		return hr;
	}

	HRESULT StopMultiThreadLoad(TResource* pResource)
	{
		HRESULT hr = E_FAIL;
		KGLOG_PROCESS_ERROR(pResource);
		KGLOG_PROCESS_ERROR(pResource->m_bInMultiThreadLoadList);

		while (pResource == m_lpCurrentResource)//如果该模型正在被加载，等待其加载结束再说
		{
			Sleep(10);
		}

		hr = RemoveResource(pResource);
		KGLOG_COM_PROCESS_ERROR(hr);
		pResource->m_bInMultiThreadLoadList = FALSE;

		hr = S_OK;
Exit0:
		return hr;
	}

	HRESULT ProcessMultiThreadLoad()
	{
		for (int i=0; i < MAX_EXCHANGE_RESOURCE; i++)
		{
			TResource* &pResource = m_cResource[i];

			if(pResource)
			{
				m_lpCurrentResource = pResource;
				m_lpCurrentResource->AddRef();

				m_lpCurrentResource->LoadFromFileMultiThread();
				m_lpCurrentResource->m_bInMultiThreadLoadList = FALSE;

				KG_COM_RELEASE(pResource);
                KG_COM_RELEASE(m_lpCurrentResource);
			}
        }
		return S_OK;
	}

};;

template <class TResource>
class TKG3DMultiTheadProcessor
{
public:
	TResource* m_cResourceRegister[MAX_EXCHANGE_RESOURCE];

	HRESULT Init()
	{
		ZeroMemory(m_cResourceRegister,sizeof(TResource*)*MAX_EXCHANGE_RESOURCE);
		return S_OK;
	}

	HRESULT UnInit()
	{
		for (int i=0;i<MAX_EXCHANGE_RESOURCE;i++)
		{
			if( m_cResourceRegister[i] )
			{
				SAFE_RELEASE(m_cResourceRegister[i]);
			}
		}

		return S_OK;
	}

	HRESULT RegisterResource(TResource* pMgr)
	{
		KGLOG_PROCESS_ERROR(pMgr);

		for (int i=0;i<MAX_EXCHANGE_RESOURCE;i++)
		{
			if( m_cResourceRegister[i] == NULL )
			{
				m_cResourceRegister[i] = pMgr;
				pMgr->AddRef();

				return S_OK;
			}
		}

Exit0:
		return E_FAIL;
	}

	HRESULT UnRegisterResource(TResource* pMgr)
	{
		KGLOG_PROCESS_ERROR(pMgr);

		for (int i=0;i<MAX_EXCHANGE_RESOURCE;i++)
		{
			if( m_cResourceRegister[i] == pMgr )
			{
				m_cResourceRegister[i] = NULL;
				pMgr->Release();

				return S_OK;
			}
		}

Exit0:
		return E_FAIL;
	}

	HRESULT MultiThreadProcess()
	{
		for (int i=0;i<MAX_EXCHANGE_RESOURCE;i++)
		{
			TResource* pMgr = m_cResourceRegister[i];
			if(pMgr)
			{
				pMgr->AddRef();
				pMgr->MultiThreadProcess(); 
				pMgr->Release();
			}
		}
		return S_OK;
	}
};;;
