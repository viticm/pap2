////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DMaterial.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-14 11:13:15
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DMATERIAL_H_
#define _INCLUDE_IEKG3DMATERIAL_H_

////////////////////////////////////////////////////////////////////////////////

#define NORMALIZE_MAT_POWER(a) { if (a > 100.0f || a <= 0.0f) a = 15.0f; }
#define  MATERIAL_OPTION_HIDE              1

#define  MATERIAL_OPTION_ZBUFFER_FALSE     (1<<1)
#define  MATERIAL_OPTION_ZBUFFER_TRUE      (1<<2)
#define  MATERIAL_OPTION_ZBUFFER_USEW      (1<<3)

#define  MATERIAL_OPTION_FILL_POINT        (1<<4)
#define  MATERIAL_OPTION_FILL_WIREFRAME    (1<<5)
#define  MATERIAL_OPTION_FILL_SOLID        (1<<6)

#define  MATERIAL_OPTION_SHADE_FLAT        (1<<7)
#define  MATERIAL_OPTION_SHADE_GOURAUD     (1<<8)
#define  MATERIAL_OPTION_SHADE_PHONG       (1<<9)

#define  MATERIAL_OPTION_CULL_NONE         (1<<10)
#define  MATERIAL_OPTION_CULL_CW           (1<<11)
#define  MATERIAL_OPTION_CULL_CCW          (1<<12)

#define  MATERIAL_OPTION_SPECULARENABLE    (1<<13)
#define  MATERIAL_OPTION_LIGHTING          (1<<14)

#define  MATERIAL_OPTION_SELECTED          (1<<15)
#define  MATERIAL_OPTION_ZWRITEDISABLE     (1<<16)
#define  MATERIAL_OPTION_NOLOGICALCELL    (1<<17) //游戏逻辑的选项，确定是否要生成逻辑障碍

#define  MATERIAL_OPTION_VERSION_2        (1 << 18) // 由于mtl数据里没有版本信息，用这个做v2标本标志
                                                    // v2版本加入新光照模式       

#define  MATERIAL_OPTION_SPEC        (1 << 19)

#define  MATERIAL_OPTION_EMAP        (1 << 20)
#define  MATERIAL_OPTION_INCLUDEALLDEFAULTCOLORCAST	 (1 << 21)
#define  MATERIAL_OPTION_SORTASSFX   (1<<22)
#define  MATERIAL_OPTION_DETAIL      (1<<23)

#define  OPTION_MATERIAL_LIGHTING        100
#define  OPTION_MATERIAL_CULLING         101
#define  OPTION_MATERIAL_FILLMODE        102
#define  OPTION_MATERIAL_MUTIPASS        103
#define  OPTION_MATERIAL_ALPHATEST       104
#define  OPTION_MATERIAL_ALPHABLEND      105
#define  OPTION_MATERIAL_ZBUFFER         106
#define  OPTION_MATERIAL_ZFUN            107
#define  OPTION_MATERIAL_SHADER          200
#define  OPTION_MATERIAL_PIXELSHADER     201
#define  OPTION_MATERIAL_ALPHAOPERATION  202
#define  OPTION_MATERIAL_MSEFFECT        203
#define  OPTION_MATERIAL_MSEFFECT_NEW    204
#define  OPTION_MATERIAL_ALPHARENDERTWICE 205



#define  OPTION_TEXTURE_OPERATION    1000
#define  OPTION_TEXTURE_MAPMETHOD    1001
#define  OPTION_TEXTURE_TRANSFROM    1002
#define  OPTION_TEXTURE_ANIMATION    1003
#define  OPTION_TEXTURE_OPERATIONEX  1004

#define MSEFFECT_WORLD           1
#define MSEFFECT_WORLDI          2
#define MSEFFECT_WORLDIT         3
#define MSEFFECT_VIEW		     4
#define MSEFFECT_VIEWI		     5
#define MSEFFECT_VIEWIT          6
#define MSEFFECT_PROJECTION      7
#define MSEFFECT_PROJECTIONI     8
#define MSEFFECT_PROJECTIONIT    9
#define MSEFFECT_WORLDVIEW       10
#define MSEFFECT_WORLDVIEWI      11
#define MSEFFECT_WORLDVIEWIT     12
#define MSEFFECT_WORLDVIEWPROJ   13
#define MSEFFECT_WORLDVIEWPROJI  14
#define MSEFFECT_WORLDVIEWPROJIT 15
#define MSEFFECT_SUNDIRMATRIX    16
#define MSEFFECT_VIEWPROJ        17

#define MSEFFECT_POSITION        20
#define MSEFFECT_DIRECTION       21
#define MSEFFECT_COLORRGBA       22
#define MSEFFECT_STRING          23

#define MSEFFECT_VOID            30
#define MSEFFECT_BOOL            31
#define MSEFFECT_INT             32
#define MSEFFECT_FLOAT           33

#define MSEFFECT_FLOAT2          34
#define MSEFFECT_FLOAT3          35
#define MSEFFECT_FLOAT4          36

#define MSEFFECT_TEXTURE         50
#define MSEFFECT_TEXTURE1D       51
#define MSEFFECT_TEXTURE2D       52
#define MSEFFECT_TEXTURE3D       53
#define MSEFFECT_TEXTURECUBE     54
#define MSEFFECT_TEXTUREVOLUME   55

#define MSEFFECT_MATERIALAMBIENT   60
#define MSEFFECT_MATERIALDIFFUSE   61
#define MSEFFECT_MATERIALSPECULAR  62
#define MSEFFECT_MATERIALEMISSIVE  63
#define MSEFFECT_MATERIALPOWER     64

#define MSEFFECT_LOCALBBOXMIN     80
#define MSEFFECT_LOCALBBOXMAX     81
#define MSEFFECT_LOCALBBOXSIZE    82

#define MSEFFECT_LIGHTDIR0 90
#define MSEFFECT_LIGHTDIR1 91
#define MSEFFECT_LIGHTDIR2 92
#define MSEFFECT_LIGHTDIR3 93
#define MSEFFECT_LIGHTDIR4 94
#define MSEFFECT_LIGHTDIR5 95
#define MSEFFECT_LIGHTDIR6 96
#define MSEFFECT_LIGHTDIR7 97

#define MSEFFECT_LIGHTDIFF0 110
#define MSEFFECT_LIGHTDIFF1 111
#define MSEFFECT_LIGHTDIFF2 112
#define MSEFFECT_LIGHTDIFF3 113
#define MSEFFECT_LIGHTDIFF4 114
#define MSEFFECT_LIGHTDIFF5 115
#define MSEFFECT_LIGHTDIFF6 116
#define MSEFFECT_LIGHTDIFF7 117

#define MSEFFECT_ENVAMB     130




#define MAT_TEXTURETYPE_LENGTH 20
#define MAT_TEXTUREFILENAME_LENGTH 256

enum enuEffectParameters
{
    ENU_MSEFFECT_WORLD = 1,
    ENU_MSEFFECT_WORLDI,// = 2,
    ENU_MSEFFECT_WORLDIT,// = 3,
    ENU_MSEFFECT_VIEW,// = 4,
    ENU_MSEFFECT_VIEWI,// = 5,
    ENU_MSEFFECT_VIEWIT,// = 6,
    ENU_MSEFFECT_PROJECTION,// = 7,
    ENU_MSEFFECT_PROJECTIONI,// = 8,
    ENU_MSEFFECT_PROJECTIONIT,// = 9,
    ENU_MSEFFECT_WORLDVIEW,// = 10,
    ENU_MSEFFECT_WORLDVIEWI,// = 11,
    ENU_MSEFFECT_WORLDVIEWIT,// = 12,
    ENU_MSEFFECT_WORLDVIEWPROJ,//13,
    ENU_MSEFFECT_WORLDVIEWPROJI,//14
    ENU_MSEFFECT_WORLDVIEWPROJIT,// 15
    ENU_MSEFFECT_SUNDIRMATRIX,//    16
    ENU_MSEFFECT_VIEWPROJ,//        17

    ENU_MSEFFECT_POSITION = 20,//20       
    ENU_MSEFFECT_DIRECTION,//21
    ENU_MSEFFECT_COLORRGBA,//22
    ENU_MSEFFECT_STRING,//23

    ENU_MSEFFECT_VOID = 30, 
    ENU_MSEFFECT_BOOL,//31
    ENU_MSEFFECT_INT,//32
    ENU_MSEFFECT_FLOAT,//33
    ENU_MSEFFECT_FLOAT2, //34
    ENU_MSEFFECT_FLOAT3,//35
    ENU_MSEFFECT_FLOAT4,//36

    ENU_MSEFFECT_TEXTURE = 50, //50
    ENU_MSEFFECT_TEXTURE1D, //51
    ENU_MSEFFECT_TEXTURE2D, //52
    ENU_MSEFFECT_TEXTURE3D, //53
    ENU_MSEFFECT_TEXTURECUBE, //54
    ENU_MSEFFECT_TEXTUREVOLUME, //55

    ENU_MSEFFECT_MATERIALAMBIENT = 60, //60
    ENU_MSEFFECT_MATERIALDIFFUSE,//61
    ENU_MSEFFECT_MATERIALSPECULAR,//62
    ENU_MSEFFECT_MATERIALEMISSIVE,//63
    ENU_MSEFFECT_MATERIALPOWER,//64

    ENU_MSEFFECT_LOCALBBOXMIN = 80,//80
    ENU_MSEFFECT_LOCALBBOXMAX,//81
    ENU_MSEFFECT_LOCALBBOXSIZE, //82

    ENU_MSEFFECT_LIGHTDIR0 = 90, //90
    ENU_MSEFFECT_LIGHTDIR1, //91
    ENU_MSEFFECT_LIGHTDIR2, //92
    ENU_MSEFFECT_LIGHTDIR3, //93
    ENU_MSEFFECT_LIGHTDIR4, //94
    ENU_MSEFFECT_LIGHTDIR5, //95
    ENU_MSEFFECT_LIGHTDIR6, //96
    ENU_MSEFFECT_LIGHTDIR7, //97

    ENU_MSEFFECT_LIGHTDIFF0 = 110, //110
    ENU_MSEFFECT_LIGHTDIFF1, //111
    ENU_MSEFFECT_LIGHTDIFF2, //112
    ENU_MSEFFECT_LIGHTDIFF3, //113
    ENU_MSEFFECT_LIGHTDIFF4, //114
    ENU_MSEFFECT_LIGHTDIFF5, //115
    ENU_MSEFFECT_LIGHTDIFF6, //116
    ENU_MSEFFECT_LIGHTDIFF7, //117

    ENU_MSEFFECT_FOGINFO = 120,//120
    ENU_MSEFFECT_FOGCOLOR,//121
    ENU_MSEFFECT_ENVAMB = 130, //130

    //Add above this
    ENU_MSEFFECT_COUNT,

    ENU_MSEFFECT_FORCE_DWORD = 0xffffffff,
};

//////////////////////////////////////////////////////////////////////////
//高级的ALPHA操作设置
struct _MtlAlphaOperation
{
    DWORD EnableAlphaBlend;
    DWORD SrcBlend;
    DWORD DestBlend;
    DWORD BlendFactor;
    DWORD BlendOperator;

    DWORD EnableSeparateAlphaBlend;
    DWORD BlendOperatorAlpha;
    DWORD SrcBlendAlpha;
    DWORD DestBlendAlpha;

    DWORD EnableAlphaTest;
    DWORD AlphaTestRef;
    DWORD AlphaFunction;

    DWORD BackEnableAlphaBlend;
    DWORD BackSrcBlend;
    DWORD BackDestBlend;
    DWORD BackBlendFactor;
    DWORD BackBlendOperator;

    DWORD BackEnableSeparateAlphaBlend;
    DWORD BackBlendOperatorAlpha;
    DWORD BackSrcBlendAlpha;
    DWORD BackDestBlendAlpha;

    DWORD BackEnableAlphaTest;
    DWORD BackAlphaTestRef;
    DWORD BackAlphaFunction;
};

struct _MtlRenderTwiceAlphaOption
{
	BOOL  bEnable;
	DWORD dwAlphaRefPass0;
	DWORD dwAlphaRefPass1;
	const static DWORD s_dwRef0 = 254;
	const static DWORD s_dwRef1 = 32;
};

enum TEXTURE_LOAD_STATE
{
    TEXTURE_LOAD_UNKNOWN_ERROR,
    TEXTURE_LOAD_NOT_EXIST,
    TEXTURE_LOAD_FAILED,
    TEXTURE_LOAD_SUCCESS
};
interface IEKG3DMaterial
{
    virtual ULONG STDMETHODCALLTYPE Release() = 0;

    virtual HRESULT GetName(LPCTSTR *ppszName) = 0;

    virtual HRESULT SaveToFile(const TCHAR cszFileName[],UINT dwOption) = 0;
    virtual HRESULT LoadFromFile(const TCHAR cszFileName[], UINT uFileNameHash, UINT uOption) = 0;

    virtual HRESULT GetMaterialCount(DWORD *pdwCount) = 0;

    virtual HRESULT GetSubMaterialItemAlphaOp(DWORD dwIndex, _MtlAlphaOperation **ppAlphaOp) = 0;

    virtual HRESULT SetSubMaterialColorRef2Value(
        DWORD dwIndex, 
        DWORD dwAmbientColor,
        DWORD dwDiffuseColor,
        DWORD dwEmissive,
        DWORD dwSpecular
    ) = 0;

    virtual HRESULT GetSubMaterialOption(DWORD dwIndex, DWORD *pdwOption) = 0;
    virtual HRESULT SetSubMaterialOption(DWORD dwIndex, DWORD dwOption) = 0;

    virtual HRESULT Init() = 0;
    virtual HRESULT UnInit() = 0;

    virtual ULONG STDMETHODCALLTYPE AddRef() = 0;

    virtual HRESULT GetSubMaterial(DWORD dwIndex, PVOID *ppSubMat) = 0;

    virtual HRESULT GetSubMaterialTextureID(DWORD dwIndex, DWORD *pdwTextureIDs) = 0;

    virtual HRESULT GetTexturePathName(DWORD dwSubsetIndex,
        DWORD dwIndex,
        LPSTR strBuffer, 
        DWORD dwSize,
        TEXTURE_LOAD_STATE *peLoadState
    ) = 0;

	virtual BOOL IsSortAsSFX() = 0;
	virtual HRESULT SetSortAsSFX(BOOL bSort) = 0;

	virtual int GetMaterialDetailDefineCount() = 0;
	virtual HRESULT AddMaterialDetailDefine(int nIndex)    = 0;
	virtual HRESULT DeleteMaterialDetailDefine(int nIndex) = 0;

    virtual int GetMaxMtlIndex() = 0;
    virtual int FindIndex(int nIndex) = 0;
    virtual void SetDetailWholeColorCast(BOOL bWholeColorCast) = 0;
    virtual BOOL IsSubSetDetailExist(int nIndex) = 0;
    virtual HRESULT CopyTextureToFile(LPCSTR pszSourceFile, LPCSTR pszDestFile) = 0;
    virtual HRESULT MoveMaterialDetails(int nIndex) = 0;
    virtual string GetOrgTextureFileName() = 0;
    virtual HRESULT SetOrgTextureFileName(LPCSTR pszNewFileName) = 0;
};
#endif //_INCLUDE_IEKG3DMATERIAL_H_
