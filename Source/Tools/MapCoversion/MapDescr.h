////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : MapDescr.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-05-18 14:43:32
//  Comment     : terrain serialize
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _INCLUDE_MAPDESCR_H_
#define _INCLUDE_MAPDESCR_H_


#include  <string>
#include  <memory>
#include  <vector>


#include  <d3d9.h>
#include  <d3dx9.h>



#define TERRAIN_FMTVER_OLD			0x000
#define TERRAIN_FMTVER_COMPACT		0x101

#define TERRAIN_TEXTURE_LAYERS		4


#define TERRAIN_HEADER_LENGTHS		512
#define TERRAIN_HEADER_SKIPS		512 - sizeof( MapFileHeader_T )


/*
	MapFileHeader_T
	MapFileSect_T	
	MapFileRegion_T

	......


*/
enum
{
	TERR_LUMP_HEADER	=	0xF7789023,
	TERR_LUMP_SECT		=	0x4345534D,		// 'MSEC'
	TERR_LUMP_REGION	=	0X4745524D,		// 'MREG'
};


enum
{
	TERR_OPT_SPLIT		=	0x00000001,		//	置位, region 是单独的文件
	TERR_OPT_COMPRESS	=	0x00000002,		//	置位, region 是压缩过的BLEND&LIGHT MAP文件
};



struct MapFileMask_T
{
	DWORD dwMask;
	DWORD dwVersion;
};


struct MapFileLump_T
{
	DWORD dwMask;
	DWORD dwLength;
};


struct MapFileHeader_T
{
	DWORD dwMask;
	DWORD dwVersion;

	DWORD dwRegions;
	DWORD dwOptional;

	DWORD dwNumTerrainVertexX;
	DWORD dwNumTerrainVertexZ;

	INT32 nRegionStartX;
	INT32 nRegionStartZ;
	INT32 nRegionEndX;
	INT32 nRegionEndZ;

	DWORD dwNumRegionDeleted;
	DWORD dwExtend[63];

};


#pragma pack(2)
struct MapFileSect_T
{
	DWORD dwOffset;
	DWORD dwSize;

	SHORT m_nIndex_X;
	SHORT m_nIndex_Z;

	SHORT nGrassNums;
	SHORT nWaterNums;

	SHORT dwTextureCount;
	SHORT nTextureIndex[TERRAIN_TEXTURE_LAYERS];

};

struct MapFileRegion_T
{
public:

	// 以下块都是顺序排列，
	// 每个块记录各自所需的字节大小。
	WORD  m_dwVertexBlock;
	WORD  m_dwBlendTextureBlock;
	WORD  m_dwLightmapTextureBlock;

	WORD  m_dwGrassBlock;
	WORD  m_dwWaterBlock;

};
#pragma pack()



#pragma pack(1)
struct MapFileRegionVB_T
{
	float			Height;
	BYTE			NormalR;
	BYTE			NormalG;
	BYTE			NormalB;
	//DWORD			Normal;
	//D3DXVECTOR3		Normal;
};
#pragma pack()



struct MapFileRegionGrass_T
{
	D3DCOLOR dwColor;
	D3DXVECTOR3 vPosition;//位置
	D3DXVECTOR3 vNormal;
	float fWidth;
	float fHeight;
	float fDistenes;
	int    nIndex;//贴图中的第几个
};



#endif // _INCLUDE_MAPDESCR_H_