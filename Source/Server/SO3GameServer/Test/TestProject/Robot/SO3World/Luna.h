/************************************************************************/
/* C Class -> Lua Class 映射											*/
/* Copyright : Kingsoft 2006										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2006.02.28	Create												*/
/************************************************************************/
#ifndef _LUNA_H_
#define _LUNA_H_

#include "Engine/KLuaScriptEx.h"

//映射相关宏定义

//申明该类需要导出变量或者函数给Lua使用
#define DECLARE_LUA_CLASS(__CLASS_NAME__)									\
	static const char szClassName[];										\
	static Luna<__CLASS_NAME__>::KLuaData LuaInterface[];					\
	int LuaGetObj(Lua_State* L)												\
	{																		\
		Luna<__CLASS_NAME__>::KObjData *pObjData =							\
			static_cast<Luna<__CLASS_NAME__>::KObjData*>					\
			(lua_newuserdata(L, sizeof(Luna<__CLASS_NAME__>::KObjData)));	\
		if (pObjData == NULL)												\
			return 0;														\
		pObjData->pObj = this;												\
		luaL_getmetatable(L, __CLASS_NAME__::szClassName);					\
		if (lua_isnil(L, -1))												\
			return 0;														\
		lua_setmetatable(L, -2);											\
		return 1;															\
	}

/************************************************************************/

//申明导出该整形变量
#define DECLARE_LUA_INTEGER(__VAR_NAME__)									\
	int get##__VAR_NAME__(void) {return m_n##__VAR_NAME__;};				\
	void set##__VAR_NAME__(int nValue)	{m_n##__VAR_NAME__ = nValue;}

//申明导出该无符号整形变量
#define DECLARE_LUA_DWORD(__VAR_NAME__)										\
	DWORD get##__VAR_NAME__(void) {return m_dw##__VAR_NAME__;};				\
	void set##__VAR_NAME__(DWORD dwValue)	{m_dw##__VAR_NAME__ = dwValue;}

//申明导出该字符串
#define DECLARE_LUA_STRING(__VAR_NAME__, __STRING_LEN__)					\
	const char* get##__VAR_NAME__(void) {return m_sz##__VAR_NAME__;};		\
	void set##__VAR_NAME__(const char* pszValue)							\
	{strncpy(m_sz##__VAR_NAME__, pszValue, __STRING_LEN__);					\
	 m_sz##__VAR_NAME__[__STRING_LEN__ - 1] = 0;}

//申明导出该枚举型变量
#define DECLARE_LUA_ENUM(__VAR_NAME__)										\
	int get##__VAR_NAME__(void) {return (int)m_e##__VAR_NAME__;};			\
	void set##__VAR_NAME__(int nValue)	{*(int*)&m_e##__VAR_NAME__ = nValue;}

//申明导出该布尔型变量
#define DECLARE_LUA_BOOL(__VAR_NAME__)										\
	int get##__VAR_NAME__(void) {return (int)m_b##__VAR_NAME__;};			\
	void set##__VAR_NAME__(int nValue)	{*(int*)&m_b##__VAR_NAME__ = nValue;}

/************************************************************************/

//申明导出该整形变量
#define DECLARE_LUA_STRUCT_INTEGER(__VAR_NAME__, __REAL_NAME__)				\
	int get##__VAR_NAME__(void) {return __REAL_NAME__;};					\
	void set##__VAR_NAME__(int nValue)	{__REAL_NAME__ = nValue;}

//申明导出该无符号整形变量
#define DECLARE_LUA_STRUCT_DWORD(__VAR_NAME__, __REAL_NAME__)				\
	DWORD get##__VAR_NAME__(void) {return __REAL_NAME__;};					\
	void set##__VAR_NAME__(DWORD dwValue)	{__REAL_NAME__ = dwValue;}

//申明导出该字符串
#define DECLARE_LUA_STRUCT_STRING(__VAR_NAME__, __STRING_LEN__, __REAL_NAME__)	\
	const char* get##__VAR_NAME__(void) {return __REAL_NAME__;};			\
	void set##__VAR_NAME__(const char* pszValue)							\
	{strncpy((__REAL_NAME__), pszValue, __STRING_LEN__);					\
	(__REAL_NAME__)[__STRING_LEN__ - 1] = 0;}

//申明导出该枚举型变量
#define DECLARE_LUA_STRUCT_ENUM(__VAR_NAME__, __REAL_NAME__)				\
	int get##__VAR_NAME__(void) {return (int)(__REAL_NAME__);};				\
	void set##__VAR_NAME__(int nValue)	{*(int*)&(__REAL_NAME__) = nValue;	}

//申明导出该布尔型变量
#define DECLARE_LUA_STRUCT_BOOL(__VAR_NAME__, __REAL_NAME__)				\
	int get##__VAR_NAME__(void) {return (int)(__REAL_NAME__);};				\
	void set##__VAR_NAME__(int nValue)	{*(int*)&(__REAL_NAME__) = nValue;}

/************************************************************************/

//开始定义导出类
#define DEFINE_LUA_CLASS_BEGIN(__CLASS_NAME__)								\
	const char __CLASS_NAME__::szClassName[] = #__CLASS_NAME__;				\
	Luna<__CLASS_NAME__>::KLuaData __CLASS_NAME__::LuaInterface[] = {

/************************************************************************/

//注册整形变量
#define REGISTER_LUA_INTEGER(__CLASS_NAME__, __VAR_NAME__)					\
	{"n"#__VAR_NAME__,															\
	 (Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::set##__VAR_NAME__),	\
	 (Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::get##__VAR_NAME__),	\
	 ldtInteger},

//注册无符号整形变量
#define REGISTER_LUA_DWORD(__CLASS_NAME__, __VAR_NAME__)					\
	{"dw"#__VAR_NAME__,															\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::set##__VAR_NAME__),	\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::get##__VAR_NAME__),	\
	ldtDword},

//注册字符串变量
#define REGISTER_LUA_STRING(__CLASS_NAME__, __VAR_NAME__)					\
	{"sz"#__VAR_NAME__,															\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::set##__VAR_NAME__),	\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::get##__VAR_NAME__),	\
	ldtString},

//注册枚举型变量
#define REGISTER_LUA_ENUM(__CLASS_NAME__, __VAR_NAME__)						\
	{"n"#__VAR_NAME__,															\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::set##__VAR_NAME__),	\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::get##__VAR_NAME__),	\
	ldtEnum},

//注册布尔变量
#define REGISTER_LUA_BOOL(__CLASS_NAME__, __VAR_NAME__)						\
	{"b"#__VAR_NAME__,															\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::set##__VAR_NAME__),	\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::get##__VAR_NAME__),	\
	ldtBool},

/************************************************************************/

//注册只读整形变量
#define REGISTER_LUA_INTEGER_READONLY(__CLASS_NAME__, __VAR_NAME__)			\
	{"n"#__VAR_NAME__,															\
	NULL,																	\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::get##__VAR_NAME__),	\
	ldtInteger},

//注册只读无符号整形变量
#define REGISTER_LUA_DWORD_READONLY(__CLASS_NAME__, __VAR_NAME__)			\
	{"dw"#__VAR_NAME__,															\
	NULL,																	\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::get##__VAR_NAME__),	\
	ldtDword},

//注册只读字符串变量
#define REGISTER_LUA_STRING_READONLY(__CLASS_NAME__, __VAR_NAME__)			\
	{"sz"#__VAR_NAME__,															\
	NULL,																	\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::get##__VAR_NAME__),	\
	ldtString},

//注册只读枚举型变量
#define REGISTER_LUA_ENUM_READONLY(__CLASS_NAME__, __VAR_NAME__)			\
	{"n"#__VAR_NAME__,															\
	NULL,																	\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::get##__VAR_NAME__),	\
	ldtEnum},

//注册只读布尔变量
#define REGISTER_LUA_BOOL_READONLY(__CLASS_NAME__, __VAR_NAME__)			\
	{"b"#__VAR_NAME__,															\
	NULL,																	\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::get##__VAR_NAME__),	\
	ldtBool},

/************************************************************************/

//注册导出函数
#define REGISTER_LUA_FUNC(__CLASS_NAME__, __FUNC_NAME__)					\
	{#__FUNC_NAME__,														\
	NULL,																	\
	(Luna<__CLASS_NAME__>::VoidFunc)(&__CLASS_NAME__::Lua##__FUNC_NAME__),	\
	ldtFunc},

/************************************************************************/

//定义导出类结束
#define DEFINE_LUA_CLASS_END(__CLASS_NAME__)								\
		{0, 0, 0, ldtInvalid}												\
	};

//Lua导出的数据类型，仅内部使用
enum LUA_DATA_TYPE
{
	ldtInvalid = 0,

	ldtInteger,
	ldtString,
	ldtDword,
	ldtEnum,
	ldtBool,
	ldtFunc,
	ldtClass,

	ldtTotal
};

//辅助模板类
template <typename T> 
class Luna
{
public:
	//注册类的对象指针和ID
	struct KObjData
	{ 
		T		*pObj;
	};
public:
	typedef void (T::*VoidFunc)(void);

	typedef void (T::*SetIntFunc)(int);
	typedef int (T::*GetIntFunc)(void);

	typedef void (T::*SetStrFunc)(const char*);
	typedef const char* (T::*GetStrFunc)(void);

	typedef void (T::*SetDwordFunc)(DWORD);
	typedef DWORD (T::*GetDwordFunc)(void);

	typedef int (T::*LuaFunc)(lua_State* L);

	typedef struct
	{
		const char*		pszName;		//导出变量或者函数的名称
		VoidFunc		pSetFunc;		//导出的变量读写接口函数或导出函数的指针
		VoidFunc		pGetFunc;
		LUA_DATA_TYPE	eDataType;
	}KLuaData;

	static int Register(lua_State *L) 
	{
		//创建导出数据表，主要为了利用Lua内建的字符串hash机制，否则也可以自己实现
		//表中Key : 导出数据名称；Value : KLuaData指针
		lua_newtable(L);
		int nLuaData = lua_gettop(L);

		//创建Metatable，以便重载相关的操作符
		luaL_newmetatable(L, T::szClassName);
		int metatable = lua_gettop(L);

		//将导出数据表加入全局表中，以便以后访问
		lua_pushstring(L, T::szClassName);
		lua_pushvalue(L, nLuaData);
		lua_settable(L, LUA_GLOBALSINDEX);

		lua_pushliteral(L, "__metatable");
		lua_pushvalue(L, nLuaData);
		lua_settable(L, metatable);  // hide metatable from Lua getmetatable()

		//重载GetTable操作
		lua_pushliteral(L, "__index");
		lua_pushcfunction(L, index);
		lua_settable(L, metatable);

		//重载SetTable操作
		lua_pushliteral(L, "__newindex");
		lua_pushcfunction(L, newindex);
		lua_settable(L, metatable);

		//重载字符串转换操作
		lua_pushliteral(L, "__tostring");
		lua_pushcfunction(L, tostring_T);
		lua_settable(L, metatable);

		//向表中加入所有需要注册的数据
		for (KLuaData* pLuaData = T::LuaInterface; pLuaData->pszName; pLuaData++) 
		{
			/* edited by Snaily: shouldn't it be const RegType *l ... ? */
			lua_pushstring(L, pLuaData->pszName);
			lua_pushlightuserdata(L, pLuaData);
			lua_settable(L, nLuaData);
		}

		lua_pop(L, 2);  // drop metatable and method table

		return 0;
	}

private:
	//隐藏构造函数
	Luna();

	//导出函数分派器，可以将static的调用，转换成有this指针的类成员函数调用
	static int Dispatcher(lua_State* L)
	{
		KObjData* pObjData = (KObjData*)lua_touserdata(L, lua_upvalueindex(1));
		KLuaData* pLuaData = (KLuaData*)lua_touserdata(L, lua_upvalueindex(2));
		
		return ((pObjData->pObj)->*(LuaFunc)(pLuaData->pGetFunc))(L);
	}

	//重载字符串转换函数
	static int tostring_T (lua_State* L) 
	{
		KObjData *pObjData = static_cast<KObjData*>(lua_touserdata(L, 1));
		lua_pushfstring(L, "Ptr:%p ClassName:%s",	pObjData->pObj, T::szClassName);
		
		return 1;
	}

	//重载GetTable操作
	static int index(lua_State* L)
	{
		//获取Self指向的数据
		KObjData* pObjData = (KObjData*)lua_touserdata(L, 1);
		//获取Key字符串
		const char* pszKey = lua_tostring(L, 2);

		if (pObjData == NULL || pObjData->pObj == NULL || pszKey == NULL)
		{
			printf("[LUA] Error index\n");
			return 0;
		}

		//查找导出数据表
		lua_pushstring(L, T::szClassName);
		lua_gettable(L, LUA_GLOBALSINDEX);
		//在导出数据表中查找Key对应的KLuaData指针
		lua_pushstring(L, pszKey);
		lua_gettable(L, -2);
		KLuaData* pLuaData = (KLuaData*)lua_touserdata(L, -1);
		if (pLuaData == NULL || pLuaData->pGetFunc == NULL)
		{
			printf("[LUA] Error index, classname = %s, key = %s\n", 
			T::szClassName, pszKey);
			return 0;
		}

		lua_settop(L, 2);

		switch (pLuaData->eDataType)
		{
		case ldtInteger:
			{
				int nResult = ((pObjData->pObj)->*(GetIntFunc)(pLuaData->pGetFunc))();
				lua_pushnumber(L, nResult);
				return 1;
			}
		case ldtString:
			{
				const char* pszResult = ((pObjData->pObj)->*(GetStrFunc)(pLuaData->pGetFunc))();
				lua_pushstring(L, pszResult);
				return 1;
			}
		case ldtEnum:
			{
				int nResult = ((pObjData->pObj)->*(GetIntFunc)(pLuaData->pGetFunc))();
				lua_pushnumber(L, nResult);
				return 1;
			}
		case ldtDword:
			{
				// NOTE:
				// 这里强转为int而不是DWORD，是为了避免CPU浮点运算的时候做修正处理，
				// 而CPU浮点单元有可能在real4模式下运行，这时候就有有精度丢失现象；
				// 如发现类似情况，可检查CTRL寄存器8、9两位是否为00，
				// 详细信息可参考MASM32浮点运算相关文档，或咨询飞舟
				int nResult = (int)((pObjData->pObj)->*(GetDwordFunc)(pLuaData->pGetFunc))();
				lua_pushnumber(L, nResult);
				return 1;
			}
		case ldtBool:
			{
				int nResult = ((pObjData->pObj)->*(GetIntFunc)(pLuaData->pGetFunc))();
				lua_pushboolean(L, nResult);
				return 1;
			}
		case ldtFunc:
			{
				lua_pushlightuserdata(L, pObjData);
				lua_pushlightuserdata(L, pLuaData);
				lua_pushcclosure(L, Dispatcher, 2);
				return 1;
			}
		}

		return 0;
	}

	//重载SetTable操作
	static int newindex(lua_State* L)
	{
		KObjData* pObjData = (KObjData*)lua_touserdata(L, 1);
		const char* pszKey = lua_tostring(L, 2);

		if (pObjData == NULL || pObjData->pObj == NULL || pszKey == NULL)
		{
			printf("[LUA]Error newindex\n");
			return 0;
		}

		lua_pushstring(L, T::szClassName);
		lua_gettable(L, LUA_GLOBALSINDEX);
		lua_pushstring(L, pszKey);
		lua_gettable(L, -2);
		KLuaData* pLuaData = (KLuaData*)lua_touserdata(L, -1);
		if (pLuaData == NULL || pLuaData->pSetFunc == NULL)
		{
			printf("[LUA]Error newindex, classname = %s, key = %s\n", 
				T::szClassName, pszKey);
			return 0;
		}

		lua_settop(L, 3);

		switch (pLuaData->eDataType)
		{
		case ldtInteger:
			{
				int nValue = (int)lua_tonumber(L, 3);
				((pObjData->pObj)->*(SetIntFunc)(pLuaData->pSetFunc))(nValue);
				return 0;
			}
		case ldtString:
			{
				const char* pszValue = lua_tostring(L, 3);
				((pObjData->pObj)->*(SetStrFunc)(pLuaData->pSetFunc))(pszValue);
				return 0;
			}
		case ldtEnum:
			{
				int nValue = (int)lua_tonumber(L, 3);
				((pObjData->pObj)->*(SetIntFunc)(pLuaData->pSetFunc))(nValue);
				return 0;
			}
		case ldtBool:
			{
				int nValue = (int)lua_tonumber(L, 3);
				((pObjData->pObj)->*(SetIntFunc)(pLuaData->pSetFunc))(nValue);
				return 0;
			}
		case ldtDword:
			{
				// NOTE:
				// 这里强转为int而不是DWORD，是为了避免CPU浮点运算的时候做修正处理，
				// 而CPU浮点单元有可能在real4模式下运行，这时候就有有精度丢失现象；
				// 如发现类似情况，可检查CTRL寄存器8、9两位是否为00，
				// 详细信息可参考MASM32浮点运算相关文档，或咨询飞舟
				int nValue = (int)lua_tonumber(L, 3);
				((pObjData->pObj)->*(SetDwordFunc)(pLuaData->pSetFunc))(nValue);
				return 0;
			}
		}

		return 0;
	}
};

#endif	//_LUNA_H_