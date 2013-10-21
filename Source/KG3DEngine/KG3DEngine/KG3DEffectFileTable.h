#pragma once
#include "Engine/KMutex.h"

class KG3DEffectFileTable 
{
public:
    KG3DEffectFileTable(void);
    virtual ~KG3DEffectFileTable(void);
    HRESULT LoadFromFile(LPCSTR strFileName, LPD3DXEFFECT& pEffect);
    void Release(LPD3DXEFFECT pEffect);
    HRESULT OnLostDevice();
    HRESULT OnResetDevice();
private:
	//KMutex m_Lock;
    struct EffectInfo
    {
        DWORD dwRef;
        std::string strFileName;
        EffectInfo()
        {
            dwRef = 0;
        }
    };
    stdext::hash_map<std::string, LPD3DXEFFECT> m_EffectFiles;
    std::map<LPD3DXEFFECT, EffectInfo> m_ReleaseMapTable;
};

extern KG3DEffectFileTable g_cEffectFileTable;
