////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DAnimationTable.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-2 10:00:01
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DANIMATIONTABLE_H_
#define _INCLUDE_IEKG3DANIMATIONTABLE_H_

#include "IEKG3DClip.h"

class IEKG3DClip;
interface IEKG3DAnimationTable
{
    virtual HRESULT LoadResourceFromFileInEditor(const char cszFileName[], unsigned uFileNameHash, unsigned uLoadOption, IEKG3DClip **ppiRetResource) = 0;
};
#endif //_INCLUDE_IEKG3DANIMATIONTABLE_H_
