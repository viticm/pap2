#include "stdafx.h"
#include "../../SO3Represent.h"

namespace 
{
	static const LPCTSTR s_szFilePath = _T("Represent\\filepath.ini");
	static const LPCTSTR s_szFilePathSection = _T("FILE_PATH");
}

KFilePath::KFilePath()
{
	for (int nIndex = FILE_PATH_BEGIN; nIndex != FILE_PATH_END; ++nIndex)
		m_aszFilePath[nIndex][0] = _T('\0');

	m_szFilePath[0] = _T('\0');
}

KFilePath::~KFilePath()
{
}

LPCTSTR KFilePath::GetFilePath(int nIndex) const
{
	if (nIndex < FILE_PATH_BEGIN || nIndex >= FILE_PATH_END)
		return _T("");
	return m_aszFilePath[nIndex];
}

LPCTSTR KFilePath::GetFilePath(int nIndex, LPCTSTR szFileName) const
{
	ASSERT(szFileName);

	_sntprintf(m_szFilePath, _countof(m_szFilePath), _T("%s/%s"), GetFilePath(nIndex), szFileName);
	m_szFilePath[_countof(m_szFilePath) - 1] = _T('\0');

	return m_szFilePath;
}

LPCTSTR KFilePath::GetFilePath(int nIndex, LPCTSTR szFileName, LPCTSTR szFileExt, BOOL bExtHasDot, BOOL bHasSep) const
{
	ASSERT(szFileName);
	ASSERT(szFileExt);

	if (bHasSep)
	{
		if (bExtHasDot)
			_sntprintf(m_szFilePath, _countof(m_szFilePath), _T("%s%s%s"), GetFilePath(nIndex), szFileName, szFileExt);
		else
			_sntprintf(m_szFilePath, _countof(m_szFilePath), _T("%s%s.%s"), GetFilePath(nIndex), szFileName, szFileExt);
	}
	else
	{
		if (bExtHasDot)
			_sntprintf(m_szFilePath, _countof(m_szFilePath), _T("%s/%s%s"), GetFilePath(nIndex), szFileName, szFileExt);
		else
			_sntprintf(m_szFilePath, _countof(m_szFilePath), _T("%s/%s.%s"), GetFilePath(nIndex), szFileName, szFileExt);
	}

	m_szFilePath[_countof(m_szFilePath) - 1] = _T('\0');

	return m_szFilePath;
}

BOOL KFilePath::Init()
{
	int nRetCode = false;

	nRetCode = LoadFilePath();
	KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
	return nRetCode;
}

void KFilePath::Exit()
{
}

BOOL KFilePath::Reset()
{
	int nRetCode = false;

	Exit();

	nRetCode = Init();
	KGLOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	Exit();
	return FALSE;
}

BOOL KFilePath::MissFile(LPCTSTR szFilePath)
{
	ASSERT(szFilePath);

	KCountedStringMap& aMissFilePath = g_pRL->m_FilePath.m_aMissFilePath;
	_tstring const strFilePath(szFilePath);

	if (aMissFilePath.find(strFilePath) != aMissFilePath.end())
	{
		++aMissFilePath[strFilePath];
		return TRUE;
	}

	aMissFilePath.insert(std::make_pair(strFilePath, 1));
	KGLogPrintf(KGLOG_INFO, "RL file %s", szFilePath);
	return FALSE;
}

BOOL KFilePath::TabError(LPCTSTR szFmt, ...)
{
    TCHAR szBuffer[512];

	ASSERT(szFmt);

    va_list args;
    va_start(args, szFmt);
    _vsntprintf(szBuffer, _countof(szBuffer), szFmt, args);
    va_end(args);

	KCountedStringMap& aTabError = g_pRL->m_FilePath.m_aTabError;
	_tstring const strTabError(szBuffer);

	if (aTabError.find(strTabError) != aTabError.end())
	{
		++aTabError[strTabError];
		return TRUE;
	}

	aTabError.insert(std::make_pair(strTabError, 1));
	KGLogPrintf(KGLOG_INFO, "RL tab %s", szBuffer);
	return FALSE;
}

BOOL KFilePath::LoadFilePath()
{
	static LPCTSTR s_aszFileName[FILE_PATH_END] =
	{
        // Common
        _T("CommonNumber"),
        _T("CommonGloballEffect"),
        _T("CommonSoundTable"),
        _T("CommonForceRelationTable"),
        _T("CommonOptimizationTable"),
        _T("CommonCursorEffect"),

        // Player
        _T("PlayerDirectory"),
        _T("PlayerEquipDirectory"),
        _T("PlayerEquipSFXDirectory"),
		_T("PlayerEquipModel"),
		_T("PlayerAnimationModel"),
        _T("PlayerTable"),
        _T("PlayerAnimationRelationUpTable"),
        _T("PlayerAnimationRelationDownTable"),
        _T("PlayerBuffAnimationAdjustTable"),
        _T("PlayerSerialAnimationTable"),
        _T("PlayerCraftAnimationTable"),
        _T("PlayerUniformTable"),
        _T("PlayerRoleTypeUniformTable"),
        _T("PlayerForceUniformTable"),

        // Npc
        _T("NpcModel"),
        _T("NpcAnimationModel"),
        _T("NpcAnimationAdjustTable"),
        _T("NpcAnimationFusionTable"),
        _T("NpcSerialAnimationTable"),
        _T("NpcAnimationRelationTable"),
        _T("NpcUniformTable"),

        // Rides
        _T("RidesDirectory"),
        _T("RidesModel"),
        _T("RidesEquipModel"),
        _T("RidesAnimationModel"),
        _T("RidesAnimationFusion"),
        _T("RidesForRoad"),

        // Doodad
        _T("DoodadModel"),
        _T("DoodadUniform"),
        _T("DoodadEffectModel"),

        // Skill
        _T("SkillDirectory"),
        _T("SkillBuffModel"),
        _T("SkillChainModel"),
        _T("SkillCasterModel"),
        _T("SkillEffectModel"),
        _T("SkillResultTable"),
        _T("SkillMissileTable"),

        // Missile
        _T("MissileDirectory"),
		_T("MissilePhaseModel"),
        _T("MissileFixedTrackFrameTable"),   
	};

	int nRetCode = false;
	KSmartIniFile Ini;

    Ini = g_OpenIniFile(s_szFilePath);
	KGLOG_PROCESS_ERROR(Ini);

	for (int nIndex = FILE_PATH_BEGIN; nIndex < FILE_PATH_END; ++nIndex)
	{
		LPCTSTR szKey = s_aszFileName[nIndex];

		m_aszFilePath[nIndex][_countof(m_aszFilePath[nIndex]) - 1] = _T('\0');
		nRetCode = Ini->GetString(s_szFilePathSection, szKey, "", m_aszFilePath[nIndex], FILE_PATH_LEN);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "Represent : load file error. [%s]\n", szKey);
			continue;
		}

		nRetCode = m_aszFilePath[nIndex][_countof(m_aszFilePath[nIndex]) - 1] == _T('\0');
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}
