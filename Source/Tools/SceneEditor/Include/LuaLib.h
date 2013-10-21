/*
** $Id: LuaLib.h,v 1.1 2005/06/20 03:16:39 zhujianqiu Exp $
** Lua - An Extensible Extension Language
** TeCGraf: Grupo de Tecnologia em Computacao Grafica, PUC-Rio, Brazil
** e-mail: lua@tecgraf.puc-rio.br
** www: http://www.tecgraf.puc-rio.br/lua/
** See Copyright Notice at the end of this file
*/


#ifndef lua_h
#define lua_h


/* definition of `size_t' */
#include <stddef.h>

#ifdef __linux
	#define LUA_API
	#define	LUALIB_API
#else
#ifdef _USELUALIB
	#ifndef LUA_API
	#define LUA_API		extern
	#define	LUALIB_API	extern
	#endif
#else
	#define LUA_API extern __declspec(dllimport)
	#define LUALIB_API extern __declspec(dllimport)
#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif 

#define LUA_VERSION	"Lua 4.0"
#define LUA_COPYRIGHT	"Copyright (C) 1994-2000 TeCGraf, PUC-Rio"
#define LUA_AUTHORS 	"W. Celes, R. Ierusalimschy & L. H. de Figueiredo"


/* name of global variable with error handler */
#define LUA_ERRORMESSAGE	"_ERRORMESSAGE"


/* pre-defined references */
#define LUA_NOREF	(-2)
#define LUA_REFNIL	(-1)
#define LUA_REFREGISTRY	0

/* pre-defined tags */
#define LUA_ANYTAG	(-1)
#define LUA_NOTAG	(-2)


/* option for multiple returns in lua_call */
#define LUA_MULTRET	(-1)


/* minimum stack available for a C function */
#define LUA_MINSTACK	20


/* error codes for lua_do* */
#define LUA_ERRRUN	1
#define LUA_ERRFILE	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5


//##ModelId=3BA83F3A02ED
typedef struct lua_State lua_State;

//##ModelId=3BA83F3A02FD
typedef int (*lua_CFunction) (lua_State *L);

/*
** types returned by `lua_type'
*/
#define LUA_TNONE	(-1)

#define LUA_TUSERDATA	0
#define LUA_TNIL	1
#define LUA_TNUMBER	2
#define LUA_TSTRING	3
#define LUA_TTABLE	4
#define LUA_TFUNCTION	5



/*
** state manipulation
*/
LUA_API lua_State *lua_open (int stacksize);
LUA_API void       lua_close (lua_State *L);


/*
** basic stack manipulation
*/
LUA_API int   lua_gettop (lua_State *L);
LUA_API void  lua_settop (lua_State *L, int index);
LUA_API void  lua_pushvalue (lua_State *L, int index);
LUA_API void  lua_remove (lua_State *L, int index);
LUA_API void  lua_insert (lua_State *L, int index);
LUA_API int   lua_stackspace (lua_State *L);

/*
** debug out functions
*/
LUA_API int	  lua_setdebugout(const char * , const char *);
LUA_API void lua_outoutmsg(const char * szoutmsg);
LUA_API void lua_outerrmsg(const char * szerrmsg);


/*
** access functions (stack -> C)
*/

LUA_API int            lua_type (lua_State *L, int index);
LUA_API const char    *lua_typename (lua_State *L, int t);
LUA_API int            lua_isnumber (lua_State *L, int index);
LUA_API int            lua_isstring (lua_State *L, int index);
LUA_API int            lua_iscfunction (lua_State *L, int index);
LUA_API int            lua_tag (lua_State *L, int index);

LUA_API int            lua_equal (lua_State *L, int index1, int index2);
LUA_API int            lua_lessthan (lua_State *L, int index1, int index2);

LUA_API double         lua_tonumber (lua_State *L, int index);
LUA_API const char    *lua_tostring (lua_State *L, int index);
LUA_API size_t         lua_strlen (lua_State *L, int index);
LUA_API lua_CFunction  lua_tocfunction (lua_State *L, int index);
LUA_API void	      *lua_touserdata (lua_State *L, int index);
LUA_API const void    *lua_topointer (lua_State *L, int index);


/*
** push functions (C -> stack)
*/
LUA_API void  lua_pushnil (lua_State *L);
LUA_API void  lua_pushnumber (lua_State *L, double n);
LUA_API void  lua_pushlstring (lua_State *L, const char *s, size_t len);
LUA_API void  lua_pushstring (lua_State *L, const char *s);
LUA_API void  lua_pushcclosure (lua_State *L, lua_CFunction fn, int n);
LUA_API void  lua_pushusertag (lua_State *L, void *u, int tag);


/*
** get functions (Lua -> stack)
*/
LUA_API void  lua_getglobal (lua_State *L, const char *name);
LUA_API void  lua_gettable (lua_State *L, int index);
LUA_API void  lua_rawget (lua_State *L, int index);
LUA_API void  lua_rawgeti (lua_State *L, int index, int n);
LUA_API void  lua_getglobals (lua_State *L);
LUA_API void  lua_gettagmethod (lua_State *L, int tag, const char *event);
LUA_API int   lua_getref (lua_State *L, int ref);
LUA_API void  lua_newtable (lua_State *L);


/*
** set functions (stack -> Lua)
*/
LUA_API void  lua_setglobal (lua_State *L, const char *name);
LUA_API void  lua_settable (lua_State *L, int index);
LUA_API void  lua_rawset (lua_State *L, int index);
LUA_API void  lua_rawseti (lua_State *L, int index, int n);
LUA_API void  lua_setglobals (lua_State *L);
LUA_API void  lua_settagmethod (lua_State *L, int tag, const char *event);
LUA_API int   lua_ref (lua_State *L, int lock);


/*
** "do" functions (run Lua code)
*/
LUA_API int   lua_call (lua_State *L, int nargs, int nresults);
LUA_API void  lua_rawcall (lua_State *L, int nargs, int nresults);
LUA_API int   lua_dofile (lua_State *L, const char *filename);
LUA_API int   lua_dostring (lua_State *L, const char *str);
LUA_API int   lua_dobuffer (lua_State *L, const char *buff, size_t size, const char *name);

/*
** Garbage-collection functions
*/
LUA_API int   lua_getgcthreshold (lua_State *L);
LUA_API int   lua_getgccount (lua_State *L);
LUA_API void  lua_setgcthreshold (lua_State *L, int newthreshold);

/*
** miscellaneous functions
*/
LUA_API int   lua_newtag (lua_State *L);
LUA_API int   lua_copytagmethods (lua_State *L, int tagto, int tagfrom);
LUA_API void  lua_settag (lua_State *L, int tag);

LUA_API void  lua_error (lua_State *L, const char *s);

LUA_API void  lua_unref (lua_State *L, int ref);

LUA_API int   lua_next (lua_State *L, int index);
LUA_API int   lua_getn (lua_State *L, int index);

LUA_API void  lua_concat (lua_State *L, int n);

LUA_API void *lua_newuserdata (lua_State *L, size_t size);
LUA_API int ldo (int (*f)(lua_State *l, const char *), const char *name) ;


/* 
** ===============================================================
** some useful macros
** ===============================================================
*/

#define lua_pop(L,n)		lua_settop(L, -(n)-1)

#define lua_register(L,n,f)	(lua_pushcfunction(L, f), lua_setglobal(L, n))
#define lua_pushuserdata(L,u)	lua_pushusertag(L, u, 0)
#define lua_pushcfunction(L,f)	lua_pushcclosure(L, f, 0)
#define lua_clonetag(L,t)	lua_copytagmethods(L, lua_newtag(L), (t))

#define lua_isfunction(L,n)	(lua_type(L,n) == LUA_TFUNCTION)
#define lua_istable(L,n)	(lua_type(L,n) == LUA_TTABLE)
#define lua_isuserdata(L,n)	(lua_type(L,n) == LUA_TUSERDATA)
#define lua_isnil(L,n)		(lua_type(L,n) == LUA_TNIL)
#define lua_isnull(L,n)		(lua_type(L,n) == LUA_TNONE)

#define lua_getregistry(L)	lua_getref(L, LUA_REFREGISTRY)

//新加的API
LUA_API int lua_compilebuffer(lua_State *L, const char *buff, size_t size, const char *name);//只对buffer经编绎，并不执行任何函数
LUA_API int lua_compilefile (lua_State *L, const char *filename);//编译从文件流传来的脚本
LUA_API int lua_execute(lua_State *L);//执行经过编绎之后的脚本
LUA_API void lua_gettopindex(lua_State *L , int * pindex);//获得当前的脚本堆栈顶的索引


//BaseLib


//脚本引擎
#define Lua_CFunction					lua_CFunction
#define Lua_State						lua_State
#define Lua_Create(nSize)				lua_open(nSize)
#define Lua_Release(L)					lua_close(L)
#define Lua_GetTopIndex(L)				lua_gettop(L)
#define Lua_SetTopIndex(L,nIndex)		lua_settop(L,nIndex)
#define Lua_PushValue(L,nIndex)			lua_pushvalue(L,nIndex)
#define Lua_RemoveValue(L,nIndex)		lua_remove(L,nIndex)
#define Lua_InsertValue(L,nIndex)		lua_insert(L,nIndex)
#define Lua_GetStackSpace(L)			lua_stackspace(L)
#define Lua_GetValueType(L,nIndex)		lua_type(L,nIndex)
#define Lua_GetTypeName(L,Tag)			lua_typename(L,Tag)
#define Lua_IsNumber(L,nIndex)			lua_isnumber(L,nIndex)
#define Lua_IsString(L,nIndex)			lua_isstring(L,nIndex)
#define Lua_IsCFunction(L,nIndex)		lua_iscfunction(L,nIndex)
#define Lua_IsTable(L,nIndex)			lua_istable(L,nIndex)
#define Lua_GetValueTag(L,nIndex)		lua_tag(L,nIndex)
#define Lua_IsEqual(L,index1,index2)	lua_equal(L,index1,index2)
#define Lua_IsLessThan(L,index1,index2)	lua_lessthan(L,index1,index2)
#define Lua_ValueToNumber(L,nIndex)		lua_tonumber(L,nIndex)
#define Lua_ValueToString(L,nIndex)		lua_tostring(L,nIndex)
#define Lua_GetStrLen(L,nIndex)			lua_strlen(L,nIndex)
#define Lua_ValueToCFunction(L,nIndex)	lua_tocfunction(L,nIndex)
#define Lua_ValueToUserData(L,nIndex)	lua_touserdata(L,nIndex)
#define Lua_ValueToPoint(L,nIndex)		lua_topointer(L,nIndex)

/*
** push functions (C -> stack)
*/

#define Lua_PushNil(L)					lua_pushnil(L)
#define Lua_PushNumber(L,Number)		lua_pushnumber(L,Number)
#define Lua_PushLString(L,LString,Len)	lua_pushlstring(L,LString,Len)
#define Lua_PushString(L,String)		lua_pushstring(L,String)
#define Lua_PushCClosure(L,Fun,N)		lua_pushcclosure(L,Fun,N)
#define Lua_PushUserTag(L,PVoid,Tag)	lua_pushusertag(L,PVoid,Tag)

#define Lua_GetGlobal(L,Valuename)		lua_getglobal(L,Valuename)
#define Lua_GetTable(L,nIndex)			lua_gettable(L,nIndex)
#define Lua_RawGet(L,nIndex)			lua_rawget(L,nIndex)
#define Lua_RawGetI(L,nIndex,n)			lua_rawgeti(L,nIndex,n)
#define Lua_GetGlobals(L)				lua_getglobals(L)
#define Lua_GetTagMethod(L,iTag,cEvent)	lua_gettagmethod(L,iTag,cEvent)
#define Lua_GetRef(L,iRef)				lua_getref(L,iRef)
#define Lua_NewTable(L)					lua_newtable(L)


#define Lua_SetGlobal(L,cName)			lua_setglobal(L,cName)
#define Lua_SetTable(L,nIndex)			lua_settable(L,nIndex)
#define Lua_RawSet(L,nIndex)			lua_rawset(L,nIndex)
#define Lua_RawSetI(L,nIndex,nNum)		lua_rawseti(L,nIndex,nNum)
#define Lua_SetGlobals(L)				lua_setglobals(L)
#define Lua_SetTagMethod(L,iTag,cEvent)	lua_settagmethod(L,iTag,cEvent)
#define Lua_SetRef(L,nLock)				lua_ref(L,nLock)

#define Lua_Call(L,nArgs,nResults)		lua_call(L,nArgs,nResults)
#define Lua_RawCall(L,nArgs,nResults)	lua_rawcall(L,nArgs,nResults)
#define	Lua_ExecuteFile(L,cFilename)	lua_dofile(L,cFilename)
#define Lua_ExecuteString(L,cString)	lua_dostring(L,cString)
#define Lua_ExecuteBuffer(L,pBuff,iSize,cname)	lua_dobuffer(L,pBuff,iSize,cname)

#define Lua_GetGCThreshold(L)			lua_getgcthreshold(L)
#define Lua_GetGCCount(L)				lua_getgccount(L)
#define Lua_SetGCThreshold(L,nThreshold)lua_setgcthreshold(L,nThreshold)


/*
** miscellaneous functions
*/

#define Lua_NewTag(L)					lua_newtag(L)
#define Lua_CopyTagMethods(L,nTagTo,nTagFrom)	lua_copytagmethods(L,nTagTo,nTagFrom)
#define Lua_SetTag(L,nTag)				lua_settag(L,nTag)
#define	Lua_Error(L,cError)				lua_error(L,cError)u
#define Lua_UnRef(L,nRef)				lua_unref(L,nRef)
#define Lua_Next(L,nIndex)				lua_next(L,nIndex)
#define Lua_GetN(L,nIndex)				lua_getn(L,nIndex)
#define Lua_Concat(L,nNum)				lua_concat(L,nNum)
#define Lua_NewUserData(L,nSize)		lua_newuserdata(L,nSize)


#define Lua_OpenBaseLib(L)				 lua_baselibopen(L)
#define Lua_OpenIOLib(L)				 lua_iolibopen(L)
#define Lua_OpenStrLib(L)				 lua_strlibopen(L)
#define Lua_OpenMathLib(L)				 lua_mathlibopen(L)
#define Lua_OpenDBLib(L)				 lua_dblibopen(L)


/* 
** ===============================================================
** some useful macros
** ===============================================================
*/
#define Lua_Pop(L,nIndex)				lua_pop(L,nIndex)
#define Lua_Register(L,cfname,pFun)		lua_register(L,cfname,pFun)
#define Lua_PushUserData(L,UseData)	lua_pushuserdata(L,UseData)
#define Lua_PushCFunction(L,pFun)	lua_pushcfunction(L,pFun)
#define Lua_CloneTag(L,nTag)		lua_clonetag(L,nTag)
#define Lua_GetRegistry(L)			lua_getregistry(L)


#define Lua_CompileBuffer(L,pBUFF,nBuffSize,cBname)	lua_compilebuffer(L,pBUFF,nBuffSize,cBname)
#define Lua_CompileFile(L,cFilename)		lua_compilefile(L,cFilename)
#define Lua_Execute(L)				lua_execute(L)
#define Lua_SafeBegin(L,pIndex)		lua_gettopindex(L,pIndex)
#define Lua_SafeEnd(L,nIndex)		Lua_SetTopIndex(L,nIndex)


LUA_API int Lua_SetTable_StringFromId(Lua_State * L, int nIndex, int Id, const char * szString);
LUA_API int Lua_SetTable_DoubleFromId(Lua_State * L, int nIndex, int Id, double nNumber);
LUA_API int Lua_SetTable_IntFromId(Lua_State * L, int nIndex, int Id, int nNumber);
LUA_API int Lua_SetTable_CFunFromName(Lua_State * L, int nIndex, const char * szMemberName, Lua_CFunction CFun);
LUA_API int Lua_SetTable_CFunFromId(Lua_State * L, int nIndex, int nId, Lua_CFunction CFun);
LUA_API int Lua_SetTable_StringFromName(Lua_State * L, int nIndex, const char * szMemberName, char * szString);
LUA_API int Lua_SetTable_IntFromName(Lua_State * L, int nIndex, const char * szMemberName, int Number);
LUA_API int Lua_SetTable_DoubleFromName(Lua_State * L, int nIndex, const char * szMemberName, double Number);
LUA_API int Lua_GetValuesFromStack(Lua_State * L, char * cFormat , ...);

LUALIB_API void lua_baselibopen (lua_State *L);
LUALIB_API void lua_iolibopen (lua_State *L);
LUALIB_API void lua_strlibopen (lua_State *L);
LUALIB_API void lua_mathlibopen (lua_State *L);
LUALIB_API void lua_dblibopen (lua_State *L);


#ifdef __cplusplus
}
#endif 

#endif



/******************************************************************************
* Copyright (C) 1994-2000 TeCGraf, PUC-Rio.  All rights reserved.
* 
* Permission is hereby granted, without written agreement and without license
* or royalty fees, to use, copy, modify, and distribute this software and its
* documentation for any purpose, including commercial applications, subject to
* the following conditions:
* 
*  - The above copyright notice and this permission notice shall appear in all
*    copies or substantial portions of this software.
* 
*  - The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software in a
*    product, an acknowledgment in the product documentation would be greatly
*    appreciated (but it is not required).
* 
*  - Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
*    
* The authors specifically disclaim any warranties, including, but not limited
* to, the implied warranties of merchantability and fitness for a particular
* purpose.  The software provided hereunder is on an "as is" basis, and the
* authors have no obligation to provide maintenance, support, updates,
* enhancements, or modifications.  In no event shall TeCGraf, PUC-Rio, or the
* authors be held liable to any party for direct, indirect, special,
* incidental, or consequential damages arising out of the use of this software
* and its documentation.
* 
* The Lua language and this implementation have been entirely designed and
* written by Waldemar Celes Filho, Roberto Ierusalimschy and
* Luiz Henrique de Figueiredo at TeCGraf, PUC-Rio.
*
* This implementation contains no third-party code.
******************************************************************************/

