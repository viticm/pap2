////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : MemCompileFileController.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-6-2 20:26:18
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MemCompileFileController.h"
#include "Core.h"

struct CCompiledEffectData 
{
	FILETIME ftLastWriteTime;
};
class  CMemCompileFileController : public ICCompileFileController	//输出文件没有地方放，放内存中，只保存文件写时间，App不关闭的情况下，可以避免重复编译
{
public:
	CMemCompileFileController():m_bForceRebuild(FALSE){}

	virtual BOOL IsFileUnChanged(LPCTSTR fileName, const FILETIME& ftLastWriteTime)
	{
		if (m_bForceRebuild)
		{
			return FALSE;
		}
		std::tstring strTemp(fileName);
		TypeMap::iterator it = m_compiledEffectData.find(strTemp);
		if (it != m_compiledEffectData.end())
		{
			CCompiledEffectData& dataTemp = it->second;
			LONG nRet = ::CompareFileTime(&dataTemp.ftLastWriteTime, &ftLastWriteTime);
			if (0 == nRet)
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	virtual BOOL SaveFileOutput(LPCTSTR fileName, const FILETIME& ftLastWriteTime, LPD3DXEFFECT, BOOL)
	{
		_ASSERTE(Core::em_file_limit >= m_compiledEffectData.size());
		if(Core::em_file_limit <= m_compiledEffectData.size())
			return FALSE;

		CCompiledEffectData dataTemp;
		dataTemp.ftLastWriteTime = ftLastWriteTime;
		TypeMap::value_type dataToInsert(std::tstring(fileName), dataTemp);
		TypeMap::_Pairib ib = m_compiledEffectData.insert(dataToInsert);
		return  ib.second;
	}
	virtual BOOL CopyFileToDes(LPCTSTR fileFullPath, BOOL* pSkipped)
	{
		_ASSERTE(NULL && _T("不支持这个"));
		return FALSE;
	}
	virtual VOID SetPaths(LPCTSTR strOutputDir, LPCTSTR strCopyToDir)
	{
		return;
	}
	virtual VOID SetForceRebuild(BOOL bForceRebuild)
	{
		if (bForceRebuild)
		{
			m_compiledEffectData.clear();
		}
		m_bForceRebuild = bForceRebuild;
	}
private:
	typedef std::map<std::tstring, CCompiledEffectData> TypeMap;
	TypeMap m_compiledEffectData;

	BOOL m_bForceRebuild;	
};

ICCompileFileController* CreateMemFileController()
{
	return new CMemCompileFileController;
}
////////////////////////////////////////////////////////////////////////////////

