#include "StdAfx.h"
#include "KG3DEffectFileTable.h"
#include "KG3DGraphicsTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DEffectFileTable g_cEffectFileTable;

KG3DEffectFileTable::KG3DEffectFileTable(void)
{

}

KG3DEffectFileTable::~KG3DEffectFileTable(void)
{
    for (std::map<LPD3DXEFFECT, EffectInfo>::iterator i = m_ReleaseMapTable.begin();
        i != m_ReleaseMapTable.end(); ++i)
    {
        ASSERT(QueryRef(i->first) == 0);
    }
}

HRESULT KG3DEffectFileTable::OnLostDevice()
{
	//m_Lock.Lock();
    for (std::map<LPD3DXEFFECT, EffectInfo>::iterator i = m_ReleaseMapTable.begin();
        i != m_ReleaseMapTable.end(); ++i)
    {
        i->first->OnLostDevice();
    }
	//m_Lock.Unlock();

    return S_OK;
}

HRESULT KG3DEffectFileTable::OnResetDevice()
{
	//m_Lock.Lock();

    for (std::map<LPD3DXEFFECT, EffectInfo>::iterator i = m_ReleaseMapTable.begin();
        i != m_ReleaseMapTable.end(); ++i)
    {
        i->first->OnResetDevice();
    }

	//m_Lock.Unlock();
    return S_OK;
}

HRESULT KG3DEffectFileTable::LoadFromFile(LPCSTR strFileName, LPD3DXEFFECT& pEffect)
{
	//m_Lock.Lock();

    HRESULT hr = E_FAIL;
    stdext::hash_map<std::string, LPD3DXEFFECT>::iterator i;
    KG_PROCESS_ERROR(strFileName);
    i = m_EffectFiles.find(strFileName);
    if (i != m_EffectFiles.end())
    {
        pEffect = i->second;
        m_ReleaseMapTable[pEffect].dwRef++;
        hr = S_OK;
    }
    else
    {
        LPD3DXEFFECT pNewEffect = NULL;
        DWORD dwShaderFlags = 0;
#ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif
        hr = KG3DD3DXCreateEffectFromFile(g_pd3dDevice, 
            strFileName, 
            NULL, 
            NULL, 
            dwShaderFlags,
            NULL,
            &pNewEffect,
            NULL);
        KGLOG_COM_PROCESS_ERROR(hr);
        m_EffectFiles[strFileName] = pNewEffect;
        m_ReleaseMapTable[pNewEffect].strFileName = strFileName;
        m_ReleaseMapTable[pNewEffect].dwRef = 1;
        pEffect = pNewEffect;
    }
    if (pEffect)
    {
        pEffect->AddRef();
    }
Exit0:
	//m_Lock.Unlock();
    return hr;
}

void KG3DEffectFileTable::Release(LPD3DXEFFECT pEffect)
{
	std::map<LPD3DXEFFECT, EffectInfo>::iterator j;
	//m_Lock.Lock();

	KGLOG_PROCESS_ERROR(pEffect);

	j = m_ReleaseMapTable.find(pEffect);
    if (j != m_ReleaseMapTable.end())
    {
        pEffect->Release();
        m_ReleaseMapTable[pEffect].dwRef--;
        if (m_ReleaseMapTable[pEffect].dwRef == 0)
        {
            ASSERT(QueryRef(pEffect) == 1);
            pEffect->Release();

            stdext::hash_map<std::string, LPD3DXEFFECT>::iterator i = m_EffectFiles.find(m_ReleaseMapTable[pEffect].strFileName);
            if (i != m_EffectFiles.end())
            {
                m_EffectFiles.erase(i);
            }
            m_ReleaseMapTable.erase(j);
        }
    }

Exit0:
	;
	//m_Lock.Unlock();
}