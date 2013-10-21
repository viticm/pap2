#include "StdAfx.h"
#include "KG3DTerrainRepresentInfo.h"

extern TCHAR g_szConfigFilePath[MAX_PATH];
KG3DTerrainRepresentInfo& g_GetTerrainRepresentInfo()
{
	static KG3DTerrainRepresentInfo s_Mgr;
	return s_Mgr;
}

static const TCHAR* s_strConfigFile = "data\\public\\terraindetailinfo.tab";
static const TCHAR* s_strGroundEffectConfigFile="data\\public\\groundeffect.ini";
KG3DTerrainRepresentInfo::KG3DTerrainRepresentInfo(void)
{
	memset(&m_Default, 0, sizeof(TerrainRepresentInfo));
	m_Default.dwType = INVALID_GROUNDTYPE;
    m_fDis = 500.0;
    m_fwaterDis = 500.0;
}

KG3DTerrainRepresentInfo::~KG3DTerrainRepresentInfo(void)
{
}

const TerrainRepresentInfo* KG3DTerrainRepresentInfo::GetInfo(DWORD dwDetailTextureHash)
{
	const TerrainRepresentInfo* pReturn = &m_Default;//如果没有就返回默认的
	std::map<DWORD, TerrainRepresentInfo>::iterator it = m_TerrainRepresentInfo.find(dwDetailTextureHash);
	KG_PROCESS_ERROR(it != m_TerrainRepresentInfo.end());

	pReturn = &it->second;
Exit0:
	return pReturn;
}
HRESULT KG3DTerrainRepresentInfo::InitDis()
{
    HRESULT hResult = E_FAIL;
    IIniFile* pFile = NULL;

    pFile = g_OpenIniFile(s_strGroundEffectConfigFile);
    KG_PROCESS_ERROR(pFile);
    
    pFile->GetFloat("ROOT","GroundEffectDis",500.0,&m_fDis);
    pFile->GetFloat("ROOT","GroundEffectWaterDis",500.0,&m_fwaterDis);

    hResult = S_OK;
Exit0:
    SAFE_RELEASE(pFile);
    return hResult;
}
HRESULT KG3DTerrainRepresentInfo::Init()
{
	HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL;
	int nHeight = 0;
	
	m_TerrainRepresentInfo.clear();

	ITabFile *pTabFile = g_OpenTabFile(s_strConfigFile);
	KG_PROCESS_ERROR(pTabFile);

	nHeight = pTabFile->GetHeight();
	for (int i = 2; i <= nHeight; i++)
	{
		TerrainRepresentInfo Info;
		int nGroundType = 0;
		TCHAR strLower[MAX_PATH];
		pTabFile->GetString(i, COL_FILENAME, "", Info.strFileName, MAX_PATH);
		strcpy_s(strLower, MAX_PATH, Info.strFileName);
		_strlwr_s(strLower, MAX_PATH);
		DWORD dwHash = g_FileNameHash(strLower);
		pTabFile->GetString(i, COL_ANNOTATE, "", Info.strAnnotate, MAX_PATH);
		pTabFile->GetInteger(i, COL_TYPE, INVALID_GROUNDTYPE, &nGroundType);
		Info.dwType = static_cast<DWORD>(nGroundType);

        pTabFile->GetString(i,COL_SFX,"",Info.strSfx[COL_SFX-COL_SFX],MAX_PATH);
        pTabFile->GetString(i,COL_SFX1,"",Info.strSfx[COL_SFX1-COL_SFX],MAX_PATH);
        pTabFile->GetString(i,COL_SFX2,"",Info.strSfx[COL_SFX2-COL_SFX],MAX_PATH);
        pTabFile->GetString(i,COL_SFX3,"",Info.strSfx[COL_SFX3-COL_SFX],MAX_PATH);
        pTabFile->GetString(i,COL_SFXTERRAIN,"",Info.strSfxTerrain,MAX_PATH);

		pTabFile->GetFloat(i, COL_SFX_RATE, 1.0f, &Info.fSfxPlayRate);//不填默认100%
		m_TerrainRepresentInfo[dwHash] = Info;
	}
    hRetCode = InitDis();
    KG_COM_PROCESS_ERROR(hRetCode);
    hRetCode = InitDefaultSfx();
    KG_COM_PROCESS_ERROR(hRetCode);
	hResult = S_OK;
Exit0:
	SAFE_RELEASE(pTabFile);
	return hResult;
}
HRESULT KG3DTerrainRepresentInfo::GetWaterDis(FLOAT *pfDis)
{
    *pfDis = m_fwaterDis;
    return S_OK;
}
HRESULT KG3DTerrainRepresentInfo::GetDis(FLOAT *pfDis)
{
    *pfDis = m_fDis;
    return S_OK;
}
HRESULT KG3DTerrainRepresentInfo::GetDefaultSfx(GroundDefaultSfx **ppGoundDefaultSfx)
{
    HRESULT hResult = E_FAIL;
    KG_PROCESS_ERROR(ppGoundDefaultSfx);
    *ppGoundDefaultSfx = &m_DefaultSfx;
    hResult = S_OK;
Exit0:
    return hResult;
}
HRESULT KG3DTerrainRepresentInfo::InitDefaultSfx()
{
    HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    IIniFile *pFile = NULL;
    pFile = g_OpenIniFile(s_strGroundEffectConfigFile);
    KG_PROCESS_ERROR(pFile);

    pFile->GetString("ROOT","DefaultSfx1","",m_DefaultSfx.str_DefaultSfx[0],MAX_PATH);
    pFile->GetString("ROOT","DefaultSfx2","",m_DefaultSfx.str_DefaultSfx[1],MAX_PATH);
    pFile->GetString("ROOT","DefaultSfx3","",m_DefaultSfx.str_DefaultSfx[2],MAX_PATH);
    pFile->GetString("ROOT","DefaultSfx4","",m_DefaultSfx.str_DefaultSfx[3],MAX_PATH);
    pFile->GetString("ROOT","DefaultSfxTerrain","",m_DefaultSfx.str_DefaultSfxTerrain,MAX_PATH);
    pFile->GetFloat("ROOT","DefaultSfxTerrainPlayRate",0.0,&m_DefaultSfx.fPlayRate);

    hResult = S_OK;
Exit0:
    SAFE_RELEASE(pFile);
    return hResult;
}
