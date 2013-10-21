//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   IKGLoadController.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-6-2 10:00
//  Comment     :   封装加载旧的引擎Dll，提供获得加载后的材质数据的接口抽象类
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _IKGLoadController_H__
#define _IKGLoadController_H__

#include <Windows.h>
#include <vector>
#include <string>

class IKGLoadController
{
public:
	typedef struct _BaseMtlData
	{
		BOOL m_bSortAsSFX;
		BOOL m_bHasDetail;
		BOOL m_eLoadingState;

	}BaseMtlData;

	typedef struct _OldOption
	{
		DWORD Type;
		PVOID pData;

	}OldOption;

	typedef struct _OldKG3DTexture
	{
		std::string m_strNameReal;
		BOOL m_bHasTexture;

	}OldKG3DTexture;

	typedef struct _OldMaterialSubset
	{
		const static int cNumID = 8;
		const static int cNumColorCast = 8;
		D3DMATERIAL9     m_Material9;
		OldKG3DTexture   m_Textures[cNumID];
		DWORD            m_TextureNumOption[cNumID];
		DWORD            m_dwOption;
		DWORD            m_dwNumOptions;
		DWORD            m_dwHashValue;
		DWORD            m_dwNumUsedTexture;
		OldKG3DTexture	 m_NormalMap;

		D3DCOLORVALUE*   m_pColorCast;
		float            m_fSpecPower;
		float            m_fEmssPower;

		std::string      m_TextureName[cNumID];
		std::string      m_TextureType[cNumID];

		std::vector<void*> m_pTextureOptions[cNumID];
		std::vector<void*> m_pMatOptions;

		DWORD m_dwMaterialID;//用来做材质排序用的id
		DWORD m_dwPassID;//使用的Shader的Pass

	}OldMaterialSubset;

public:
	virtual int   Init(const char cszClientPath[]) = 0;
	virtual void  UnInit() = 0;
	virtual int   Reset() = 0;
	virtual int   LoadMaterialByOldFunc(const char cszFileName[]) = 0;
	virtual DWORD GetNumMaterials() = 0;
	virtual int   GetSubset(OldMaterialSubset** ppOldMaterialSubset, const UINT cuIndex) = 0;
	virtual int   GetBaseData(BaseMtlData** ppBaseMtlData) = 0;
};
#endif