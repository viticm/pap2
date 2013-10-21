#pragma once
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DResourceDelayReleaseMgr.h
//  Version     : 1.0
//  Creator     : yebixuan
//
////////////////////////////////////////////////////////////////////////////////

namespace KG3DResourceDelayReleaseMgr
{
    HRESULT Init();
    HRESULT UnInit();
    HRESULT Push(IUnknown *pResource);
    HRESULT ReleaseAll();
}
