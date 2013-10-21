////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : MapDescr.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-05-18 14:43:32
//  Comment     : terrain serialize
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _INCLUDE_MAPDESCROLD_H_
#define _INCLUDE_MAPDESCROLD_H_


#define TERRAIN_FMTMARK				0xF7789023
#define TERRAIN_FMTBLOCKEND			0xFFFFFFFF

#define GROUND_TEXTURE_LAYERS		4


struct KG3DTerrainRegionTextureInfo
{
	DWORD   TextureCount;
	INT32   TextureIndex[GROUND_TEXTURE_LAYERS];

	KG3DTerrainRegionTextureInfo()
	{
		TextureCount = 1;
		TextureIndex[0] = 1;
		TextureIndex[1] = TextureIndex[2] = TextureIndex[3] = -1;
	}
	KG3DTerrainRegionTextureInfo operator = (const KG3DTerrainRegionTextureInfo &texinfo)
	{
		TextureCount = texinfo.TextureCount;
		for(int i = 0;i < GROUND_TEXTURE_LAYERS;++i)
		{
			TextureIndex[i] = texinfo.TextureIndex[i];
		}
		return *this;
	}
};

struct KG3DTerrainMapFileHeader
{
	DWORD dwFileMask;
	DWORD dwVersion;
	DWORD dwBlockLength;

	DWORD dwOption;

	DWORD dwNumTerrainVertexX;
	DWORD dwNumTerrainVertexZ;

	INT32 nRegionStartX;
	INT32 nRegionStartZ;
	INT32 nRegionEndX;
	INT32 nRegionEndZ;

	DWORD dwRegionsBlock;
	DWORD dwGroundHeightBlock;
	DWORD dwWaterHeightBlock;

	DWORD dwExtend[64];

	KG3DTerrainMapFileHeader()
	{
		dwFileMask = 0xF7789023;
		dwVersion = 0;
		dwBlockLength = 0;

		dwOption = 0;

		dwNumTerrainVertexX = 0;
		dwNumTerrainVertexZ = 0;

		nRegionStartX = 0;
		nRegionStartZ = 0;
		nRegionEndX = 0;
		nRegionEndZ = 0;

		dwRegionsBlock = 0;
		dwGroundHeightBlock = 0;
		dwWaterHeightBlock = 0;

		ZeroMemory(dwExtend,sizeof(DWORD)*64);
	}
};

class KG3DTerrainRegionFileData
{
public:
	INT32 m_nIndex_X;
	INT32 m_nIndex_Z;

	DWORD m_dwVertexBlock;
	DWORD m_dwBlendTextureBlock;
	DWORD m_dwLightmapTextureBlock;
	DWORD m_dwNormalmapTextureBlock;
	DWORD m_dwSpecularTextureBlock;
	DWORD m_dwWaterBlock;
	DWORD m_dwSprayBlock;
	DWORD m_dwObjectBlock;
	DWORD m_dwLogicalBlock;
	DWORD m_dwGrassBlock;

	DWORD m_dwUnusedBlock9;
	DWORD m_dwUnusedBlock8;
	DWORD m_dwUnusedBlock7;
	DWORD m_dwUnusedBlock6;
	DWORD m_dwUnusedBlock5;
	DWORD m_dwUnusedBlock4;
	DWORD m_dwUnusedBlock3;
	DWORD m_dwUnusedBlock2;
	DWORD m_dwUnusedBlock1;
	DWORD m_dwUnusedBlock0;

	DWORD m_dwUnusedBlock00;
	KG3DTerrainRegionTextureInfo m_vTextureInfo;
	DWORD m_dwExtend[16];


};


struct OLD_RegionVB
{
	float			Height;
	D3DXVECTOR3		Normal;
	DWORD			Color;
};


struct OLD_RegionGrass
{
	D3DCOLOR dwColor;
	D3DXVECTOR3 vPosition;//位置
	D3DXVECTOR3 vNormal;
	float fWidth;
	float fHeight;
	float fDistenes;
	int    nIndex;//贴图中的第几个
};




#endif // _INCLUDE_MAPDESCROLD_H_