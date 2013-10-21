////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneSettings.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-8-22 18:47:06
//  Comment     : 用于保存一个场景的明码的全局状态，暂时不是很有用
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSCENESETTINGS_H_
#define _INCLUDE_KG3DSCENESETTINGS_H_

#ifndef tstring
#if defined(UNICODE) | defined(_UNICODE)
#define tstring wstring
#else
#define tstring string
#endif
#endif
////////////////////////////////////////////////////////////////////////////////

class KG3DSceneSettings 
{
public:
	KG3DSceneSettings();
	HRESULT SetSceneSettingData(DWORD dwEnum, DWORD_PTR dwData);
	HRESULT GetSceneSettingData(DWORD dwEnum, DWORD_PTR* pdwData);
	DWORD_PTR	GetSceneSettingData(DWORD dwEnum);
	//HRESULT GetSceneSettingData(LPCTSTR Section, LPCTSTR Key, std::tstring& Data);
	/*template<typename _Type>
	HRESULT GetSceneSettingData(LPCTSTR Section, LPCTSTR Key, _Type& dwData)
	{
		std::tstring strTemp;
		HRESULT hr = GetSceneSettingData(Section, Key, strTemp);
		KG_COM_PROCESS_ERROR(hr);
		KG_PROCESS_ERROR(TStringToValue(strTemp, dwData));
		return S_OK;
Exit0:
		return E_FAIL;
	}*/
	HRESULT LoadFromFile(TCHAR lpFile[], DWORD_PTR dwOption);
	HRESULT SaveToFile(TCHAR lpFile[], DWORD_PTR dwOption);
private:
	std::tstring	m_strIniFile;
	typedef std::vector<DWORD_PTR>	TypeDataCt;
	TypeDataCt m_Datas;
};

#endif //_INCLUDE_KG3DSCENESETTINGS_H_
