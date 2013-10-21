#pragma once

class KG3DTerrainRegionInterface
{
public:
	KG3DTerrainRegionInterface(void);
	virtual ~KG3DTerrainRegionInterface(void);
};


class KG3DTerrainRegionDataBase
{
public:
	DWORD m_dwDetailBlendWidth;//地表细节混合信息的宽度，默认32*32.还有16*16,8*8,4*4;
	BYTE* m_lpDetailBlend;
	unsigned long long m_lTextureKey;

	KG3DTerrainRegionDataBase();
	virtual ~KG3DTerrainRegionDataBase();

	HRESULT CreateDetailBlendData(DWORD dwWidth);

	virtual HRESULT Init();
	virtual HRESULT UnInit();
};