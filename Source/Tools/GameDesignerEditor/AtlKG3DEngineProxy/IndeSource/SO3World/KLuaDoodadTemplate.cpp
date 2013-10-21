#include "stdafx.h"
#include "KDoodadTemplateList.h"

DEFINE_LUA_CLASS_BEGIN(KDoodadTemplate)
	REGISTER_LUA_DWORD(KDoodadTemplate, TemplateID)
	REGISTER_LUA_STRING(KDoodadTemplate, Name)	
	REGISTER_LUA_BOOL(KDoodadTemplate, Selectable)
	REGISTER_LUA_INTEGER(KDoodadTemplate, ReviveDelay)
	REGISTER_LUA_DWORD(KDoodadTemplate, OpenPrepareFrame)
	REGISTER_LUA_ENUM(KDoodadTemplate, Kind)
	REGISTER_LUA_DWORD(KDoodadTemplate, ClassID)
	REGISTER_LUA_INTEGER(KDoodadTemplate, Level)
	REGISTER_LUA_BOOL(KDoodadTemplate, CanOperateEach)
	REGISTER_LUA_DWORD(KDoodadTemplate, CraftID)

#if defined(_CLIENT)
	REGISTER_LUA_DWORD(KDoodadTemplate, RepresentID)
	REGISTER_LUA_STRING(KDoodadTemplate, BarText)
#endif	//_CLIENT

DEFINE_LUA_CLASS_END(KDoodadTemplate)
