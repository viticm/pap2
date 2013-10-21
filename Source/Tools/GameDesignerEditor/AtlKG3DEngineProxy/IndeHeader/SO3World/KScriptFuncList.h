/************************************************************************/
/* LUA½Å±¾º¯Êý					                                        */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.03.02	Create												*/
/************************************************************************/
#ifndef _SCRIPT_FUNC_LIST_H_
#define _SCRIPT_FUNC_LIST_H_

#include "Engine/KLuaScriptEx.h"

namespace KScriptFuncList
{
	int GetBaseFuncList(KLuaFunc*& pFuncList);
    int GetShopFuncList(KLuaFunc*& pFuncList);
    int GetLoginFuncList(KLuaFunc*& pFuncList);

}	//end namespace KScriptFuncList



#endif	//_SCRIPT_FUNC_LIST_H_
