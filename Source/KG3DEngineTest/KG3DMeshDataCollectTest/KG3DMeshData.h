#ifndef _KG3DMESHDATA_H_
#define _KG3DMESHDATA_H_

#define MESH_TEST_OUT_FILE "MeshData.tab"

struct _MeshFileHead
{
    TFileHeader KSFileHeader_NotUse;
    DWORD       dwFileMask;
    DWORD       dwBlockLength;
    DWORD       dwVersion;
    DWORD       AnimationBlock_NotUse;
    DWORD       ExtendBlock_NotUse[10];
    DWORD       MeshCount_NotUse;
    _MeshFileHead();
    const static DWORD s_dwFileMask = 0x4d455348;//"mesh"
    const static DWORD s_dwCurrentVersion = 0;
};

struct MeshDataBlocks
{
    DWORD  PositionBlock;
    DWORD  NormalBlock;
    DWORD  DiffuseBlock;
    DWORD  TextureUVW1Block;
    DWORD  TextureUVW2Block;
    DWORD  TextureUVW3Block;
    DWORD  FacesIndexBlock;
    DWORD  SubsetIndexBlock;
    DWORD  SkinInfoBlock;
    DWORD  LODInfoBlock;
    DWORD  FlexibleBodyBlock; // not need the data now
    DWORD  BBoxBlock;
    DWORD  BlendMeshBlock;
    DWORD  ExtendBlock[17];
};

struct _MeshHead
{
    DWORD  dwNumVertices;
    DWORD  dwNumFaces;
    DWORD  dwNumSubset;
    union
    {
        MeshDataBlocks Blocks;
        DWORD dwBlocks[30];
    };
    _MeshHead(){ZeroMemory(dwBlocks, sizeof(DWORD) * 30);};
};

#endif