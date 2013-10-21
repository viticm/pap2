#include "stdafx.h"
#include "SO3UI/IKGUI.h"
#include "./kgwtextencoder.h"
#include "./kschemescripttable.h"
#include "./SO3UI.h"
#include "ISO3Interaction.h"
#include "./kgameworldmgr.h"

static size_t TripSlashes(char* szBuffer)
{
    //TODO:可能在解析某些中文字符时会出错!

    ASSERT(szBuffer);

    LPSTR szBegin = szBuffer;
    LPSTR szCurrent = NULL;

    szCurrent = szBuffer;
    while (true)
    {
        if (szBuffer[0] == '\\')
        {
            BOOL bTripped = TRUE;

            switch (szBuffer[1])
            {
            case 'n':
                szCurrent[0] = '\n';
                break;
            case 't':
                szCurrent[0] = '\t';
                break;
            case '\\':
                szCurrent[0] = '\\';
                break;
            case '\"':
                szCurrent[0] = '\"';
                break;
            default:
                bTripped = FALSE;
                break;
            }
            if (bTripped)
            {
                ++szCurrent;
                szBuffer += 2;
                continue;
            }
        }
        if (szBuffer[0] == '\0')
            break;
        LPCSTR szNext = ::CharNextA(szBuffer);
        if (szNext == NULL)
            break;
        while (szNext != szBuffer)
        {
            *szCurrent++ = *szBuffer++;
        }
    }
    szCurrent[0] = '\0';

    return (size_t)(szCurrent - szBegin + 1);
}


void KSchemeScriptTable::Load()
{
	static luaL_reg s_aGlobalFunctionTable[] =
    {
		{ "GWTextEncoder_Encode",				LuaGWTextEncoder_Encode },
		{ "GWTextEncoder_EncodeTalkData",		LuaGWTextEncoder_EncodeTalkData },

		{ "Interaction_Clear",					LuaInteraction_Clear },
		{ "Interaction_AddParam",				LuaInteraction_AddParam },
		{ "Interaction_Send",					LuaInteraction_Send },
		{ "Interaction_Request",				LuaInteraction_Request },

        { "AttributeStringToID",                LuaAttributeStringToID },
        { "RequireStringToID",                  LuaRequireStringToID },

		{ NULL,									NULL },
		
	};

	g_pSO3UI->m_pUI->RegisterFunctions(s_aGlobalFunctionTable);
}

int KSchemeScriptTable::LuaGWTextEncoder_Encode(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	LPCSTR szStream = NULL;
	size_t nEncodedLength = 0;
	int nIndex = 1;
	KGWTextEncoder::KGWTextAtomList aEncodedAtom;
    std::string szEncode;
    unsigned int uCodePage = 0;

	ASSERT(L);

    ASSERT(g_pSO3UI && g_pSO3UI->m_pUI);
    uCodePage = g_pSO3UI->m_pUI->GetCodePage();

	bRetCode = Lua_GetTopIndex(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	szStream = Lua_ValueToString(L, 1);
	KGLOG_PROCESS_ERROR(szStream);

    szEncode = szStream;

	TripSlashes(&szEncode[0]);

    {
        ATL::CA2W wszStream(szEncode.c_str(), uCodePage);

		size_t nStreamLength = wcslen(wszStream);

        bRetCode = KGWTextEncoder::GetSelf().Encode(wszStream, nStreamLength, aEncodedAtom, nEncodedLength);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

	Lua_PushNumber(L, nEncodedLength);

	if (aEncodedAtom.size() > 0)
	{
		Lua_NewTable(L);

		KGWTextEncoder::KGWTextAtomList::const_iterator it = aEncodedAtom.begin();
		KGWTextEncoder::KGWTextAtomList::const_iterator ie = aEncodedAtom.end();
		for (; it != ie; ++it)
		{
			KGWTextAtom const& item = *it;
			LPCWSTR wszName = item.GetName();
			LPCWSTR wszContext = item.GetContext();

			Lua_PushNumber(L, nIndex);

			Lua_NewTable(L);

			if (wszName)
			{
				Lua_PushString(L, "name");
                Lua_PushString(L, ATL::CW2A(wszName, uCodePage));
				Lua_SetTable(L, -3);
			}
			if (wszContext)
			{
				Lua_PushString(L, "context");
                Lua_PushString(L, ATL::CW2A(wszContext, uCodePage));
				Lua_SetTable(L, -3);
			}

			size_t const nCount = item.GetAttributeCount();
			if (nCount > 0)
			{
				Lua_PushString(L, "attribute");

				Lua_NewTable(L);

				for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
				{
					LPCWSTR wszKey = item.GetAttributeKey(nIndex);
					LPCWSTR wszValue = item.GetAttributeValue(nIndex);

					ASSERT(wszKey && wszKey[0] != L'\0');

                    Lua_PushString(L, ATL::CW2A(wszKey, uCodePage));
                    Lua_PushString(L, wszValue ? ATL::CW2A(wszValue, uCodePage) : "");
					Lua_SetTable(L, -3);
				}

				Lua_SetTable(L, -3);
			}

			Lua_SetTable(L, -3);

			++nIndex;
		}
	}
	else
	{
		Lua_PushNil(L);
	}

	return 2;
Exit0:
	return 0;
}

int KSchemeScriptTable::LuaGWTextEncoder_EncodeTalkData(Lua_State *L)
{
	BOOL bRetCode = FALSE;
	LPCSTR szStream = NULL;
	size_t nEncodedLength = 0;
	int nIndex = 1;
	KGWTextEncoder::KGWTextAtomList aEncodedAtom;
    unsigned int uCodePage = 0;

	ASSERT(L);
    ASSERT(g_pSO3UI && g_pSO3UI->m_pUI);
    uCodePage = g_pSO3UI->m_pUI->GetCodePage();

	bRetCode = Lua_GetTopIndex(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	szStream = Lua_ValueToString(L, 1);
	KGLOG_PROCESS_ERROR(szStream);

	{
		ATL::CA2W wszStream(szStream, uCodePage);

		size_t nStreamLength = wcslen(wszStream);

		bRetCode = KGWTextEncoder::GetTalkEncoder().Encode(wszStream, nStreamLength, aEncodedAtom, nEncodedLength);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	Lua_PushNumber(L, nEncodedLength);

	if (aEncodedAtom.size() > 0)
	{
		Lua_NewTable(L);

		KGWTextEncoder::KGWTextAtomList::const_iterator it = aEncodedAtom.begin();
		KGWTextEncoder::KGWTextAtomList::const_iterator ie = aEncodedAtom.end();
		for (; it != ie; ++it)
		{
			KGWTextAtom const& item = *it;
			LPCWSTR wszName = item.GetName();
			LPCWSTR wszContext = item.GetContext();

			Lua_PushNumber(L, nIndex);

			Lua_NewTable(L);

			if (wszName)
			{
				Lua_PushString(L, "name");
				Lua_PushString(L, ATL::CW2A(wszName, uCodePage));
				Lua_SetTable(L, -3);
			}
			if (wszContext)
			{
				Lua_PushString(L, "context");
				Lua_PushString(L, ATL::CW2A(wszContext, uCodePage));
				Lua_SetTable(L, -3);
			}

			size_t const nCount = item.GetAttributeCount();
			if (nCount > 0)
			{
				Lua_PushString(L, "attribute");

				Lua_NewTable(L);

				for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
				{
					LPCWSTR wszKey = item.GetAttributeKey(nIndex);
					LPCWSTR wszValue = item.GetAttributeValue(nIndex);

					ASSERT(wszKey && wszKey[0] != L'\0');

					Lua_PushString(L, ATL::CW2A(wszKey, uCodePage));
					Lua_PushString(L, wszValue ? ATL::CW2A(wszValue, uCodePage) : "");
					Lua_SetTable(L, -3);
				}

				Lua_SetTable(L, -3);
			}

			Lua_SetTable(L, -3);

			++nIndex;
		}
	}
	else
	{
		Lua_PushNil(L);
	}

	return 2;
Exit0:
	return 0;
}

int KSchemeScriptTable::LuaInteraction_Clear(Lua_State *L)
{
	g_pSO3UI->m_pInteraction->Clear();
	return 0;
}

int KSchemeScriptTable::LuaInteraction_AddParam(Lua_State *L)
{
	const char * pcszName = NULL;
	const char * pcszValue = NULL;
	BOOL bFile = FALSE;
	int nParamCount = Lua_GetTopIndex(L);
	int nRetCode = false;
	KGLOG_PROCESS_ERROR(nParamCount == 2 || nParamCount == 3);

	pcszName = Lua_ValueToString(L, 1);
	KGLOG_PROCESS_ERROR(pcszName);

	pcszValue = Lua_ValueToString(L, 2);
	KGLOG_PROCESS_ERROR(pcszValue);

	if (nParamCount == 3)
	{
		if (lua_isboolean(L, 3))
			bFile = lua_toboolean(L, 3);
		else
			bFile = (BOOL)(int)lua_tonumber(L, 3);
	}

	nRetCode = g_pSO3UI->m_pInteraction->AddParam(pcszName, pcszValue, bFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	lua_pushboolean(L, TRUE);
	return 1;
Exit0:
	lua_pushboolean(L, FALSE);
	return 1;
}

int KSchemeScriptTable::LuaInteraction_Send(Lua_State *L)
{
    int nRetCode = false;
	const char * pcszName = NULL;
    const char * pcszAddress = NULL;
    const char * pcszObjectName = NULL;
	const char * pcszVerb = NULL;
	int nPort = 0;
	int nParamCount = Lua_GetTopIndex(L);

	KGLOG_PROCESS_ERROR(nParamCount == 5);

	pcszName = Lua_ValueToString(L, 1);
	KGLOG_PROCESS_ERROR(pcszName);

	pcszAddress = Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pcszAddress);

    pcszObjectName = Lua_ValueToString(L, 3);
    KGLOG_PROCESS_ERROR(pcszObjectName);

	pcszVerb = Lua_ValueToString(L, 4);
	KGLOG_PROCESS_ERROR(pcszVerb);

	nPort = (int)Lua_ValueToNumber(L, 5);

	nRetCode = g_pSO3UI->m_pInteraction->Send(pcszName, pcszAddress, pcszObjectName, pcszVerb, nPort, &Interaction_SendResult);
	KGLOG_PROCESS_ERROR(nRetCode);

	lua_pushboolean(L, TRUE);
	return 1;
Exit0:
	lua_pushboolean(L, FALSE);
	return 1;
}

int KSchemeScriptTable::LuaInteraction_Request(Lua_State *L)
{
    int nRetCode = false;
	const char * pcszName = NULL;
    const char * pcszAddress = NULL;
    const char * pcszObjectName = NULL;
	const char * pcszVerb = NULL;
	int nPort = 0;
	int nParamCount = Lua_GetTopIndex(L);

	KGLOG_PROCESS_ERROR(nParamCount == 5);

	pcszName = Lua_ValueToString(L, 1);
	KGLOG_PROCESS_ERROR(pcszName);

	pcszAddress = Lua_ValueToString(L, 2);
	KGLOG_PROCESS_ERROR(pcszAddress);

    pcszObjectName = Lua_ValueToString(L, 3);
    KGLOG_PROCESS_ERROR(pcszObjectName);

	pcszVerb = Lua_ValueToString(L, 4);
    KGLOG_PROCESS_ERROR(pcszVerb);

	nPort = (int)Lua_ValueToNumber(L, 5);

	nRetCode = g_pSO3UI->m_pInteraction->Request(pcszName, pcszAddress, pcszObjectName, pcszVerb, nPort, &Interaction_RequestResult);
	KGLOG_PROCESS_ERROR(nRetCode);

	lua_pushboolean(L, TRUE);
	return 1;
Exit0:
	lua_pushboolean(L, FALSE);
	return 1;
}

void KSchemeScriptTable::Interaction_SendResult(char *pszName, int nResult)
{
	if (!pszName)
		pszName = "\0";
	g_pSO3UI->m_pUI->PushArg("arg0", pszName);
	g_pSO3UI->m_pUI->PushArg("arg1", nResult ? true : false);
	g_pSO3UI->m_pUI->FireEvent("INTERACTION_SEND_RESULT", 0);
}

void KSchemeScriptTable::Interaction_RequestResult(char *pszName, int nResult, BYTE *pbyBuffer, DWORD dwBufferSize)
{
	if (!pszName)
		pszName = "\0";
	char * pszValue = (char *)pbyBuffer;
	if (!pszValue)
		pszValue = "\0";

	g_pSO3UI->m_pUI->PushArg("arg0", pszName);
	g_pSO3UI->m_pUI->PushArg("arg1", nResult ? true : false);
	g_pSO3UI->m_pUI->PushArg("arg2", pszValue);
	g_pSO3UI->m_pUI->PushArg("arg3", (int)dwBufferSize);
	g_pSO3UI->m_pUI->FireEvent("INTERACTION_REQUEST_RESULT", 0);
}

int KSchemeScriptTable::LuaAttributeStringToID(lua_State *pLua)
{
    int nRetCode = false;
    int nRetParamCount = 0;

    const char *pcszAttributeName = NULL;
    int nID = 0;

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = lua_gettop(pLua);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pcszAttributeName = lua_tostring(pLua, 1);
    KGLOG_PROCESS_ERROR(pcszAttributeName);

    nRetCode = g_GameWorldMgr.AttributeStringToID(pcszAttributeName, nID);
    KGLOG_PROCESS_ERROR(nRetCode);

    lua_pushnumber(pLua, nID);
    nRetParamCount++;

Exit0:
    return nRetParamCount;
}

int KSchemeScriptTable::LuaRequireStringToID(lua_State *pLua)
{
    int nRetCode = false;
    int nRetParamCount = 0;

    const char *pcszRequireName = NULL;
    int nID = 0;

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = lua_gettop(pLua);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pcszRequireName = lua_tostring(pLua, 1);
    KGLOG_PROCESS_ERROR(pcszRequireName);

    nRetCode = g_GameWorldMgr.RequireStringToID(pcszRequireName, nID);
    KGLOG_PROCESS_ERROR(nRetCode);

    lua_pushnumber(pLua, nID);
    nRetParamCount++;

Exit0:
    return nRetParamCount;
}


