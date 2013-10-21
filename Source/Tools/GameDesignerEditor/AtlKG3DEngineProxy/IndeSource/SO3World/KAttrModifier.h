/************************************************************************/
/* 属性修改函数集合						                                */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.01.26	Create												*/
/************************************************************************/
#ifndef _KATTR_MODIFIER_H_
#define _KATTR_MODIFIER_H_

#include "KAttribute.h"

class KCharacter;

BOOL InitAttributeFunctions();

typedef BOOL (*ATTR_PROCESS_FUN)(KCharacter *pDstCharacter,BOOL bApply, int nValue1, int nValue2);

extern ATTR_PROCESS_FUN g_AttributeProcessFunctions[];

#endif	//_KATTR_MODIFIER_H_
