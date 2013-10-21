#include "stdafx.h"
#include "./kgameworldscripttable.h"
#include "./kscriptloader.h"
#include "../SO3UI.h"

namespace UI
{

	void KGameWorldScriptTable::Load(lua_State* L)
	{
		KLuaFunc* pBegin = NULL;
		int nCount = 0;

		nCount = SO3UI::GetPluginInitGameWorld().fnBaseFuncList(pBegin);
		if (nCount > 0 && pBegin != NULL)
			KScriptLoader::RegisterFunctions(L, pBegin, nCount);

		nCount = SO3UI::GetPluginInitGameWorld().fnPartyFuncList(pBegin);
		if (nCount > 0 && pBegin != NULL)
			KScriptLoader::RegisterFunctions(L, pBegin, nCount);

		nCount = SO3UI::GetPluginInitGameWorld().fnShopFuncList(pBegin);
		if (nCount > 0 && pBegin != NULL)
			KScriptLoader::RegisterFunctions(L, pBegin, nCount);

		nCount = SO3UI::GetPluginInitGameWorld().fnLoginFuncList(pBegin);
		if (nCount > 0 && pBegin != NULL)
			KScriptLoader::RegisterFunctions(L, pBegin, nCount);
	}

}

