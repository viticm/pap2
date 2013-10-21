#pragma once
#include "IEKG3DScene.h"

class KG3DTerrainRepresentInfo
{
	enum enuTabRow
	{
		COL_BLANK,
		COL_FILENAME,
		COL_ANNOTATE,
		COL_TYPE,
		COL_SFX,
		COL_SFX1,
        COL_SFX2,
        COL_SFX3,
        COL_SFXTERRAIN,
		COL_SFX_RATE,
		TABWIDTH,
	};
public:
    struct GroundDefaultSfx 
    {
        TCHAR str_DefaultSfx[TERRAINREPRESENTSFXNUM][MAX_PATH];
        TCHAR str_DefaultSfxTerrain[MAX_PATH];
        FLOAT fPlayRate;
        GroundDefaultSfx()
        {
            for (int i = 0 ;  i < TERRAINREPRESENTSFXNUM; i ++)
            {
                str_DefaultSfx[i][0] = '\0'; 
            }
            str_DefaultSfxTerrain[0] = '\0';
            fPlayRate = 0.0;
        }
    };
	KG3DTerrainRepresentInfo(void);
	~KG3DTerrainRepresentInfo(void);
	HRESULT Init();
    HRESULT InitDis();
    HRESULT GetDis(FLOAT *pfDis);// 获取地表基准值
    HRESULT GetWaterDis(FLOAT *pfDis);//获取水面基准值
    HRESULT GetDefaultSfx(GroundDefaultSfx **ppGoundDefaultSfx);
    HRESULT InitDefaultSfx();
	const TerrainRepresentInfo* GetInfo(DWORD dwDetailTextureHash);
private:
	TerrainRepresentInfo m_Default;
	std::map<DWORD, TerrainRepresentInfo> m_TerrainRepresentInfo;
    FLOAT m_fDis;
    FLOAT m_fwaterDis;
    GroundDefaultSfx m_DefaultSfx;
};

extern KG3DTerrainRepresentInfo& g_GetTerrainRepresentInfo();
