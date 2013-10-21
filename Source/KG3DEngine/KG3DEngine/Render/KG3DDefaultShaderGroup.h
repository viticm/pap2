////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DDefaultShaderGroup.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-9-5 9:40:39
//  Comment     : 
/*
使用方式：
1.在KG3DTypes.h里面加一个DEFST_XX的类型
2.在KG3DCollocator里面按顺序填一个KG3DDefaultShaderRegistrationArg
3.OK，搞定，在任何地方,g_GetDefaultShaderGroup().GetDefaultShader(DEFST_XX)这样就好罗
4.得到的KG3DShaderHolder参考其h文件的说明，反正GetEffect，然后按Effect的用法用就好了。
自动设备丢失与恢复，不用管析构。

5.有个这样的问题，Effect会对SetTexture的每个Texture加Reference，这样OnLost的时候其实
没有完全清空。所以ShaderManager提供了KG3DShaderEventHandler让自定义Lost的时候的释放。
但这样子很麻烦。现在定义一个这样的规定
texture都这样声明：

int gTexCount = 2;
texture gTex0;
texture gTex1;

那么ShaderGroup会自动帮你完成释放这件事。
*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DDEFAULTSHADERGROUP_H_
#define _INCLUDE_KG3DDEFAULTSHADERGROUP_H_

#include "KG3DShaderHolder.h"

class KG3DShaderEventHandler;
////////////////////////////////////////////////////////////////////////////////

enum
{
	EM_MIN_SHADER_MODEL_SUPPORTED = 1,
	EM_MAX_SHADER_MODEL_SUPPORTED = 3,
	EM_SHADER_MODEL_SUPPORTED_COUNT = EM_MAX_SHADER_MODEL_SUPPORTED - EM_MIN_SHADER_MODEL_SUPPORTED + 1,
};

struct KG3DDefaultShaderRegistrationArg 
{
	DWORD dwType;

	//下面三个字符串，如果是资源，则使用MAKEINTRESOURCE宏来把ID变成字符串指针
	std::tstring	tcsPathVersion1_1;	//必须是static字符指针
	std::tstring	tcsPathVersion2_0; //必须是static字符指针
	std::tstring	tcsPathVersion3_0;	//同上

    unsigned uShaderParamHandleSize;
	
	D3DXMACRO_ARRAY	macroArray;	//默认是D3DXMACRO_ARRAY::GetDefault，请保证Macro里面的字符指针的有效，用直接写的字符串就没有问题
	//下面的都可以为NULL
	LPCTSTR		IncludeFilePath;
	DWORD		Flags;
};

struct KG3D_MaterialDetailInfo_ParamHandle
{
    D3DXHANDLE  tChannel;
    D3DXHANDLE  tDetail0;
    D3DXHANDLE  tDetail1;
    D3DXHANDLE  vEnvSpecular;
    D3DXHANDLE  vBodyScales;
    D3DXHANDLE  vDetailScale;
    D3DXHANDLE  vDetailMtl;
    D3DXHANDLE  vBodyColor;
    D3DXHANDLE  vBodyColor1; 
	D3DXHANDLE  vDetail0Color;
    D3DXHANDLE  vDetail1Color;
};

struct KG3D_LightInfo_ParamHandle
{
    D3DXHANDLE  tEnv;
    D3DXHANDLE  sunLight;
    D3DXHANDLE  playerLight;
    D3DXHANDLE  tSkinTable;
    D3DXHANDLE  eyes;
    D3DXHANDLE  Time;
    D3DXHANDLE  bShadowMask;
    D3DXHANDLE  ShadowTexture;
    D3DXHANDLE  fBiasU;
    D3DXHANDLE  fBiasV;
};

struct KG3D_DEFST_SKINMESH_ParamHandle
{
    KG3D_MaterialDetailInfo_ParamHandle   MaterialDetailInfo;
    KG3D_LightInfo_ParamHandle            LightInfo;

    D3DXHANDLE  nNumBlend;
    D3DXHANDLE  WorldViewProj;
    D3DXHANDLE  matWorld;
    D3DXHANDLE  eyes;
    D3DXHANDLE  bAlphaUseFactor;
   /* D3DXHANDLE  m_emissive;
    D3DXHANDLE  m_diffuse;
    D3DXHANDLE  m_ambient;
    D3DXHANDLE  m_specular;
	D3DXHANDLE  spe_power;*/
	D3DXHANDLE  materialCur;
    D3DXHANDLE  spe_exp;
    D3DXHANDLE  color_cast;    
    D3DXHANDLE  emssive_power;
    D3DXHANDLE  bhightlight;
};


#define KG_SHADER_SET(pShader, Function, Key, Value, pParamHandle, hrRetCode)   \
    if (!pParamHandle->Key)         \
    { \
        pParamHandle->Key = pShader->GetParameterByName(NULL, #Key);    \
        if (!pParamHandle->Key)     \
            hrRetCode = E_FAIL;     \
    }   \
    if (pParamHandle->Key)          \
    {   \
        hrRetCode = pShader->Function(pParamHandle->Key, Value); \
    }

#define KG_SHADER_SET_VALUE(pShader, Function, Key, Value, ValueSize, pParamHandle, hrRetCode) \
    if (!pParamHandle->Key) \
    {   \
        pParamHandle->Key = pShader->GetParameterByName(NULL, #Key);        \
        if (!pParamHandle->Key)     \
            hrRetCode = E_FAIL;      \
    }   \
    if (pParamHandle->Key)          \
    {   \
        hrRetCode = pShader->Function(pParamHandle->Key, Value, ValueSize);   \
    }

#define KG_SHADER_SET2(pShader, Function, Key, Value, pParamHandle, hrRetCode) \
    hrRetCode = pShader->Function(#Key, Value);


#define KG_SHADER_SET_VALUE2(pShader, Function, Key, Value, ValueSize, pParamHandle, hrRetCode) \
    hrRetCode = pShader->Function(#Key, Value, ValueSize);

//下面的KG3DShaderParam开头的Struct，和Shader的Struct是一一对应的
//<KG3DShaderParam>
struct KG3DShaderParamMaterialEx 
{
	INT   bhightlight;
	D3DXCOLOR color_cast;
	FLOAT  emssive_power;
	FLOAT  spe_exp;
};

struct KG3DShaderParamShadowMap 
{
	INT	bShadowMask;
	D3DXVECTOR2 vBiasUV;
};

struct KG3DShaderParamDetail
{
	D3DXVECTOR4 vDetailScale;
	D3DXVECTOR4 vDetailMtl;

	D3DCOLORVALUE vDetail0Color;
	D3DCOLORVALUE vDetail1Color;
	D3DCOLORVALUE vBodyColor;
	D3DCOLORVALUE vBodyColor1;

	D3DXVECTOR4 vBodyScales;
	D3DXVECTOR4 vEnvSpecular;
};

//</KG3DShaderParam>

struct KG3DDefaultShaderGroupElem
{
	KG3DShaderHolder	m_ShaderBottles[EM_SHADER_MODEL_SUPPORTED_COUNT];
};
struct KG3DDefaultShaderGroup 
{
	virtual KG3DShaderHolder& GetDefaultShader(DWORD dwType) = 0;
	virtual LPD3DXEFFECT	GetDefaultEffect(DWORD dwType) = 0;
	//virtual LPD3DXEFFECT	GetDefaultEffectAndHandles(DWORD dwType, LPCTSTR handleStrings[], size_t uStringCount, LPVOID* ppStruct, size_t uStructSize) = 0;
	virtual HRESULT ReloadShaders() = 0;
	
	virtual HRESULT	Init(KG3DDefaultShaderRegistrationArg* pArgs, size_t Count) = 0;//类型要在一定范围内
	virtual HRESULT ReplaceDefaultShader(DWORD dwType, const KG3DDefaultShaderGroupElem& newShader, KG3DDefaultShaderGroupElem& oldShader) = 0;	//替换Shader,需要保存旧的，不然过一会可能会被自动析构掉
	virtual HRESULT ModifyGlobalMacro(D3DXMACRO_ARRAY macroModificationArray) = 0;//还是需要NULL,NULL结尾。注意调用时机，可能会引发一部分Shader重编译。如果没有被初始化的Shader，会只修改创建参数，等待初始化。

	virtual HRESULT CheckShaderModel() = 0;
	virtual ULONG GetShaderModelToUse() = 0;

	virtual HRESULT LoadAllShaders() = 0;

	virtual ~KG3DDefaultShaderGroup() = 0{}
};

extern KG3DDefaultShaderGroup& g_GetDefaultShaderGroup();

#endif //_INCLUDE_KG3DDEFAULTSHADERGROUP_H_
