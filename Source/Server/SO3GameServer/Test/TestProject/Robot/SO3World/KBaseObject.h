/************************************************************************/
/* 游戏世界基础对象						                                */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.01.01	Create												*/
/* Comment	 :															*/
/*		凡是游戏世界中需要有ID并且需要管理内存分配的对象，都从这里派生	*/
/************************************************************************/
#ifndef _KBASEOBJECT_H_
#define	_KBASEOBJECT_H_

#include "Luna.h"

class KBaseObject
{
public:
	DWORD			m_dwID;

	DECLARE_LUA_DWORD(ID);
};
#endif