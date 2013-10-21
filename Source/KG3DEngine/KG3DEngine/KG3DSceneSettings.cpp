////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneSettings.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-8-22 18:47:17
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DSceneSettings.h"
#include "FileType.h"
#include "KGINIFileHelpers.h"
#include "IEKG3DScene.h"
#include "IEKG3DTerrain.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
KG3DSceneSettings::KG3DSceneSettings()
:m_Datas(scene_setting_count, NULL)
{
	_ASSERTE(scene_setting_count <= scene_setting_max && _T("SceneSetting的位置是预先分配的，太多会溢出"));
	//_ASSERTE(_countof(scene_setting_descriptions) == scene_setting_count && _T("变量说明数量和变量数量要相等"));
	
	m_strIniFile.clear();
}
HRESULT KG3DSceneSettings::LoadFromFile( TCHAR lpFile[], DWORD_PTR dwOption )
{
//	IIniFile* pFile = KG_CUSTOM_HELPERS::OpenIniFile(lpFile);	
//	KG_PROCESS_ERROR(pFile);
//	m_strIniFile = lpFile;//记录一下这个文件名
//	_ASSERTE(m_Datas.size() >= scene_setting_count);
//	for(size_t i = 0; i < scene_setting_count; ++i)
//	{
//		//KG_CUSTOM_HELPERS::LoadINIFileData(pFile, _T("RLSplit"), _T("HowManyPixelsPerRegion"), 1, &m_nHowManyPixelPerRegionInRLSplit);
//
//		KG_CUSTOM_HELPERS::LoadINIFileData(pFile, _T("SceneSettings"), scene_setting_descriptions[i]
//				, NULL, &m_Datas[i]);
//	}
//	KG_COM_RELEASE(pFile);
//	return S_OK;
//Exit0:
//	KG_COM_RELEASE(pFile);
//	return E_FAIL;
	return E_FAIL;
}

HRESULT KG3DSceneSettings::SaveToFile( TCHAR lpFile[], DWORD_PTR dwOption )
{
//	IIniFile* pFile = KG_CUSTOM_HELPERS::OpenIniFile(lpFile, false, true);
//	KG_PROCESS_ERROR(pFile);
//	_ASSERTE(m_Datas[i].size() >= scene_setting_count);
//	for(size_t i = 0; i < scene_setting_count; ++i)
//	{
//		//KG_CUSTOM_HELPERS::SaveINIFileData(pFile, _T("RLSplit"), _T("HowManyPixelsPerRegion"), m_nHowManyPixelPerRegionInRLSplit);
//		KG_CUSTOM_HELPERS::SaveINIFileData(pFile, _T("SceneSettings"), scene_setting_descriptions[i]
//				, m_Datas[i]);
//	}
//	KG_COM_RELEASE(pFile);
//	return S_OK;
//Exit0:
//	KG_COM_RELEASE(pFile);
//	return E_FAIL;
	return E_FAIL;
}

HRESULT KG3DSceneSettings::SetSceneSettingData( DWORD dwEnum, DWORD_PTR dwData )
{
	_ASSERTE(dwEnum < scene_setting_count);
	_ASSERTE(m_Datas.size() > (size_t)dwEnum);
	KG_PROCESS_ERROR(m_Datas.size() > (size_t)dwEnum);
	m_Datas[dwEnum] 
		= (unsigned long)dwData;
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSettings::GetSceneSettingData( DWORD dwEnum, DWORD_PTR* pdwData )
{
	KG_PROCESS_ERROR(NULL != pdwData);
	KG_PROCESS_ERROR(m_Datas.size() > (size_t)dwEnum);
	*pdwData = GetSceneSettingData(dwEnum);
	return S_OK;
Exit0:
	return E_FAIL;
}

DWORD_PTR KG3DSceneSettings::GetSceneSettingData( DWORD dwEnum )
{
	_ASSERTE(dwEnum < scene_setting_count);
	_ASSERTE(m_Datas.size() > (size_t)dwEnum);
	KG_PROCESS_ERROR(m_Datas.size() > (size_t)dwEnum);
	return m_Datas[dwEnum];
Exit0:
	return 0;
}
//HRESULT KG3DSceneSettings::GetSceneSettingData(LPCTSTR Section, LPCTSTR Key, std::tstring& Data)
//{
//	IIniFile* pFile = NULL;
//	KG_PROCESS_ERROR(m_strIniFile.empty());
//	{
//		pFile = KG_CUSTOM_HELPERS::OpenIniFile(m_strIniFile.c_str());
//		KG_PROCESS_ERROR(pFile);
//		KG_PROCESS_ERROR(KG_CUSTOM_HELPERS::LoadINIFileData(pFile, Section, Key, _T(""), Data));
//	}
//	KG_COM_RELEASE(pFile);
//	return S_OK;
//Exit0:
//	KG_COM_RELEASE(pFile);
//	return E_FAIL;
//}
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DSceneSettings)
{
	KG3DSceneSettings Settings;
	{
		DWORD_PTR enable = Settings.GetSceneSettingData(scene_setting_enable_render_terrain_mask);
		_ASSERTE(! enable);
		Settings.SetSceneSettingData(scene_setting_enable_render_terrain_mask, TRUE);
		enable = Settings.GetSceneSettingData(scene_setting_enable_render_terrain_mask);
		_ASSERTE(enable);
		Settings.SetSceneSettingData(scene_setting_enable_render_terrain_mask, FALSE);
		enable = Settings.GetSceneSettingData(scene_setting_enable_render_terrain_mask);
		_ASSERTE(! enable);
	}
}
KG_TEST_END(KG3DSceneSettings)
#endif