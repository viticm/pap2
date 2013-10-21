#ifndef _INCLUDE_IEKG3DCLIPTABLE_H_
#define _INCLUDE_IEKG3DCLIPTABLE_H_

#include "IEKG3DClip.h"

interface IEKG3DClip;
interface IEKG3DClipTable
{
	virtual HRESULT LoadResourceFromFileInEditor(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, IEKG3DClip **ppiRetResource) = 0;
};

#endif //_INCLUDE_IEKG3DANIMATIONTABLE_H_