// File: RenderCommon.h
// Desc:

#ifndef RENDERCOMMON_H
#define RENDERCOMMON_H

#include"..\Common\DLLAPI.h"

#include<d3d9.h>
#include<d3d9types.h>
#include<d3dx9math.h>

namespace Render
{

inline unsigned int GetSizeOfIndex(D3DFORMAT IndexFormat)
{
	return (IndexFormat == D3DFMT_INDEX16)? sizeof(WORD): sizeof(DWORD);
}

}  // namespace

#endif